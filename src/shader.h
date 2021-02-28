#pragma once

#include "vkcommon.h"

#include <initializer_list>
#include <vector>

struct Shader
{
	VkShaderModule        shader = VK_NULL_HANDLE;
	VkShaderStageFlagBits stage  = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

	VkDescriptorType resourceTypes[32] = {};
	u32              resourceMask      = 0;

	u32 localSizeX = 0;
	u32 localSizeY = 0;
	u32 localSizeZ = 0;

	bool usePushConstants = false;

	static Shader Create(const char* filename);
	void          Destroy();
};

using Shaders = std::initializer_list<const Shader*>;

struct Program
{
	VkPipelineBindPoint   pipelineBindPoint   = VK_PIPELINE_BIND_POINT_MAX_ENUM;
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout      pipelineLayout      = VK_NULL_HANDLE;

	std::vector<VkDescriptorSetLayoutBinding> setBindings;

	static Program Create(Shaders shaders, VkPipelineBindPoint bindPoint);
	void           Destroy();
};

struct DescriptorInfo
{
	VkDescriptorImageInfo  image  = {};
	VkDescriptorBufferInfo buffer = {};

	DescriptorInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout)
	{
		image.sampler     = sampler;
		image.imageView   = imageView;
		image.imageLayout = imageLayout;
	}

	DescriptorInfo(VkImageView imageView, VkImageLayout imageLayout)
	{
		image.sampler     = VK_NULL_HANDLE;
		image.imageView   = imageView;
		image.imageLayout = imageLayout;
	}

	DescriptorInfo(VkBuffer pBuffer, VkDeviceSize offset, VkDeviceSize range)
	{
		buffer.buffer = pBuffer;
		buffer.offset = offset;
		buffer.range  = range;
	}

	explicit DescriptorInfo(VkBuffer pBuffer)
	{
		buffer.buffer = pBuffer;
		buffer.offset = 0;
		buffer.range  = VK_WHOLE_SIZE;
	}
};

// FIXME: This needs to be more specific
VkPipeline CreatePipeline(VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, Shaders shaders);