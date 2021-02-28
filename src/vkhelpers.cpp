#include "vkhelpers.h"

#include <vector>

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice gpu, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;

	u32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueFamilies.data());

	for (u32 i = 0; i < queueFamilyCount; ++i)
	{
		if (!indices.graphicsFamily.has_value() && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		if (!indices.presentFamily.has_value())
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &presentSupport);

			if (presentSupport)
			{
				indices.presentFamily = i;
			}
		}
	}

	return indices;
}