#pragma once

#include "GP2Camera.h"
#include "GP2GameObject.h"

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
		GP2GameObject::Map& gameObjects;
	};
}