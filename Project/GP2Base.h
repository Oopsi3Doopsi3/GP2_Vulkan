#pragma once

#include "GP2Window.h"
#include "GP2Pipeline.h"
#include "GP2Device.h"
#include "GP2SwapChain.h"
#include "GP2Model.h"

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
		void operator=(const GP2Base&) = delete;
		
		void Run();

	private:
		void LoadModels();
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void DrawFrame();
		
		GP2Window m_GP2Window{WIDTH, HEIGHT, "I Vulkan't Anymore"};
		GP2Device m_GP2Device{ m_GP2Window };
		GP2SwapChain m_GP2SwapChain{ m_GP2Device, m_GP2Window.GetExtent() };
		std::unique_ptr<GP2Pipeline> m_GP2Pipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::unique_ptr<GP2Model> m_GP2Model;
	};
}

