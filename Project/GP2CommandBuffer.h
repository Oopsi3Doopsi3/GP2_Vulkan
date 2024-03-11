#pragma once
#include <vulkan/vulkan_core.h>

class GP2CommandBuffer final
{
public:
	GP2CommandBuffer() = default;

	~GP2CommandBuffer() = default;
	GP2CommandBuffer(const GP2CommandBuffer&) = delete;
	GP2CommandBuffer& operator=(const GP2CommandBuffer&) = delete;
	GP2CommandBuffer(const GP2CommandBuffer&&) = delete;
	GP2CommandBuffer& operator=(const GP2CommandBuffer&&) = delete;

	void BeginCommandBuffer();
	void EndCommandBuffer();
	const VkCommandBuffer& GetCommandBuffer() const { return m_CommandBuffer; }
	void SetCommandBuffer(VkCommandBuffer commandBuffer) { m_CommandBuffer = commandBuffer; }

private:
	VkCommandBuffer m_CommandBuffer;
};
