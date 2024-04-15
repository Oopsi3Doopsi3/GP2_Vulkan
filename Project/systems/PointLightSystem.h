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
	class PointLightSystem
	{
	public:
		PointLightSystem(GP2Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void Update(FrameInfo& frameInfo, GlobalUbo& ubo, GP2GameObject::Map& gameObjects);
		void Render(FrameInfo& frameInfo, GP2GameObject::Map& gameObjects);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);
		

		GP2Device& m_GP2Device;
		std::unique_ptr<GP2Pipeline> m_GP2Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}

