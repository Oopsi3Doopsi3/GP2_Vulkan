#pragma once

#include "GP2Window.h"
#include "GP2Pipeline.h"
#include "GP2Device.h"
#include "GP2SwapChain.h"
#include "GP2Model.h"
#include "GP2GameObject.h"

//std
#include <memory>
#include <vector>

namespace GP2 
{
	class GP2Base
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		
		GP2Base();
		~GP2Base();
		
		GP2Base(const GP2Base&) = delete;
		GP2Base& operator=(const GP2Base&) = delete;
		
		void Run();

	private:
		void LoadGameObjects();
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void DrawFrame();
		void RecreateSwapChain();
		void RecordCommandBuffer(int imageIndex);
		void RenderGameObject(VkCommandBuffer commandBuffer);
		
		GP2Window m_GP2Window{WIDTH, HEIGHT, "I Vulkan't Anymore"};
		GP2Device m_GP2Device{ m_GP2Window };
		std::unique_ptr<GP2SwapChain> m_GP2SwapChain;
		std::unique_ptr<GP2Pipeline> m_GP2Pipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<GP2GameObject> m_GameObjects;
	};
}

