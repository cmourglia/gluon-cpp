#include "shader.h"

#include "gluon_app.h"

#include "loguru.hpp"

#include <spirv-headers/spirv.h>

#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <array>

struct Identifier
{
	SpvOp           opcode;
	u32             typeID;
	SpvStorageClass storageClass;
	u32             binding;
	u32             set;
	SpvDecoration   blockType;
};

constexpr u32 MAX_BINDING_COUNT = 32u;

namespace
{
inline VkShaderStageFlagBits GetShaderStage(SpvExecutionModel executionModel)
{
	switch (executionModel)
	{
		case SpvExecutionModelVertex:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case SpvExecutionModelFragment:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		case SpvExecutionModelGeometry:
			return VK_SHADER_STAGE_GEOMETRY_BIT;
		case SpvExecutionModelGLCompute:
			return VK_SHADER_STAGE_COMPUTE_BIT;
		default:
			assert(!"Unsupported shader type");
	}

	return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

inline void ParseShader(Shader* shader, const u32* code, u64 codeSize)
{
	assert(code[0] == SpvMagicNumber);

	u32 idsBound = code[3];

	std::vector<Identifier> ids(idsBound);

	const u32* ptr = code + 5;

	while (ptr < code + codeSize)
	{
		u16 opcode    = (u16)ptr[0];
		u16 wordCount = (u16)(ptr[0] >> 16);

		switch (opcode)
		{
			case SpvOpEntryPoint:
			{
				assert(wordCount >= 2);
				shader->stage = GetShaderStage((SpvExecutionModel)ptr[1]);
			}
			break;

			case SpvOpExecutionMode:
			{
				assert(wordCount >= 3);
				u32 mode = ptr[2];

				switch (mode)
				{
					case SpvExecutionModeLocalSize:
						assert(wordCount == 6);
						shader->localSizeX = ptr[3];
						shader->localSizeY = ptr[4];
						shader->localSizeZ = ptr[5];
				}
				break;
			}
			break;

			case SpvOpDecorate:
			{
				assert(wordCount >= 3);

				u32 id = ptr[1];
				assert(id < idsBound);

				switch (ptr[2])
				{
					case SpvDecorationDescriptorSet:
					{
						assert(wordCount == 4);
						ids[id].set = ptr[3];
					}
					break;

					case SpvDecorationBinding:
					{
						assert(wordCount == 4);
						ids[id].binding = ptr[3];
					}
					break;

					case SpvDecorationBlock:
					case SpvDecorationBufferBlock:
					{
						assert(wordCount == 3);
						ids[id].blockType = (SpvDecoration)ptr[2];
					}
					break;
				}
			}
			break;

			case SpvOpTypeStruct:
			case SpvOpTypeImage:
			case SpvOpTypeSampler:
			case SpvOpTypeSampledImage:
			{
				assert(wordCount >= 2);

				u32 id = ptr[1];
				assert(id < idsBound);

				assert(ids[id].opcode == 0);
				ids[id].opcode = (SpvOp)opcode;
			}
			break;

			case SpvOpTypePointer:
			{
				assert(wordCount == 4);

				u32 id = ptr[1];
				assert(id < idsBound);

				assert(ids[id].opcode == 0);
				ids[id].opcode       = (SpvOp)opcode;
				ids[id].typeID       = (SpvImageFormat)ptr[3];
				ids[id].storageClass = (SpvStorageClass)ptr[2];
			}
			break;

			case SpvOpVariable:
			{
				assert(wordCount >= 4);

				u32 id = ptr[2];
				assert(id < idsBound);

				assert(ids[id].opcode == 0);
				ids[id].opcode       = (SpvOp)opcode;
				ids[id].typeID       = (SpvImageFormat)ptr[1];
				ids[id].storageClass = (SpvStorageClass)ptr[3];
			}
			break;
		}

		assert(ptr + wordCount <= code + codeSize);
		ptr += wordCount;
	}

	for (auto&& id : ids)
	{
		if (id.opcode == SpvOpVariable &&
		    (id.storageClass == SpvStorageClassUniform || id.storageClass == SpvStorageClassUniformConstant ||
		     id.storageClass == SpvStorageClassStorageBuffer))
		{
			assert(id.set == 0);
			assert(id.binding < MAX_BINDING_COUNT);
			assert(ids[id.typeID].opcode == SpvOpTypePointer);

			assert((shader->resourceMask & (1 << id.binding)) == 0);

			const u32 typeKind  = ids[ids[id.typeID].typeID].opcode;
			const u32 blockType = ids[ids[id.typeID].typeID].blockType;

			switch (typeKind)
			{
				case SpvOpTypeStruct:
				{
					assert(blockType != 0);
					switch (blockType)
					{
						case SpvDecorationBlock:
						{
							shader->resourceTypes[id.binding] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						}
						break;

						case SpvDecorationBufferBlock:
						{
							shader->resourceTypes[id.binding] = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
						}
						break;

						default:
							assert(!"Block type not handled");
					}
					shader->resourceMask |= 1 << id.binding;
				}
				break;

				case SpvOpTypeImage:
				{
					shader->resourceTypes[id.binding] = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
					shader->resourceMask |= 1 << id.binding;
				}
				break;

				case SpvOpTypeSampledImage:
				{
					shader->resourceTypes[id.binding] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					shader->resourceMask |= 1 << id.binding;
				}
				break;

				default:
					assert(!"Resource type not handled");
			}
		}

		if (id.opcode == SpvOpVariable && id.storageClass == SpvStorageClassPushConstant)
		{
			assert(!"Shader push constants are not handled yet");
			shader->usePushConstants = true;
		}
	}
}
} // namespace

Shader Shader::Create(const char* filename)
{
	FILE* file = fopen(filename, "rb");

	if (nullptr == file)
	{
		LOG_F(ERROR, "Cannot load file %s", filename);
	}

	fseek(file, 0, SEEK_END);
	const u64 size = ftell(file);
	fseek(file, 0, SEEK_SET);

	std::vector<u8> bytes(size);
	const u64       read = fread(bytes.data(), 1, size, file);
	assert(read == size && size % 4 == 0);

	fclose(file);

	u32* code = (u32*)bytes.data();

	VkShaderModuleCreateInfo shaderCreateInfo = {
	    .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
	    .codeSize = size,
	    .pCode    = code,
	};

	Shader shader;

	VK_CHECK(vkCreateShaderModule(GluonApp::Get()->device, &shaderCreateInfo, nullptr, &shader.shader));

	ParseShader(&shader, code, size / 4);

	return shader;
}

void Shader::Destroy()
{
	if (shader != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(GluonApp::Get()->device, shader, nullptr);
	}
}

Program Program::Create(Shaders shaders, VkPipelineBindPoint bindPoint)
{
	VkDevice device = GluonApp::Get()->device;

	Program program;
	program.pipelineBindPoint = bindPoint;

	std::array<VkDescriptorType, MAX_BINDING_COUNT> resourceTypes;
	u32                                             resourceMask = 0;

	for (const Shader* shader : shaders)
	{
		for (u32 i = 0; i < MAX_BINDING_COUNT; ++i)
		{
			const u32 bindingBit = 1 << i;

			if (shader->resourceMask & bindingBit)
			{
				if (resourceMask & bindingBit)
				{
					assert(resourceTypes[i] == shader->resourceTypes[i]);
				}
				else
				{
					resourceTypes[i] = shader->resourceTypes[i];
					resourceMask |= bindingBit;
				}
			}
		}
	}

	for (u32 i = 0; i < MAX_BINDING_COUNT; ++i)
	{
		const u32 bindingBit = 1 << i;
		if (resourceMask & bindingBit)
		{
			VkDescriptorSetLayoutBinding binding = {
			    .binding         = i,
			    .descriptorType  = resourceTypes[i],
			    .descriptorCount = 1,
			    .stageFlags      = 0,
			};

			for (const Shader* shader : shaders)
			{
				if (shader->resourceMask & bindingBit)
				{
					binding.stageFlags |= shader->stage;
				}
			}

			program.setBindings.push_back(binding);
		}
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetCreateInfo = {
	    .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
	    .bindingCount = (u32)program.setBindings.size(),
	    .pBindings    = program.setBindings.data(),
	};

	VK_CHECK(vkCreateDescriptorSetLayout(device, &descriptorSetCreateInfo, nullptr, &program.descriptorSetLayout));

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
	    .sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	    .setLayoutCount = 1,
	    .pSetLayouts    = &program.descriptorSetLayout,
	};

	VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &program.pipelineLayout));

	return program;
}

void Program::Destroy()
{
	if (pipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(GluonApp::Get()->device, pipelineLayout, nullptr);
	}

	if (descriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(GluonApp::Get()->device, descriptorSetLayout, nullptr);
	}
}

VkPipeline CreatePipeline(VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, Shaders shaders)
{
	std::vector<VkPipelineShaderStageCreateInfo> stages;

	for (const Shader* shader : shaders)
	{
		VkPipelineShaderStageCreateInfo stageInfos = {
		    .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		    .stage  = shader->stage,
		    .module = shader->shader,
		    .pName  = "main",
		};

		stages.push_back(stageInfos);
	}

	auto vertexInputBindingDescription   = Vertex::VertexInputBindingDescription();
	auto vertexInputAttributeDescription = Vertex::VertexInputAttributeDescription();

	VkPipelineVertexInputStateCreateInfo vertexInputState = {
	    .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
	    .vertexBindingDescriptionCount   = (u32)vertexInputBindingDescription.size(),
	    .pVertexBindingDescriptions      = vertexInputBindingDescription.data(),
	    .vertexAttributeDescriptionCount = (u32)vertexInputAttributeDescription.size(),
	    .pVertexAttributeDescriptions    = vertexInputAttributeDescription.data(),
	};

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {
	    .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
	    .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
	    .primitiveRestartEnable = VK_TRUE,
	    // .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	};

	VkPipelineViewportStateCreateInfo viewportState = {
	    .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
	    .viewportCount = 1,
	    .scissorCount  = 1,
	};

	VkPipelineRasterizationStateCreateInfo rasterizationState = {
	    .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
	    .depthClampEnable        = VK_FALSE,
	    .rasterizerDiscardEnable = VK_FALSE,
	    .polygonMode             = VK_POLYGON_MODE_FILL,
	    .cullMode                = VK_CULL_MODE_BACK_BIT,
	    .depthBiasEnable         = VK_FALSE,
	    .lineWidth               = 1.0f,
	};

	VkPipelineMultisampleStateCreateInfo multisampleState = {
	    .sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
	    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
	    .sampleShadingEnable  = VK_FALSE,
	};

	VkPipelineDepthStencilStateCreateInfo depthStencilState = {
	    .sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
	    .depthTestEnable  = VK_TRUE,
	    .depthWriteEnable = VK_TRUE,
	    .depthCompareOp   = VK_COMPARE_OP_LESS_OR_EQUAL,
	    // .depthTestEnable  = VK_FALSE,
	    // .depthWriteEnable = VK_FALSE,
	    // .depthCompareOp   = VK_COMPARE_OP_ALWAYS,
	};

	VkPipelineColorBlendAttachmentState colorAttachmentState = {
	    .blendEnable         = VK_TRUE,
	    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
	    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	    .colorBlendOp        = VK_BLEND_OP_ADD,
	    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
	    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
	    .alphaBlendOp        = VK_BLEND_OP_ADD,
	    .colorWriteMask      = COLOR_MASK_RGBA,
	};

	VkPipelineColorBlendStateCreateInfo colorBlendState = {
	    .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
	    .logicOpEnable   = VK_FALSE,
	    .attachmentCount = 1,
	    .pAttachments    = &colorAttachmentState,
	};

	// VkStructureType                            sType;
	// const void*                                pNext;
	// VkPipelineColorBlendStateCreateFlags       flags;
	// VkBool32                                   logicOpEnable;
	// VkLogicOp                                  logicOp;
	// uint32_t                                   attachmentCount;
	// const VkPipelineColorBlendAttachmentState* pAttachments;
	// float                                      blendConstants[4];

	VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

	VkPipelineDynamicStateCreateInfo dynamicState = {
	    .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
	    .dynamicStateCount = ARRAY_SIZE(dynamicStates),
	    .pDynamicStates    = dynamicStates,
	};

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
	    .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
	    .stageCount          = (u32)stages.size(),
	    .pStages             = stages.data(),
	    .pVertexInputState   = &vertexInputState,
	    .pInputAssemblyState = &inputAssemblyState,
	    .pViewportState      = &viewportState,
	    .pRasterizationState = &rasterizationState,
	    .pMultisampleState   = &multisampleState,
	    .pDepthStencilState  = &depthStencilState,
	    .pColorBlendState    = &colorBlendState,
	    .pDynamicState       = &dynamicState,
	    .layout              = pipelineLayout,
	    .renderPass          = renderPass,
	};

	VkPipeline pipeline;
	VK_CHECK(
	    vkCreateGraphicsPipelines(GluonApp::Get()->device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline));

	return pipeline;
}
