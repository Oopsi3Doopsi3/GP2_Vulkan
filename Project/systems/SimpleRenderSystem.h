#pragma once

#include "GP2Pipeline.h"
#include "GP2Device.h"
#include "GP2GameObject.h"
#include "GP2FrameInfo.h"

//std
#include <memory>
#include <vector>

namespace GP2
{
	enum class RenderMode
	{
		observedArea,
		diffuseColor,
		specular,
		combined
	};

	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(GP2Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void Update(FrameInfo& frameInfo, GP2GameObject::Map& gameObjects);
		void RenderGameObjects(FrameInfo& frameInfo, GP2GameObject::Map& gameObjects);

		void CycleRenderMode();
		void ToggleRotation();
		void ToggleNormalMap();

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);
		

		GP2Device& m_GP2Device;
		std::unique_ptr<GP2Pipeline> m_GP2Pipeline;
		VkPipelineLayout m_PipelineLayout;

		RenderMode m_RenderMode = RenderMode::combined;
		bool m_ShouldRotate{ false };
		bool m_UseNormalMap{ true };
	};
}

