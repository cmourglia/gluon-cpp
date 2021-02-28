#include "gluon_app.h"

#include "compiler/parser.h"
#include "vkhelpers.h"

#include <GLFW/glfw3.h>
#include <loguru.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <unordered_set>
#include <filesystem>
#include <chrono>

#include <assert.h>

GluonApp* GluonApp::s_instance = nullptr;

struct FrameInfos
{
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec2 viewport;
};

constexpr u32 MAX_FRAMES_IN_FLIGHT = 2u;

std::vector<glm::vec4> defaultColors = {
    {1.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 1.0f, 1.0f},
    {0.0f, 1.0f, 1.0f, 1.0f},
};

namespace
{
std::vector<const char*> GetRequiredExtensions()
{
	u32          glfwExtensionCount = 0;
	const char** glfwExtensions     = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> result(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef _DEBUG
	result.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	return result;
}

bool CheckValidationLayersSupport(const u32 validationLayerCount, const char** validationLayers)
{
	u32 layerCount;
	VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));

	std::vector<VkLayerProperties> availableLayers(layerCount);
	VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

	for (u32 i = 0; i < validationLayerCount; ++i)
	{
		bool layerFound = false;

		for (const auto& props : availableLayers)
		{
			if (strcmp(props.layerName, validationLayers[i]) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

std::vector<const char*> GetRequiredLayers()
{
#ifdef _DEBUG
	static const char* s_validationLayers[]   = {"VK_LAYER_KHRONOS_validation"};
	static const u32   s_validationLayerCount = ARRAY_SIZE(s_validationLayers);
	if (!CheckValidationLayersSupport(s_validationLayerCount, s_validationLayers))
	{
		assert(false);
	}

	return std::vector<const char*>(s_validationLayers, s_validationLayers + s_validationLayerCount);
#endif
	return std::vector<const char*>();
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void*                                       pUserData)
{
	if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		LOG_F(ERROR, "%s: %s", pCallbackData->pMessageIdName, pCallbackData->pMessage);
	}
	else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		LOG_F(WARNING, "%s: %s", pCallbackData->pMessageIdName, pCallbackData->pMessage);
	}

	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance                                instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks*              pAllocator,
                                      VkDebugUtilsMessengerEXT*                 pMessenger)
{
	auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (fn != nullptr)
	{
		return fn(instance, pCreateInfo, pAllocator, pMessenger);
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance               instance,
                                   VkDebugUtilsMessengerEXT messenger,
                                   VkAllocationCallbacks*   pAllocator)
{
	auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (fn != nullptr)
	{
		fn(instance, messenger, pAllocator);
	}
}

void KeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) { }
} // namespace

GluonApp* GluonApp::Get()
{
	return s_instance;
}

GluonApp::GluonApp(int argc, char** argv)
{
	assert(s_instance == nullptr);
	s_instance = this;

	// TODO: Extract window infos if set

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(1024, 768, "Gluon App", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	// TODO: Setup callbacks

	glfwSetKeyCallback(window, KeyCallback);

	VK_CHECK(volkInitialize());

	// Create instance
	auto extensions = GetRequiredExtensions();
	auto layers     = GetRequiredLayers();

	VkApplicationInfo appInfo = {
	    .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
	    .pApplicationName   = "Gluon",
	    .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
	    .pEngineName        = "Gluon",
	    .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
	    .apiVersion         = VK_API_VERSION_1_1,
	};

	VkInstanceCreateInfo instanceInfo = {
	    .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	    .pApplicationInfo        = &appInfo,
	    .enabledLayerCount       = (u32)layers.size(),
	    .ppEnabledLayerNames     = layers.data(),
	    .enabledExtensionCount   = (u32)extensions.size(),
	    .ppEnabledExtensionNames = extensions.data(),
	};
	VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &instance));

	volkLoadInstance(instance);

#ifdef _DEBUG
	// Create debug messenger
	VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {
	    .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
	    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
	                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
	    .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
	                   VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
	    .pfnUserCallback = DebugCallback,
	    .pUserData       = this,
	};

	VK_CHECK(CreateDebugUtilsMessengerEXT(instance, &messengerCreateInfo, nullptr, &messenger));
#endif

	VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));

	u32 gpuCount = 0;
	vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
	std::vector<VkPhysicalDevice> gpus(gpuCount);
	vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());

	for (const auto& currentGpu : gpus)
	{
		auto indices = FindQueueFamilies(currentGpu, surface);
		if (indices.IsComplete())
		{
			queueIndices = indices;
			gpu          = currentGpu;
			break;
		}
	}

	assert(gpu != VK_NULL_HANDLE);

	static const char* deviceExtensions[]   = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME};
	static const u32   deviceExtensionCount = ARRAY_SIZE(deviceExtensions);

	static const f32        queuePriorities[] = {1.0f};
	std::unordered_set<u32> uniqueIndices = {queueIndices.graphicsFamily.value(), queueIndices.presentFamily.value()};

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (u32 index : uniqueIndices)
	{
		queueCreateInfos.emplace_back(VkDeviceQueueCreateInfo{
		    .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		    .queueFamilyIndex = index,
		    .queueCount       = 1,
		    .pQueuePriorities = queuePriorities,
		});
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.geometryShader           = true;

	VkDeviceCreateInfo deviceCreateInfo = {
	    .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
	    .queueCreateInfoCount    = (u32)queueCreateInfos.size(),
	    .pQueueCreateInfos       = queueCreateInfos.data(),
	    .enabledExtensionCount   = deviceExtensionCount,
	    .ppEnabledExtensionNames = deviceExtensions,
	    .pEnabledFeatures        = &deviceFeatures,
	};

	VK_CHECK(vkCreateDevice(gpu, &deviceCreateInfo, nullptr, &device));

	vkGetDeviceQueue(device, queueIndices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, queueIndices.presentFamily.value(), 0, &presentQueue);

	swapchain = Swapchain::Create();

	Shader vshader = Shader::Create("shaders/bin/triangle.vert.spv");
	Shader gshader = Shader::Create("shaders/bin/triangle.geom.spv");
	Shader fshader = Shader::Create("shaders/bin/triangle.frag.spv");

	// Render pass attachments
	VkAttachmentDescription attachments[] = {
	    // Color attachment
	    VkAttachmentDescription{
	        .format         = swapchain.format,
	        .samples        = VK_SAMPLE_COUNT_1_BIT,
	        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
	        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
	        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
	        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
	        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
	        .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	    },

	    // Depth attachment
	    VkAttachmentDescription{
	        .format         = VK_FORMAT_D32_SFLOAT,
	        .samples        = VK_SAMPLE_COUNT_1_BIT,
	        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
	        .storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE,
	        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
	        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
	        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
	        .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	    },
	};

	VkAttachmentReference colorReference = {
	    .attachment = 0,
	    .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};

	VkAttachmentReference depthReference = {
	    .attachment = 1,
	    .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	};

	VkSubpassDescription subpassDescription = {
	    .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
	    .colorAttachmentCount    = 1,
	    .pColorAttachments       = &colorReference,
	    .pDepthStencilAttachment = &depthReference,
	};

	VkSubpassDependency subpassDependency = {
	    .srcSubpass    = VK_SUBPASS_EXTERNAL,
	    .dstSubpass    = 0,
	    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    .srcAccessMask = 0,
	    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	};

	VkRenderPassCreateInfo renderPassCreateInfo = {
	    .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
	    .attachmentCount = ARRAY_SIZE(attachments),
	    .pAttachments    = attachments,
	    .subpassCount    = 1,
	    .pSubpasses      = &subpassDescription,
	    .dependencyCount = 1,
	    .pDependencies   = &subpassDependency,
	};

	VK_CHECK(vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass));

	program  = Program::Create({&vshader, &gshader, &fshader}, VK_PIPELINE_BIND_POINT_GRAPHICS);
	pipeline = CreatePipeline(device, renderPass, program.pipelineLayout, {&vshader, &gshader, &fshader});

	vshader.Destroy();
	gshader.Destroy();
	fshader.Destroy();

	VkCommandPoolCreateInfo commandPoolCreateInfo = {
	    .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
	    .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
	    .queueFamilyIndex = queueIndices.graphicsFamily.value(),
	};

	VK_CHECK(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));

	vkGetPhysicalDeviceMemoryProperties(gpu, &memoryProperties);

	std::vector<VkDescriptorPoolSize> poolSizes;
	for (const auto& binding : program.setBindings)
	{
		poolSizes.push_back({
		    .type            = binding.descriptorType,
		    .descriptorCount = binding.descriptorCount * (u32)swapchain.images.size(),
		});
	}

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
	    .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
	    .flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
	    .maxSets       = (u32)swapchain.images.size(),
	    .poolSizeCount = (u32)poolSizes.size(),
	    .pPoolSizes    = poolSizes.data(),
	};
	VK_CHECK(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));
}

GluonApp::~GluonApp()
{
	for (auto&& framebuffer : framebuffers)
	{
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	for (auto&& buffer : frameInfos)
	{
		buffer.Destroy();
	}

	rectangleBuffer.Destroy();
	indexBuffer.Destroy();
	vertexBuffer.Destroy();

	if (descriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	}

	if (commandPool != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(device, commandPool, (u32)commandBuffers.size(), commandBuffers.data());
		vkDestroyCommandPool(device, commandPool, nullptr);
	}

	if (pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device, pipeline, nullptr);
	}

	program.Destroy();

	if (renderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(device, renderPass, nullptr);
	}

	depth.Destroy();
	swapchain.Destroy();

	if (device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(device, nullptr);
	}

	if (surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(instance, surface, nullptr);
	}

	if (messenger != VK_NULL_HANDLE)
	{
		DestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
	}

	if (instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(instance, nullptr);
	}

	if (window)
	{
		glfwDestroyWindow(window);
	}
	glfwTerminate();
}

int GluonApp::Run()
{
	std::vector<VkSemaphore> imageAvailableSemaphores(MAX_FRAMES_IN_FLIGHT);
	std::vector<VkSemaphore> renderFinishedSemaphores(MAX_FRAMES_IN_FLIGHT);
	std::vector<VkFence>     inFlightFences(MAX_FRAMES_IN_FLIGHT);
	std::vector<VkFence>     inFlightImages(swapchain.images.size(), VK_NULL_HANDLE);

	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		imageAvailableSemaphores[i] = CreateSemaphore(device);
		renderFinishedSemaphores[i] = CreateSemaphore(device);
		inFlightFences[i]           = CreateFence(device);
	}

	glfwSwapInterval(0);

	u32 currentFrame = 0;

	f64 t0         = glfwGetTime();
	u32 frameCount = 0;

	glm::mat4 proj;
	glm::mat4 view = glm::mat4(1.0f); // Identity

	namespace fs                     = std::filesystem;
	fs::file_time_type lastWriteTime = {};

	while (!glfwWindowShouldClose(window))
	{
		if (++frameCount == 100)
		{
			const f64 t1 = glfwGetTime();
			const f64 t  = t1 - t0;
			t0           = t1;
			frameCount   = 0;

			printf("Average time (over 100 frames): %fms (%ffps). Rendering %d rectangles.\n",
			       f32(t / 100) * 1000.0f,
			       100.f / t,
			       (i32)rectangles.size());
		}

		glfwPollEvents();

		// Check if gluon file update is needed
		auto writeTime = fs::last_write_time("test.gluon");
		if (writeTime > lastWriteTime)
		{
			rectangles = ParseGluonFile("test.gluon");
			UpdateRectangles();
			lastWriteTime = writeTime;
		}

		VK_CHECK(vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX));

		if (swapchain.ResizeIfNecessary() || framebuffers.empty())
		{
			for (auto&& framebuffer : framebuffers)
			{
				vkDestroyFramebuffer(device, framebuffer, nullptr);
			}

			for (auto&& buffer : frameInfos)
			{
				buffer.Destroy();
			}

			if (!commandBuffers.empty())
			{
				vkFreeCommandBuffers(device, commandPool, commandBuffers.size(), commandBuffers.data());
				commandBuffers.clear();
			}

			if (!descriptorSets.empty())
			{
				vkFreeDescriptorSets(device, descriptorPool, descriptorSets.size(), descriptorSets.data());
				descriptorSets.clear();
			}

			depth.Destroy();
			depth = Image::Create(swapchain.width,
			                      swapchain.height,
			                      1,
			                      VK_FORMAT_D32_SFLOAT,
			                      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

			u32 imageCount = (u32)swapchain.images.size();

			framebuffers.resize(imageCount);
			commandBuffers.resize(imageCount);
			descriptorSets.resize(imageCount);
			frameInfos.resize(imageCount);

			for (u32 i = 0; i < imageCount; ++i)
			{
				VkImageView attachments[] = {swapchain.imageViews[i], depth.imageView};

				VkFramebufferCreateInfo framebufferCreateInfo = {
				    .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				    .renderPass      = renderPass,
				    .attachmentCount = ARRAY_SIZE(attachments),
				    .pAttachments    = attachments,
				    .width           = swapchain.width,
				    .height          = swapchain.height,
				    .layers          = 1,
				};

				VK_CHECK(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffers[i]));
			}

			for (u32 i = 0; i < imageCount; ++i)
			{
				VkCommandBufferAllocateInfo commandBufferAllocInfo = {
				    .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				    .commandPool        = commandPool,
				    .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				    .commandBufferCount = 1,
				};
				VK_CHECK(vkAllocateCommandBuffers(device, &commandBufferAllocInfo, &commandBuffers[i]));
			}

			std::vector<VkDescriptorSetLayout> setLayouts(imageCount, program.descriptorSetLayout);

			VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {
			    .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			    .descriptorPool     = descriptorPool,
			    .descriptorSetCount = imageCount,
			    .pSetLayouts        = setLayouts.data(),
			};

			VK_CHECK(vkAllocateDescriptorSets(device, &descriptorSetAllocInfo, descriptorSets.data()));

			for (u32 i = 0; i < imageCount; ++i)
			{
				frameInfos[i] = Buffer::Create(sizeof(FrameInfos), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
			}

			// RecordCommandBuffers();

			inFlightImages.resize(swapchain.images.size(), VK_NULL_HANDLE);

			proj = glm::ortho(0.0f, (f32)swapchain.width, 0.0f, (f32)swapchain.height, -1.0f, 1.0f);

			// UpdateRectangles();
		}

		if (swapchain.width == 0 || swapchain.height == 0)
			continue;

		u32 imageIndex = 0;
		VK_CHECK(vkAcquireNextImageKHR(device,
		                               swapchain.swapchain,
		                               ~0ull,
		                               imageAvailableSemaphores[currentFrame],
		                               VK_NULL_HANDLE,
		                               &imageIndex));

		if (inFlightImages[imageIndex] != VK_NULL_HANDLE)
		{
			VK_CHECK(vkWaitForFences(device, 1, &inFlightImages[imageIndex], VK_TRUE, UINT64_MAX));
		}
		inFlightImages[imageIndex] = inFlightFences[currentFrame];

		// Upload data
		u64 vertexSize    = vertices.size() * sizeof(Vertex);
		u64 indexSize     = indices.size() * sizeof(u32);
		u64 rectangleSize = rectangles.size() * sizeof(RectangleInfo);

		if (vertexSize > 0 && indexSize > 0)
		{
			if (vertexBufferDirty)
			{
				if (vertexBuffer.size < vertexSize)
				{
					vkDeviceWaitIdle(device);
					vertexBuffer.CreateOrResize(vertexSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
					indexBuffer.CreateOrResize(indexSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
				}

				memcpy(vertexBuffer.data, vertices.data(), vertexSize);
				memcpy(indexBuffer.data, indices.data(), indexSize);

				vertexBufferDirty = false;
			}

			if (rectangleBufferDirty)
			{
				if (rectangleBuffer.size < rectangleSize)
				{
					vkDeviceWaitIdle(device);
					rectangleBuffer.CreateOrResize(rectangleSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
				}

				memcpy(rectangleBuffer.data, rectangles.data(), rectangleSize);

				rectangleBufferDirty = false;
			}
		}

		VkCommandBufferBeginInfo commandBufferBeginInfo = {
		    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		};
		VK_CHECK(vkBeginCommandBuffer(commandBuffers[imageIndex], &commandBufferBeginInfo));

		VkClearValue clearValues[2] = {
		    // {.color = {0.7f, 0.3f, 0.6f, 1.0f}},
		    {.color = {0.7f, 0.7f, 0.7f, 1.0f}},
		    {.depthStencil = {1.0f, 0}},
		};

		VkRenderPassBeginInfo renderPassBeginInfo = {
		    .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		    .renderPass  = renderPass,
		    .framebuffer = framebuffers[imageIndex],
		    .renderArea =
		        {
		            .offset = {0, 0},
		            .extent = {swapchain.width, swapchain.height},
		        },
		    .clearValueCount = ARRAY_SIZE(clearValues),
		    .pClearValues    = clearValues,
		};
		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {0.0f, (f32)swapchain.height, (f32)swapchain.width, -(f32)swapchain.height, 0.0f, 1.0};
		VkRect2D   scissor  = {{0, 0}, {swapchain.width, swapchain.height}};

		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		if (!rectangles.empty())
		{
			vkCmdBindIndexBuffer(commandBuffers[imageIndex], indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

			VkDeviceSize offset = 0;
			vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, &vertexBuffer.buffer, &offset);

			VkDescriptorBufferInfo rectangleInfosBufferInfo = {
			    .buffer = rectangleBuffer.buffer,
			    .offset = 0,
			    .range  = VK_WHOLE_SIZE,
			};

			VkDescriptorBufferInfo frameInfosBufferInfo = {
			    .buffer = frameInfos[imageIndex].buffer,
			    .offset = 0,
			    .range  = VK_WHOLE_SIZE,
			};

			VkWriteDescriptorSet descriptorWrites[] = {
			    {
			        .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			        .dstSet          = descriptorSets[imageIndex],
			        .dstBinding      = 0,
			        .dstArrayElement = 0,
			        .descriptorCount = 1,
			        .descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			        .pBufferInfo     = &rectangleInfosBufferInfo,
			    },
			    {
			        .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			        .dstSet          = descriptorSets[imageIndex],
			        .dstBinding      = 1,
			        .dstArrayElement = 0,
			        .descriptorCount = 1,
			        .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			        .pBufferInfo     = &frameInfosBufferInfo,
			    },
			};

			vkUpdateDescriptorSets(device, ARRAY_SIZE(descriptorWrites), descriptorWrites, 0, nullptr);
			vkCmdBindDescriptorSets(commandBuffers[imageIndex],
			                        program.pipelineBindPoint,
			                        program.pipelineLayout,
			                        0,
			                        1,
			                        &descriptorSets[imageIndex],
			                        0,
			                        nullptr);

			vkCmdDrawIndexed(commandBuffers[imageIndex], indices.size(), 1, 0, 0, 0);
		}

		vkCmdEndRenderPass(commandBuffers[imageIndex]);

		VK_CHECK(vkEndCommandBuffer(commandBuffers[imageIndex]));

		FrameInfos currentFrameInfos = {
		    .view     = view,
		    .proj     = proj,
		    .viewport = glm::vec2((f32)swapchain.width, (f32)swapchain.height),
		};
		memcpy(frameInfos[imageIndex].data, &currentFrameInfos, sizeof(currentFrameInfos));

		VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submitInfo = {
		    .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		    .waitSemaphoreCount   = 1,
		    .pWaitSemaphores      = &imageAvailableSemaphores[currentFrame],
		    .pWaitDstStageMask    = &pipelineStageFlags,
		    .commandBufferCount   = 1,
		    .pCommandBuffers      = &commandBuffers[imageIndex],
		    .signalSemaphoreCount = 1,
		    .pSignalSemaphores    = &renderFinishedSemaphores[currentFrame],
		};

		VK_CHECK(vkResetFences(device, 1, &inFlightFences[currentFrame]));
		VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]));

		VkPresentInfoKHR presentInfo = {
		    .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		    .waitSemaphoreCount = 1,
		    .pWaitSemaphores    = &renderFinishedSemaphores[currentFrame],
		    .swapchainCount     = 1,
		    .pSwapchains        = &swapchain.swapchain,
		    .pImageIndices      = &imageIndex,
		};

		VK_CHECK(vkQueuePresentKHR(presentQueue, &presentInfo));

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	VK_CHECK(vkDeviceWaitIdle(device));

	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vkDestroyFence(device, inFlightFences[i], nullptr);
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
	}

	return 0;
}

void GluonApp::UpdateRectangles()
{
	vertices.clear();
	vertices.reserve(rectangles.size() * 4);

	indices.clear();
	indices.reserve(rectangles.size() * 5);

	u32 i = 0;
	for (const auto& r : rectangles)
	{
		const glm::vec2 p = r.position;
		const glm::vec2 s = r.size * 0.5f;

		const f32 l = p.x - s.x;
		const f32 r = p.x + s.x;
		const f32 b = p.y - s.y;
		const f32 t = p.y + s.y;

		vertices.push_back({{l, b}, (f32)i});
		vertices.push_back({{r, b}, (f32)i});
		vertices.push_back({{l, t}, (f32)i});
		vertices.push_back({{r, t}, (f32)i});

		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
		indices.push_back(0xFFFFFFFF);

		++i;
	}

	vertexBufferDirty    = true;
	rectangleBufferDirty = true;
}
