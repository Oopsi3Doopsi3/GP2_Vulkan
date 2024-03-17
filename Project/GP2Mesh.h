#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

#include "Vertex.h"

class GP2Mesh final
{
public:
	void Initialize(VkPhysicalDevice physicalDevice, VkDevice device);

	void DestroyMesh(VkDevice device);
	void Draw(VkCommandBuffer buffer);
	void AddVertex(glm::vec2 pos, glm::vec3 color);
	void AddVertex(const Vertex& vertex);

private:
	VkBuffer m_VkBuffer;
	VkDeviceMemory m_VkBufferMemory;
	std::vector<Vertex> m_Vertices;

	uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
};
