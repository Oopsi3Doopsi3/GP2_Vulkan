#pragma once
#include <vulkan/vulkan_core.h>

class GP2DataBuffer
{
public:
	GP2DataBuffer(
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkDeviceSize size
	);

	~GP2DataBuffer()
	{
		Destroy();
	}

	void Upload(VkDeviceSize size, void* data);
	void Map(VkDeviceSize size, void* data);
	void Destroy();
	void BindAsVertexBuffer(VkCommandBuffer commandBuffer);
	void BindAsIndexBuffer(VkCommandBuffer commandBuffer);
	VkBuffer GetVkBuffer() const;
	VkDeviceSize GetSizeInBytes() const;

private:
	uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer destBuffer, VkDeviceSize size);

	VkPhysicalDevice m_VkPhysicalDevice;
	VkDevice m_VkDevice;
	VkDeviceSize m_Size;
	VkBuffer m_VkBuffer;
	VkDeviceMemory m_VkBufferMemory;
};
