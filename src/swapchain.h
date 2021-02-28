#pragma once

#include "vkhelpers.h"

#include <vector>

struct Swapchain
{
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;

	VkFormat                 format;
	u32                      width, height;
	std::vector<VkImage>     images;
	std::vector<VkImageView> imageViews;

	static Swapchain Create();
	void             Destroy();

	bool ResizeIfNecessary();

private:
	void CreateSwapchain(VkPhysicalDevice          gpu,
	                     VkDevice                  device,
	                     VkSurfaceKHR              surface,
	                     const QueueFamilyIndices& queueIndices,
	                     VkSwapchainKHR            oldSwapchain = VK_NULL_HANDLE);
};