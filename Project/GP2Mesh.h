#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

#include "Vertex.h"

//class GP2DataBuffer;

class GP2Mesh final
{
public:
	void Initialize(VkPhysicalDevice physicalDevice, VkDevice device);

	void DestroyMesh(VkDevice device);
	void Draw(VkCommandBuffer commandBuffer);
	void AddVertex(glm::vec2 pos, glm::vec3 color);
	void AddVertex(const Vertex& vertex);

private:
	std::vector<Vertex> m_Vertices;
	std::vector<uint16_t> m_Indices;
	//std::unique_ptr<GP2DataBuffer> m_VertexBuffer;
	//std::unique_ptr<GP2DataBuffer> m_IndexBuffer;

	uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
};
