#pragma once

#include "vkcommon.h"
#include "vkhelpers.h"

#include "swapchain.h"
#include "shader.h"
#include "resources.h"

#include "types.h"

#include <glm/glm.hpp>

#include <vector>

struct GLFWwindow;

struct Vertex
{
	glm::vec2 position;
	float     index;

	static std::vector<VkVertexInputBindingDescription> VertexInputBindingDescription()
	{
		return {{
		    .binding   = 0,
		    .stride    = sizeof(Vertex),
		    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
		}};
	}

	static std::vector<VkVertexInputAttributeDescription> VertexInputAttributeDescription()
	{
		return {
		    {
		        .location = 0,
		        .binding  = 0,
		        .format   = VK_FORMAT_R32G32_SFLOAT,
		        .offset   = 0,
		    },
		    {
		        .location = 1,
		        .binding  = 0,
		        .format   = VK_FORMAT_R32_SFLOAT,
		        .offset   = offsetof(Vertex, index),
		    },
		};
	}
};

struct GluonApp
{
	static GluonApp* Get();

	GLFWwindow* window = nullptr;

	VkInstance               instance  = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
	VkPhysicalDevice         gpu       = VK_NULL_HANDLE;
	VkSurfaceKHR             surface   = VK_NULL_HANDLE;
	VkDevice                 device    = VK_NULL_HANDLE;

	VkQueue          graphicsQueue  = VK_NULL_HANDLE;
	VkQueue          presentQueue   = VK_NULL_HANDLE;
	VkRenderPass     renderPass     = VK_NULL_HANDLE;
	VkPipeline       pipeline       = VK_NULL_HANDLE;
	VkCommandPool    commandPool    = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

	Swapchain swapchain;
	Program   program;
	Image     depth;

	std::vector<VkFramebuffer>   framebuffers;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkDescriptorSet> descriptorSets;

	Buffer vertexBuffer, indexBuffer, rectangleBuffer;

	std::vector<Buffer> frameInfos;

	QueueFamilyIndices               queueIndices;
	VkPhysicalDeviceMemoryProperties memoryProperties;

	explicit GluonApp(int argc, char** argv);
	~GluonApp();

	int Run();

	void UpdateRectangles();

private:
	static GluonApp* s_instance;

	GluonApp(const GluonApp&) = delete;
	GluonApp(GluonApp&&)      = delete;
	void operator=(const GluonApp&) = delete;
	void operator=(GluonApp&&) = delete;

	std::vector<Vertex>        vertices;
	std::vector<u32>           indices;
	std::vector<RectangleInfo> rectangles;

	bool vertexBufferDirty    = true;
	bool rectangleBufferDirty = true;
};
