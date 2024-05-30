#pragma once

#include "GP2Camera.h"
#include "GP2GameObject.h"

//lib
#include <vulkan/vulkan.h>

namespace GP2
{
	#define MAX_LIGHTS 10

	struct PointLight
	{
		glm::vec4 position{};
		glm::vec4 color{};
	};

	struct GlobalUbo
	{
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inversView{ 1.f };
		glm::vec4 ambientLightColor{ .01f, .01f, .01f, 1.f };
		glm::vec4 lightDirection = glm::normalize(glm::vec4{ .577f, .577f, .577f, 1.f });
		PointLight pointlights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		GP2Camera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}