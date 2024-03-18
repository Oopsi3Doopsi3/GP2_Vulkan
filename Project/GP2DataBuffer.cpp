#include "GP2DataBuffer.h"

#include <stdexcept>

GP2DataBuffer::GP2DataBuffer(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkDeviceSize size
):
	m_VkPhysicalDevice(physicalDevice),
	m_VkDevice(device),
	m_Size(size)
{
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, m_Size, 0, &data);
	memcpy(data, m_Vertices.data(), (size_t)m_Size); //How to pass vertices data?
	vkUnmapMemory(device, stagingBufferMemory);

	CreateBuffer(size, usage, properties, m_VkBuffer, m_VkBufferMemory);

	CopyBuffer(stagingBuffer, m_VkBuffer, size);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void GP2DataBuffer::Upload(VkDeviceSize size, void* data)
{
	
}

void GP2DataBuffer::Map(VkDeviceSize size, void* data)
{
	
}

void GP2DataBuffer::Destroy()
{
	vkDestroyBuffer(m_VkDevice, m_VkBuffer, nullptr);
	vkFreeMemory(m_VkDevice, m_VkBufferMemory, nullptr);
}

void GP2DataBuffer::BindAsVertexBuffer(VkCommandBuffer commandBuffer)
{
	VkBuffer vertexBuffers[] = { m_VkBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
}

void GP2DataBuffer::BindAsIndexBuffer(VkCommandBuffer commandBuffer)
{
	vkCmdBindIndexBuffer(commandBuffer, m_VkBuffer, 0, VK_INDEX_TYPE_UINT16);
}

VkBuffer GP2DataBuffer::GetVkBuffer() const
{
	return m_VkBuffer;
}

VkDeviceSize GP2DataBuffer::GetSizeInBytes() const
{
	return  m_Size;
}

uint32_t GP2DataBuffer::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void GP2DataBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_VkDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_VkDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(m_VkPhysicalDevice, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_VkDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(m_VkDevice, buffer, bufferMemory, 0);
}

void GP2DataBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer destBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_CommandPool.GetCommandPool(); //How to get commandpool?
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_VkDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, destBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE); //How to get graphicsqueue?
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(m_VkDevice, m_CommandPool.GetCommandPool(), 1, &commandBuffer);
}