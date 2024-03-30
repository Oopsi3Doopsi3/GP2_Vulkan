#pragma once

#include "GP2Window.h"
#include "GP2Device.h"
#include "GP2SwapChain.h"
#include "GP2Model.h"

//std
#include <memory>
#include <vector>
#include <cassert>

namespace GP2
{
	class GP2Renderer
	{
	public:
		GP2Renderer(GP2Window& window, GP2Device& device);
		~GP2Renderer();

		GP2Renderer(const GP2Renderer&) = delete;
		GP2Renderer& operator=(const GP2Renderer&) = delete;

		VkRenderPass GetSwapChainRenderPass() const { return m_GP2SwapChain->GetRenderPass(); }
		float GetAspectRatio() const { return m_GP2SwapChain->ExtentAspectRatio(); }
		bool IsFrameInProgress() const { return m_IsFrameStarted; }
		VkCommandBuffer GetCurrentCommandBuffer() const 
		{ 
			assert(m_IsFrameStarted && "Cannot get command buffer when frame not in progress");
			return m_CommandBuffers[m_CurrentFrameindex];
		}

		int GetFrameIndex() const
		{
			assert(m_IsFrameStarted && "Cannot get frame index when frame not in progress");
			return m_CurrentFrameindex;
		}

		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();

		GP2Window& m_GP2Window;
		GP2Device& m_GP2Device;
		std::unique_ptr<GP2SwapChain> m_GP2SwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		uint32_t m_CurrentImageIndex;
		int m_CurrentFrameindex{ 0 };
		bool m_IsFrameStarted = false;
	};
}

