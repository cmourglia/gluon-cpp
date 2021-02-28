#include "swapchain.h"

#include "gluon_app.h"

#include <vector>

namespace
{
struct SwapchainSupportDetails
{
	VkSurfaceCapabilitiesKHR        capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>   presentModes;
};

SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice gpu, VkSurfaceKHR surface)
{
	SwapchainSupportDetails details = {};

	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &details.capabilities));

	u32 formatCount;
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr));
	details.formats.resize(formatCount);
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, details.formats.data()));

	u32 presentModeCount;
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, nullptr));
	details.presentModes.resize(presentModeCount);
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, details.presentModes.data()));

	return details;
}

VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
	for (const auto& format : formats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}

	return formats[0];
}

VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& presentModes)
{
	for (const auto& presentMode : presentModes)
	{
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return presentMode;
		}
	}

	// for (const auto& presentMode : presentModes)
	// {
	// 	if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
	// 	{
	// 		return presentMode;
	// 	}
	// }

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& caps, u32 width, u32 height)
{
	if (caps.currentExtent.width != UINT32_MAX)
	{
		return caps.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent;
		actualExtent.width  = Clamp(width, caps.minImageExtent.width, caps.maxImageExtent.width);
		actualExtent.height = Clamp(height, caps.minImageExtent.height, caps.maxImageExtent.height);
		return actualExtent;
	}
}

}

Swapchain Swapchain::Create()
{
	Swapchain swapchain;

	swapchain.CreateSwapchain(GluonApp::Get()->gpu,
	                          GluonApp::Get()->device,
	                          GluonApp::Get()->surface,
	                          GluonApp::Get()->queueIndices);

	return swapchain;
}

void Swapchain::Destroy()
{
	if (swapchain != VK_NULL_HANDLE)
	{
		vkDeviceWaitIdle(GluonApp::Get()->device);

		for (auto&& imageView : imageViews)
		{
			vkDestroyImageView(GluonApp::Get()->device, imageView, nullptr);
		}
		vkDestroySwapchainKHR(GluonApp::Get()->device, swapchain, nullptr);
	}
}

bool Swapchain::ResizeIfNecessary()
{
	VkSurfaceCapabilitiesKHR surfaceCaps;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GluonApp::Get()->gpu, GluonApp::Get()->surface, &surfaceCaps));

	const u32 w = surfaceCaps.currentExtent.width;
	const u32 h = surfaceCaps.currentExtent.height;

	if (w == width && h == height)
	{
		return false;
	}

	if (w == 0 || h == 0)
	{
		width  = 0;
		height = 0;
		return false;
	}

	auto oldSwapchain = swapchain;

	vkDeviceWaitIdle(GluonApp::Get()->device);
	for (auto&& imageView : imageViews)
	{
		vkDestroyImageView(GluonApp::Get()->device, imageView, nullptr);
	}

	CreateSwapchain(GluonApp::Get()->gpu,
	                GluonApp::Get()->device,
	                GluonApp::Get()->surface,
	                GluonApp::Get()->queueIndices,
	                oldSwapchain);

	VK_CHECK(vkDeviceWaitIdle(GluonApp::Get()->device));

	vkDestroySwapchainKHR(GluonApp::Get()->device, oldSwapchain, nullptr);

	return true;
}

void Swapchain::CreateSwapchain(VkPhysicalDevice          gpu,
                                VkDevice                  device,
                                VkSurfaceKHR              surface,
                                const QueueFamilyIndices& queueIndices,
                                VkSwapchainKHR            oldSwapchain)
{
	auto details = QuerySwapchainSupport(gpu, surface);

	auto surfaceFormat = ChooseSwapchainSurfaceFormat(details.formats);
	auto presentMode   = ChooseSwapchainPresentMode(details.presentModes);

	VkSurfaceCapabilitiesKHR surfaceCaps;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfaceCaps));

	const u32 w = surfaceCaps.currentExtent.width;
	const u32 h = surfaceCaps.currentExtent.height;

	u32 minImageCount = details.capabilities.maxImageCount > 0
	                        ? Min(details.capabilities.minImageCount + 1, details.capabilities.maxImageCount)
	                        : details.capabilities.minImageCount + 1;

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {
	    .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
	    .surface          = surface,
	    .minImageCount    = minImageCount,
	    .imageFormat      = surfaceFormat.format,
	    .imageColorSpace  = surfaceFormat.colorSpace,
	    .imageExtent      = {w, h},
	    .imageArrayLayers = 1,
	    .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
	    .preTransform     = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
	    .compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
	    .presentMode      = presentMode,
	    .clipped          = VK_TRUE,
	    .oldSwapchain     = oldSwapchain,
	};

	if (queueIndices.graphicsFamily != queueIndices.presentFamily)
	{
		const u32 indices[] = {queueIndices.graphicsFamily.value(), queueIndices.presentFamily.value()};
		swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices   = indices;
	}
	else
	{
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	VK_CHECK(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain));

	u32 imageCount = 0;
	VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr));

	images.resize(imageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data()));

	imageViews.resize(imageCount);

	for (u32 i = 0; i < imageCount; ++i)
	{
		VkImageViewCreateInfo imageViewCreateInfo = {
		    .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		    .image    = images[i],
		    .viewType = VK_IMAGE_VIEW_TYPE_2D,
		    .format   = surfaceFormat.format,
		    .components =
		        {
		            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
		            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
		            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
		            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
		        },
		    .subresourceRange =
		        {
		            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
		            .baseMipLevel   = 0,
		            .levelCount     = 1,
		            .baseArrayLayer = 0,
		            .layerCount     = 1,
		        },
		};

		VK_CHECK(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]));
	}

	width  = w;
	height = h;
	format = surfaceFormat.format;
}