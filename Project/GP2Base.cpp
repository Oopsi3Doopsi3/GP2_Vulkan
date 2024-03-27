#include "GP2Base.h"

//std
#include <stdexcept>
#include <cassert>

namespace GP2
{
	//GP2Base::GP2Base()
	//{
	//	CreatePipelineLayout();
	//	CreatePipeline();
	//	CreateCommandBuffers();
	//}
	//
	//GP2Base::~GP2Base()
	//{
	//	vkDestroyPipelineLayout(m_GP2Device.Device(), m_PipelineLayout, nullptr);
	//}
	
	void GP2Base::Run()
	{
		while (!m_GP2Window.ShouldClose()){
			glfwPollEvents();
		}
	}
	
	//void GP2Base::CreatePipelineLayout()
	//{
	//	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	//	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	//	pipelineLayoutInfo.setLayoutCount = 0;
	//	pipelineLayoutInfo.pSetLayouts = nullptr;
	//	pipelineLayoutInfo.pushConstantRangeCount = 0;
	//	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	//	if (vkCreatePipelineLayout(m_GP2Device.Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
	//		throw std::runtime_error("failed to create pipeline layout");
	//	}
	//}
	//
	//void GP2Base::CreatePipeline()
	//{
	//	auto pipelineConfig = GP2Pipeline::DefaultPipelineConfigInfo(m_GP2SwapChain.Width(), m_GP2SwapChain.Height());
	//	pipelineConfig.renderPass = m_GP2SwapChain.GetRenderPass();
	//	pipelineConfig.pipelineLayout = m_PipelineLayout;
	//
	//	m_GP2Pipeline = std::make_unique<GP2Pipeline>(
	//		m_GP2Device,
	//		"shaders/GP2SimpleShader.vert.spv",
	//		"shaders/GP2SimpleShader.frag.spv",
	//		pipelineConfig
	//	);
	//}
	//
	//void GP2Base::CreateCommandBuffers()
	//{
	//
	//}
	//
	//void GP2Base::DrawFrame()
	//{
	//
	//}
}