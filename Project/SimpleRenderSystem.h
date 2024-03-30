#pragma once

#include "GP2Pipeline.h"
#include "GP2Device.h"
#include "GP2GameObject.h"
#include "GP2Camera.h"

//std
#include <memory>
#include <vector>

namespace GP2
{
	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(GP2Device& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GP2GameObject>& gameobjects, const GP2Camera& camera);

	private:
		void CreatePipelineLayout();
		void CreatePipeline(VkRenderPass renderPass);
		

		GP2Device& m_GP2Device;
		std::unique_ptr<GP2Pipeline> m_GP2Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}

