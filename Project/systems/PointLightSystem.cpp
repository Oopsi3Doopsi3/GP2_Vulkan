#include "PointLightSystem.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <cassert>
#include <array>

namespace GP2
{
	PointLightSystem::PointLightSystem(GP2Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout):
		m_GP2Device(device)
	{
		CreatePipelineLayout(globalSetLayout);
		CreatePipeline(renderPass);
	}

	PointLightSystem::~PointLightSystem()
	{
		vkDestroyPipelineLayout(m_GP2Device.Device(), m_PipelineLayout, nullptr);
	}

	void PointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		//VkPushConstantRange pushConstantRange{};
		//pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		//pushConstantRange.offset = 0;
		//pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(m_GP2Device.Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void PointLightSystem::CreatePipeline(VkRenderPass renderPass)
	{
		assert(m_PipelineLayout != nullptr && "Cannot create pipleine before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		GP2Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;

		m_GP2Pipeline = std::make_unique<GP2Pipeline>(
			m_GP2Device,
			"shaders/GP2PointLight.vert.spv",
			"shaders/GP2PointLight.frag.spv",
			pipelineConfig
		);
	}

	void PointLightSystem::Render(FrameInfo& frameInfo)
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

		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}
}