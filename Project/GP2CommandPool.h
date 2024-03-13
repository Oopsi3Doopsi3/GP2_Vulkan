#pragma once
#include <vulkan/vulkan_core.h>

struct QueueFamilyIndices;

class GP2CommandPool final
{
public:
	GP2CommandPool() = default;

	~GP2CommandPool() = default;
	GP2CommandPool(const GP2CommandPool&) = delete;
	GP2CommandPool& operator=(const GP2CommandPool&) = delete;
	GP2CommandPool(const GP2CommandPool&&) = delete;
	GP2CommandPool& operator=(const GP2CommandPool&&) = delete;

	void CreateCommandPool(const QueueFamilyIndices& queueFamilyIndices,const VkDevice& device);
	VkCommandBuffer CreateCommandBuffer(const VkDevice& device);
	void DestroyCommandPool(const VkDevice& device);

private:
	VkCommandPool m_CommandPool;
};
