#include "resources.h"

#include "gluon_app.h"

namespace
{
u32 SelectMemoryType(u32 memoryTypeBits, u32 usage)
{
	VkPhysicalDeviceMemoryProperties memoryProperties = GluonApp::Get()->memoryProperties;

	for (u32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
	{
		if ((memoryTypeBits & (1 << i)) != 0 && (memoryProperties.memoryTypes[i].propertyFlags & usage) == usage)
		{
			return i;
		}
	}
	assert(false);
	return ~0u;
}
}

Buffer Buffer::Create(VkDeviceSize bufferSize, VkBufferUsageFlags usage, bool mapped)
{
	Buffer buffer;
	buffer.CreateInternal(bufferSize, usage, mapped);

	return buffer;
}

void Buffer::CreateOrResize(VkDeviceSize bufferSize, VkBufferUsageFlags usage, bool mapped)
{
	Destroy();
	CreateInternal(bufferSize, usage, mapped);
}

void Buffer::Destroy()
{
	if (data != nullptr)
	{
		vkUnmapMemory(GluonApp::Get()->device, memory);
	}

	if (memory != VK_NULL_HANDLE)
	{
		vkFreeMemory(GluonApp::Get()->device, memory, nullptr);
	}

	if (buffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(GluonApp::Get()->device, buffer, nullptr);
	}

	buffer = VK_NULL_HANDLE;
	memory = VK_NULL_HANDLE;
	data   = nullptr;
	size   = 0ull;
}

void Buffer::CreateInternal(VkDeviceSize bufferSize, VkBufferUsageFlags usage, bool mapped)
{
	VkBufferCreateInfo info = {
	    .sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	    .size        = bufferSize,
	    .usage       = usage,
	    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};

	VK_CHECK(vkCreateBuffer(GluonApp::Get()->device, &info, nullptr, &buffer));

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(GluonApp::Get()->device, buffer, &memoryRequirements);

	u32 memoryType = mapped ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	                        : VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

	u32 memoryTypeIndex = SelectMemoryType(memoryRequirements.memoryTypeBits, memoryType);

	VkMemoryAllocateInfo allocInfo = {
	    .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
	    .allocationSize  = memoryRequirements.size,
	    .memoryTypeIndex = memoryTypeIndex,
	};

	VK_CHECK(vkAllocateMemory(GluonApp::Get()->device, &allocInfo, nullptr, &memory));
	VK_CHECK(vkBindBufferMemory(GluonApp::Get()->device, buffer, memory, 0));

	if (mapped)
	{
		VK_CHECK(vkMapMemory(GluonApp::Get()->device, memory, 0, bufferSize, 0, &data));
	}

	size = bufferSize;
}

VkBufferMemoryBarrier BufferBarrier(VkBuffer buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask)
{
	VkBufferMemoryBarrier barrier = {
	    .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
	    .srcAccessMask       = srcAccessMask,
	    .dstAccessMask       = dstAccessMask,
	    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .buffer              = buffer,
	    .offset              = 0,
	    .size                = VK_WHOLE_SIZE,
	};

	return barrier;
}

Image Image::Create(u32 width, u32 height, u32 mipLevels, VkFormat format, VkImageUsageFlags usage)
{
	Image image;

	VkImageCreateInfo imageCreateInfo = {
	    .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
	    .imageType     = VK_IMAGE_TYPE_2D,
	    .format        = format,
	    .extent        = {width, height, 1},
	    .mipLevels     = mipLevels,
	    .arrayLayers   = 1,
	    .samples       = VK_SAMPLE_COUNT_1_BIT,
	    .tiling        = VK_IMAGE_TILING_OPTIMAL,
	    .usage         = usage,
	    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	VK_CHECK(vkCreateImage(GluonApp::Get()->device, &imageCreateInfo, nullptr, &image.image));

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(GluonApp::Get()->device, image.image, &memoryRequirements);

	u32 memoryTypeIndex = SelectMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	assert(memoryTypeIndex != ~0u);

	VkMemoryAllocateInfo memoryAllocInfo = {
	    .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
	    .allocationSize  = memoryRequirements.size,
	    .memoryTypeIndex = memoryTypeIndex,
	};

	VK_CHECK(vkAllocateMemory(GluonApp::Get()->device, &memoryAllocInfo, nullptr, &image.memory));
	VK_CHECK(vkBindImageMemory(GluonApp::Get()->device, image.image, image.memory, 0));

	const VkImageAspectFlags aspectMask = (format == VK_FORMAT_D32_SFLOAT) ? VK_IMAGE_ASPECT_DEPTH_BIT
	                                                                       : VK_IMAGE_ASPECT_COLOR_BIT;

	VkImageViewCreateInfo imageViewCreateInfo = {
	    .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
	    .image    = image.image,
	    .viewType = VK_IMAGE_VIEW_TYPE_2D,
	    .format   = format,
	    .subresourceRange =
	        {
	            .aspectMask   = aspectMask,
	            .baseMipLevel = 0,
	            .levelCount   = mipLevels,
	            .layerCount   = 1,
	        },
	};
	VK_CHECK(vkCreateImageView(GluonApp::Get()->device, &imageViewCreateInfo, nullptr, &image.imageView));

	return image;
}

void Image::Destroy()
{
	if (imageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(GluonApp::Get()->device, imageView, nullptr);
	}

	if (memory != VK_NULL_HANDLE)
	{
		vkFreeMemory(GluonApp::Get()->device, memory, nullptr);
	}

	if (image != VK_NULL_HANDLE)
	{
		vkDestroyImage(GluonApp::Get()->device, image, nullptr);
	}
}

VkImageMemoryBarrier ImageBarrier(VkImage            image,
                                  VkAccessFlags      srcAccessMask,
                                  VkAccessFlags      dstAccessMask,
                                  VkImageLayout      oldLayout,
                                  VkImageLayout      newLayout,
                                  VkImageAspectFlags aspectMask)
{
	VkImageMemoryBarrier barrier = {
	    .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
	    .srcAccessMask       = srcAccessMask,
	    .dstAccessMask       = dstAccessMask,
	    .oldLayout           = oldLayout,
	    .newLayout           = newLayout,
	    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .image               = image,
	    .subresourceRange =
	        {
	            .aspectMask = aspectMask,
	            .levelCount = VK_REMAINING_MIP_LEVELS,
	            .layerCount = VK_REMAINING_ARRAY_LAYERS,
	        },
	};

	return barrier;
}

VkSemaphore CreateSemaphore(VkDevice device)
{
	VkSemaphoreCreateInfo createInfo = {
	    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	};

	VkSemaphore semaphore;
	VK_CHECK(vkCreateSemaphore(device, &createInfo, nullptr, &semaphore));

	return semaphore;
}

VkFence CreateFence(VkDevice device, bool signaled)
{
	VkFenceCreateInfo createInfo = {
	    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
	    .flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u,
	};

	VkFence fence;
	VK_CHECK(vkCreateFence(device, &createInfo, nullptr, &fence));

	return fence;
}