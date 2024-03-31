#pragma once

#include "GP2Camera.h"

//lib
#include <vulkan/vulkan.h>

namespace GP2
{
	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		GP2Camera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}