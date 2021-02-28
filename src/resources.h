#pragma once

#include "vkcommon.h"

struct Buffer
{
	VkBuffer       buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	void*          data   = nullptr;
	VkDeviceSize   size   = 0ull;

	static Buffer Create(VkDeviceSize bufferSize, VkBufferUsageFlags usage, bool mapped = true);

	void CreateOrResize(VkDeviceSize bufferSize, VkBufferUsageFlags usage, bool mapped = true);

	void Destroy();

private:
	void CreateInternal(VkDeviceSize bufferSize, VkBufferUsageFlags usage, bool mapped);
};

VkBufferMemoryBarrier BufferBarrier(VkBuffer buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask);

struct Image
{
	VkImage        image     = VK_NULL_HANDLE;
	VkImageView    imageView = VK_NULL_HANDLE;
	VkDeviceMemory memory    = VK_NULL_HANDLE;

	static Image Create(u32 width, u32 height, u32 mipLevels, VkFormat format, VkImageUsageFlags usage);

	void Destroy();
};

VkImageMemoryBarrier ImageBarrier(VkImage            image,
                                  VkAccessFlags      srcAccessMask,
                                  VkAccessFlags      dstAccessMask,
                                  VkImageLayout      oldLayout,
                                  VkImageLayout      newLayout,
                                  VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);

VkSemaphore CreateSemaphore(VkDevice device);
VkFence     CreateFence(VkDevice device, bool signaled = true);