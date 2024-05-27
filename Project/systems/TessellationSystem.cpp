#include "TessellationSystem.h"

#include "GP2SwapChain.h"

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
	struct SimplePushConstantData
	{
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
		float tessAlpha = 1.0f;
		float tessLevel = 3.0f;
	};

	TessellationSystem::TessellationSystem(GP2Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) :
		m_GP2Device(device)
	{
		CreatePipelineLayout(globalSetLayout);
		CreatePipeline(renderPass);
	}

	TessellationSystem::~TessellationSystem()
	{
		vkDestroyPipelineLayout(m_GP2Device.Device(), m_PipelineLayout, nullptr);
	}

	void TessellationSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData); //Use push constant or ubo?

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout }; //Check descriptorSetLayoutImage, for textures?

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_GP2Device.Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void TessellationSystem::CreatePipeline(VkRenderPass renderPass)
	{
		assert(m_PipelineLayout != nullptr && "Cannot create pipleine before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		GP2Pipeline::TessellationPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;

		std::vector<ShaderConfigInfo> shaderConfiginfo{};
		shaderConfiginfo.push_back(ShaderConfigInfo{ "shaders/Tessellation.vert.spv", VK_SHADER_STAGE_VERTEX_BIT });
		shaderConfiginfo.push_back(ShaderConfigInfo{ "shaders/Tessellation.tesc.spv", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT });
		shaderConfiginfo.push_back(ShaderConfigInfo{ "shaders/Tessellation.tese.spv", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT });
		shaderConfiginfo.push_back(ShaderConfigInfo{ "shaders/Tessellation.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT });

		std::vector<GP2Model::VertexComponent> vertexComponents{};
		vertexComponents.push_back(GP2Model::VertexComponent::Position);
		vertexComponents.push_back(GP2Model::VertexComponent::Color);
		vertexComponents.push_back(GP2Model::VertexComponent::Normal);
		vertexComponents.push_back(GP2Model::VertexComponent::UV);

		m_GP2Pipeline = std::make_unique<GP2Pipeline>(
			m_GP2Device,
			shaderConfiginfo,
			vertexComponents,
			pipelineConfig
		);
	}

	void TessellationSystem::RenderGameObjects(FrameInfo& frameInfo, GP2GameObject::Map& gameObjects)
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
			SimplePushConstantData push{};
			push.modelMatrix = obj.m_Transform.mat4();
			push.normalMatrix = obj.m_Transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				m_PipelineLayout,
				VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			obj.m_Model->Bind(frameInfo.commandBuffer);
			obj.m_Model->Draw(frameInfo.commandBuffer);
		}
	}
}