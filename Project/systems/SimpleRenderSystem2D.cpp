#include "SimpleRenderSystem2D.h"

//std
#include <stdexcept>
#include <cassert>

namespace GP2
{
	SimpleRenderSystem2D::SimpleRenderSystem2D(GP2Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) :
		m_GP2Device(device)
	{
		CreatePipelineLayout(globalSetLayout);
		CreatePipeline(renderPass);
	}

	SimpleRenderSystem2D::~SimpleRenderSystem2D()
	{
		vkDestroyPipelineLayout(m_GP2Device.Device(), m_PipelineLayout, nullptr);
	}

	void SimpleRenderSystem2D::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		if (vkCreatePipelineLayout(m_GP2Device.Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void SimpleRenderSystem2D::CreatePipeline(VkRenderPass renderPass)
	{
		assert(m_PipelineLayout != nullptr && "Cannot create pipleine before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		GP2Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;

		m_GP2Pipeline = std::make_unique<GP2Pipeline>(
			m_GP2Device,
			"shaders/GP2Shader2D.vert.spv",
			"shaders/GP2Shader2D.frag.spv",
			pipelineConfig
		);
	}

	void SimpleRenderSystem2D::RenderGameObjects(FrameInfo& frameInfo, GP2GameObject::Map& gameObjects)
	{
		m_GP2Pipeline->Bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_PipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr);

		for (auto& kv : gameObjects)
		{
			auto& obj = kv.second;
			if (obj.m_Model == nullptr) continue;

			obj.m_Model->Bind(frameInfo.commandBuffer);
			obj.m_Model->Draw(frameInfo.commandBuffer);
		}
	}
}