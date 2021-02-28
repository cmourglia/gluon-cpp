#pragma once

#include "vkcommon.h"

#include <optional>

struct QueueFamilyIndices
{
	std::optional<u32> graphicsFamily;
	std::optional<u32> presentFamily;
	// TODO
	// std::optional<u32> computeFamily;

	inline bool IsComplete() const
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice gpu, VkSurfaceKHR surface);