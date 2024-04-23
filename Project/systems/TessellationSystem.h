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
	class TessellationSystem
	{
	public:
		TessellationSystem(GP2Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~TessellationSystem();

		TessellationSystem(const TessellationSystem&) = delete;
		TessellationSystem& operator=(const TessellationSystem&) = delete;

		void RenderGameObjects(FrameInfo& frameInfo, GP2GameObject::Map& gameObjects);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);


		GP2Device& m_GP2Device;
		std::unique_ptr<GP2Pipeline> m_GP2Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}