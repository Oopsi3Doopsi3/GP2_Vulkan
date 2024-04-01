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
#include <map>

namespace GP2
{
	struct PointLightPushConstants
	{
		glm::vec4 position{};
		glm::vec4 color{};
		float radius;
	};

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
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstants);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

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

	void PointLightSystem::CreatePipeline(VkRenderPass renderPass)
	{
		assert(m_PipelineLayout != nullptr && "Cannot create pipleine before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		GP2Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
		GP2Pipeline::EnableAlphaBlending(pipelineConfig);
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

	void PointLightSystem::Update(FrameInfo& frameInfo, GlobalUbo& ubo)
	{
		auto rotateLight = glm::rotate(glm::mat4(1.f), frameInfo.frameTime, { 0.f, -1.f, 0.f });

		int lightIndex = 0;
		for (auto& kv : frameInfo.gameObjects)
		{
			auto& obj = kv.second;
			if (obj.m_PointLight == nullptr) {
				continue;
			}

			assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum specified");

			//update light position
			obj.m_Transform.translation = glm::vec3(rotateLight * glm::vec4(obj.m_Transform.translation, 1.f));

			//copy lights to ubo
			ubo.pointlights[lightIndex].position = glm::vec4(obj.m_Transform.translation, 1.f);
			ubo.pointlights[lightIndex].color = glm::vec4(obj.m_Color, obj.m_PointLight->lightIntensity);

			++lightIndex;
		}
		ubo.numLights = lightIndex;
	}

	void PointLightSystem::Render(FrameInfo& frameInfo)
	{
		//sort lights
		std::map<float, GP2GameObject::id_t> sorted;
		for (auto& kv : frameInfo.gameObjects)
		{
			auto& obj = kv.second;
			if (obj.m_PointLight == nullptr) {
				continue;
			}

			//calculate distance
			auto offset = frameInfo.camera.GetPosition() - obj.m_Transform.translation;
			float disSquared = glm::dot(offset, offset);
			sorted[disSquared] = obj.GetId();
		}

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

		//iterate through sorted lights in reverse order
		for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			auto& obj = frameInfo.gameObjects.at(it->second);

			PointLightPushConstants push{};
			push.position = glm::vec4(obj.m_Transform.translation, 1.f);
			push.color = glm::vec4(obj.m_Color, obj.m_PointLight->lightIntensity);
			push.radius = obj.m_Transform.scale.x;

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				m_PipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PointLightPushConstants),
				&push
			);
			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		}
	}
}