#include "GP2Renderer.h"

//std
#include <stdexcept>
#include <cassert>
#include <array>

namespace GP2
{
	GP2Renderer::GP2Renderer(GP2Window& window, GP2Device& device):
		m_GP2Window(window),
		m_GP2Device(device)
	{
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	GP2Renderer::~GP2Renderer()
	{
		FreeCommandBuffers();
	}

	void GP2Renderer::RecreateSwapChain()
	{
		auto extent = m_GP2Window.GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_GP2Window.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_GP2Device.Device());

		if (m_GP2SwapChain == nullptr) {
			m_GP2SwapChain = std::make_unique<GP2SwapChain>(m_GP2Device, extent);
		}
		else {
			std::shared_ptr<GP2SwapChain> oldSwapChain = std::move(m_GP2SwapChain);
			m_GP2SwapChain = std::make_unique<GP2SwapChain>(m_GP2Device, extent, oldSwapChain);

			if (!oldSwapChain->CompareSwapFormats(*m_GP2SwapChain.get())) {
				throw std::runtime_error("Swap chain image or depth format has changed");
			}
		}

		//brb
	}

	void GP2Renderer::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(GP2SwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_GP2Device.GetCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_GP2Device.Device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void GP2Renderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_GP2Device.Device(), m_GP2Device.GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	VkCommandBuffer GP2Renderer::BeginFrame()
	{
		assert(!m_IsFrameStarted && "Can't call BeginFrame while already in progress");

		auto result = m_GP2SwapChain->AcquireNextImage(&m_CurrentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		m_IsFrameStarted = true;

		auto commandBuffer = GetCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer");
		}
		
		return commandBuffer;
	}

	void GP2Renderer::EndFrame()
	{
		assert(m_IsFrameStarted && "Can't call EndFrame if frame is not in progress");

		auto commandBuffer = GetCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}

		auto result = m_GP2SwapChain->SubmitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_GP2Window.WasWindowResized())
		{
			m_GP2Window.ResetWindowResizedFlag();
			RecreateSwapChain();
		}
		 else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image");
		}

		m_IsFrameStarted = false;
		m_CurrentFrameindex = (m_CurrentFrameindex + 1) % GP2SwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void GP2Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(m_IsFrameStarted && "Can't call BeginSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_GP2SwapChain->GetRenderPass();
		renderPassInfo.framebuffer = m_GP2SwapChain->GetFrameBuffer(m_CurrentImageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = m_GP2SwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { .53f,.81f,.92f,1.f };
		clearValues[1].depthStencil = { 1.f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.width = static_cast<float>(m_GP2SwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(m_GP2SwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		VkRect2D scissor{ {0,0}, m_GP2SwapChain->GetSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		vkCmdSetLineWidth(commandBuffer, 1.f); //Need to call vkCmdSetLineWidth bcs dynamicStateInfo in tessellation
	}

	void GP2Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(m_IsFrameStarted && "Can't call EndSwapChainRenderPass if frame is not progress");
		assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}
}