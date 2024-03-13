#pragma once
#include <vulkan/vulkan_core.h>

class GP2RenderPass final
{
public:
	GP2RenderPass() = default;

	~GP2RenderPass() = default;
	GP2RenderPass(const GP2RenderPass&) = delete;
	GP2RenderPass& operator=(const GP2RenderPass&) = delete;
	GP2RenderPass(const GP2RenderPass&&) = delete;
	GP2RenderPass& operator=(const GP2RenderPass&&) = delete;

	void CreateRenderPass(const VkFormat& swapChainImageFormat, const VkDevice& device);
	void Destroy(const VkDevice& device);
	VkRenderPass GetRenderPass() const { return m_RenderPass; }

private:
	VkRenderPass m_RenderPass{};
};
