#include "GP2Mesh.h"

#include <stdexcept>

#include "GP2DataBuffer.h"

void GP2Mesh::Initialize(VkPhysicalDevice physicalDevice, VkDevice device)
{
	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();
	//Making vertex and index buffer, but not sure how to pass the corresponding vector data
	//m_VertexBuffer = std::make_unique<GP2DataBuffer>(physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
	//	VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);
	//
	//m_IndexBuffer = std::make_unique<GP2DataBuffer>(physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
	//	VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);
}
	 
void GP2Mesh::DestroyMesh(VkDevice device)
{
	//m_IndexBuffer->Destroy();
	//m_VertexBuffer->Destroy();
}

void GP2Mesh::Draw(VkCommandBuffer commandBuffer)
{
	//m_VertexBuffer->BindAsVertexBuffer(commandBuffer);
	//m_IndexBuffer->BindAsIndexBuffer(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
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