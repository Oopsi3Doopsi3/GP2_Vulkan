#pragma once
#include <vulkan/vulkan_core.h>

#include "GP2RenderPass.h"
#include "GP2Shader.h"

class GP2GraphicsPipeline
{
public:
	GP2GraphicsPipeline() = default;

	~GP2GraphicsPipeline() = default;
	GP2GraphicsPipeline(const GP2GraphicsPipeline&) = delete;
	GP2GraphicsPipeline& operator=(const GP2GraphicsPipeline&) = delete;
	GP2GraphicsPipeline(const GP2GraphicsPipeline&&) = delete;
	GP2GraphicsPipeline& operator=(const GP2GraphicsPipeline&&) = delete;

	void CreateGraphicsPipeline(const VkDevice& device, GP2Shader& shader, const GP2RenderPass& renderPass);
	void Destroy(const VkDevice& device);
	VkPipeline GetPipeline() const { return m_GraphicsPipeline; }

private:
	VkPipeline m_GraphicsPipeline{};
	VkPipelineLayout m_PipelineLayout{};
};
