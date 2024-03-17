#include "GP2Mesh.h"

#include <stdexcept>

void GP2Mesh::Initialize(VkPhysicalDevice physicalDevice, VkDevice device)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(m_Vertices[0]) * m_Vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_VkBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, m_VkBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &m_VkBufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(device, m_VkBuffer, m_VkBufferMemory, 0);

	void* data;
	vkMapMemory(device, m_VkBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, m_Vertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(device, m_VkBufferMemory);
}
	 
void GP2Mesh::DestroyMesh(VkDevice device)
{
	vkDestroyBuffer(device, m_VkBuffer, nullptr);
	vkFreeMemory(device, m_VkBufferMemory, nullptr);
}

void GP2Mesh::Draw(VkCommandBuffer commandBuffer)
{
	VkBuffer vertexBuffers[] = { m_VkBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
}

void GP2Mesh::AddVertex(glm::vec2 pos, glm::vec3 color)
{
	m_Vertices.push_back(Vertex{ pos, color });
}

void GP2Mesh::AddVertex(const Vertex& vertex)
{
	m_Vertices.push_back(vertex);
}

uint32_t GP2Mesh::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
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