#include "GP2Base.h"

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
		glm::mat2 transform{ 1.f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	GP2Base::GP2Base()
	{
		LoadGameObjects();
		CreatePipelineLayout();
		RecreateSwapChain();
		CreateCommandBuffers();
	}
	
	GP2Base::~GP2Base()
	{
		vkDestroyPipelineLayout(m_GP2Device.Device(), m_PipelineLayout, nullptr);
	}
	
	void GP2Base::Run()
	{
		while (!m_GP2Window.ShouldClose()){
			glfwPollEvents();
			DrawFrame();
		}

		vkDeviceWaitIdle(m_GP2Device.Device());
	}
	
	void GP2Base::LoadGameObjects()
	{
		std::vector<GP2Model::Vertex> vertices{
			{{0.f,-.5f}, {1.f, 0.f, 0.f}},
			{{.5f,.5f}, {0.f, 1.f, 0.f}},
			{{-.5f,.5f}, {0.f, 0.f, 1.f}}
		};

		auto m_GP2Model = std::make_shared<GP2Model>(m_GP2Device, vertices);

		auto triangle = GP2GameObject::CreateGameObject();
		triangle.m_Model = m_GP2Model;
		triangle.m_Color = { .1f,.8f,.1f };
		triangle.m_Transform2d.translation.x = .2f;
		triangle.m_Transform2d.scale = { 2.f, .5f };
		triangle.m_Transform2d.rotation = .25 * glm::two_pi<float>();

		m_GameObjects.push_back(std::move(triangle));
	}

	void GP2Base::CreatePipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(m_GP2Device.Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout");
		}
	}
	
	void GP2Base::CreatePipeline()
	{
		assert(m_GP2SwapChain != nullptr && "Cannot create pipleine before swap chain");
		assert(m_PipelineLayout != nullptr && "Cannot create pipleine before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		GP2Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = m_GP2SwapChain->GetRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout;
	
		m_GP2Pipeline = std::make_unique<GP2Pipeline>(
			m_GP2Device,
			"shaders/GP2SimpleShader.vert.spv",
			"shaders/GP2SimpleShader.frag.spv",
			pipelineConfig
		);
	}

	void GP2Base::RecreateSwapChain()
	{
		auto extent = m_GP2Window.GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_GP2Window.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_GP2Device.Device());

		if(m_GP2SwapChain == nullptr){
			m_GP2SwapChain = std::make_unique<GP2SwapChain>(m_GP2Device, extent);
		}
		else {
			m_GP2SwapChain = std::make_unique<GP2SwapChain>(m_GP2Device, extent, std::move(m_GP2SwapChain));
			if (m_GP2SwapChain->ImageCount() != m_CommandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}

		//If render pass compatible do nothing
		CreatePipeline();
	}
	
	void GP2Base::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(m_GP2SwapChain->ImageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_GP2Device.GetCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_GP2Device.Device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void GP2Base::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_GP2Device.Device(), m_GP2Device.GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	void GP2Base::RecordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_GP2SwapChain->GetRenderPass();
		renderPassInfo.framebuffer = m_GP2SwapChain->GetFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = m_GP2SwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { .01f,.01f,.01f,1.f };
		clearValues[1].depthStencil = { 1.f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.width = static_cast<float>(m_GP2SwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(m_GP2SwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		VkRect2D scissor{ {0,0}, m_GP2SwapChain->GetSwapChainExtent() };
		vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);
		
		RenderGameObject(m_CommandBuffers[imageIndex]);

		vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);
		if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}
	}

	void GP2Base::RenderGameObject(VkCommandBuffer commandBuffer)
	{
		m_GP2Pipeline->Bind(commandBuffer);

		for (auto& obj : m_GameObjects)
		{
			obj.m_Transform2d.rotation = glm::mod(obj.m_Transform2d.rotation + .01f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.offset = obj.m_Transform2d.translation;
			push.color = obj.m_Color;
			push.transform = obj.m_Transform2d.mat2();

			vkCmdPushConstants(
				commandBuffer,
				m_PipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			obj.m_Model->Bind(commandBuffer);
			obj.m_Model->Draw(commandBuffer);
		}
	}
	
	void GP2Base::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = m_GP2SwapChain->AcquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		RecordCommandBuffer(imageIndex);
		result = m_GP2SwapChain->SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_GP2Window.WasWindowResized())
		{
			m_GP2Window.ResetWindowResizedFlag();
			RecreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image");
		}
	}
}