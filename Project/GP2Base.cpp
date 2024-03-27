#include "GP2Base.h"

//std
#include <stdexcept>
#include <cassert>
#include <array>

namespace GP2
{
	GP2Base::GP2Base()
	{
		LoadModels();
		CreatePipelineLayout();
		CreatePipeline();
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
	
	void GP2Base::LoadModels()
	{
		std::vector<GP2Model::Vertex> vertices{
			{{0.f,-.5f}, {1.f, 0.f, 0.f}},
			{{.5f,.5f}, {0.f, 1.f, 0.f}},
			{{-.5f,.5f}, {0.f, 0.f, 1.f}}
		};

		m_GP2Model = std::make_unique<GP2Model>(m_GP2Device, vertices);
	}

	void GP2Base::CreatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(m_GP2Device.Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout");
		}
	}
	
	void GP2Base::CreatePipeline()
	{
		PipelineConfigInfo pipelineConfig{};
		GP2Pipeline::DefaultPipelineConfigInfo(pipelineConfig, m_GP2SwapChain.Width(), m_GP2SwapChain.Height());
		pipelineConfig.renderPass = m_GP2SwapChain.GetRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout;
	
		m_GP2Pipeline = std::make_unique<GP2Pipeline>(
			m_GP2Device,
			"shaders/GP2SimpleShader.vert.spv",
			"shaders/GP2SimpleShader.frag.spv",
			pipelineConfig
		);
	}
	
	void GP2Base::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(m_GP2SwapChain.ImageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_GP2Device.GetCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_GP2Device.Device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}

		for (int i{}; i < m_CommandBuffers.size(); ++i)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("Failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_GP2SwapChain.GetRenderPass();
			renderPassInfo.framebuffer = m_GP2SwapChain.GetFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0,0 };
			renderPassInfo.renderArea.extent = m_GP2SwapChain.GetSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { .1f,.1f,.1f,1.f };
			clearValues[1].depthStencil = { 1.f, 0 };

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			m_GP2Pipeline->Bind(m_CommandBuffers[i]);
			m_GP2Model->Bind(m_CommandBuffers[i]);
			m_GP2Model->Draw(m_CommandBuffers[i]);

			vkCmdEndRenderPass(m_CommandBuffers[i]);
			if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to record command buffer");
			}
		}
	}
	
	void GP2Base::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = m_GP2SwapChain.AcquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		result = m_GP2SwapChain.SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image");
		}
	}
}