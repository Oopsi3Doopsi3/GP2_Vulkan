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
	class SimpleRenderSystem2D
	{
	public:
		SimpleRenderSystem2D(GP2Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem2D();

		SimpleRenderSystem2D(const SimpleRenderSystem2D&) = delete;
		SimpleRenderSystem2D& operator=(const SimpleRenderSystem2D&) = delete;

		void RenderGameObjects(FrameInfo& frameInfo, GP2GameObject::Map& gameObjects);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);


		GP2Device& m_GP2Device;
		std::unique_ptr<GP2Pipeline> m_GP2Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}
