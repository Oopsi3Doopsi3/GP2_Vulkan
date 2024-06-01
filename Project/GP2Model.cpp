#include "GP2Model.h"

#include "GP2Utils.h"

#include "GP2Descriptors.h"
#include "GP2SwapChain.h"

//libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

//std
#include <cassert>
#include <unordered_map>

#include <iostream>

namespace std
{
	template<>
	struct hash<GP2::GP2Model::Vertex>
	{
		size_t operator()(GP2::GP2Model::Vertex const& vertex) const
		{
			size_t seed = 0;
			GP2::HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv, vertex.tangent);
			return seed;
		}
	};
}

namespace GP2
{
	std::unique_ptr<GP2DescriptorSetLayout> GP2::GP2Model::textureSetLayout = nullptr;

	GP2Model::GP2Model(GP2Device& device, const GP2Model::Builder& builder):
		m_GP2Device(device)
	{
		CreateVertexBuffers(builder.vertices);
		CreateIndexBuffers(builder.indices);
		InitDescriptorSet(VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE);
	}

	GP2Model::~GP2Model()
	{
	}

	void GP2Model::InitTextureSetLayout(GP2Device& device)
	{
		textureSetLayout = GP2DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();
	}

	void GP2Model::CreateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_VertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_VertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		GP2Buffer stagingBuffer{
			m_GP2Device,
			vertexSize,
			m_VertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)vertices.data());

		m_VertexBuffer = std::make_unique<GP2Buffer>(
			m_GP2Device,
			vertexSize,
			m_VertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_GP2Device.CopyBuffer(stagingBuffer.GetBuffer(), m_VertexBuffer->GetBuffer(), bufferSize);
	}

	void GP2Model::CreateIndexBuffers(const std::vector<uint32_t>& indices)
	{
		m_IndexCount = static_cast<uint32_t>(indices.size());
		m_HasIndexBuffer = m_IndexCount > 0;

		if (!m_HasIndexBuffer) {
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;
		uint32_t indexSize = sizeof(indices[0]);

		GP2Buffer stagingBuffer{
			m_GP2Device,
			indexSize,
			m_IndexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)indices.data());

		m_IndexBuffer = std::make_unique<GP2Buffer>(
			m_GP2Device,
			indexSize,
			m_IndexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_GP2Device.CopyBuffer(stagingBuffer.GetBuffer(), m_IndexBuffer->GetBuffer(), bufferSize);
	}

	void GP2Model::InitDescriptorSet(const GP2Texture* diffuseMap, const GP2Texture* normalMap, const GP2Texture* glossinessMap, const GP2Texture* specularMap)
	{
		m_Pool = GP2DescriptorPool::Builder(m_GP2Device)
			.SetMaxSets(GP2SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, GP2SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, GP2SwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();

		m_DescriptorSets.resize(GP2SwapChain::MAX_FRAMES_IN_FLIGHT);

		VkDescriptorImageInfo defaultImageInfo{};
		defaultImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		defaultImageInfo.imageView = VK_NULL_HANDLE;
		defaultImageInfo.sampler = VK_NULL_HANDLE;

		GP2Texture diffuse = GP2Texture(m_GP2Device, "textures/vehicle_diffuse.png");
		GP2Texture normal = GP2Texture(m_GP2Device, "textures/vehicle_normal.png");
		GP2Texture glossiness = GP2Texture(m_GP2Device, "textures/vehicle_gloss.png");
		GP2Texture specular = GP2Texture(m_GP2Device, "textures/vehicle_specular.png");

		for (int i{}; i < m_DescriptorSets.size(); ++i)
		{
			GP2DescriptorWriter(*textureSetLayout, *m_Pool)
				.WriteImage(0, &diffuse.DescriptorImageInfo())
				.WriteImage(1, &normal.DescriptorImageInfo() )
				.WriteImage(2, &glossiness.DescriptorImageInfo())
				.WriteImage(3, &specular.DescriptorImageInfo())
				.Build(m_DescriptorSets[i]);
		}

		//for (int i{}; i < m_DescriptorSets.size(); ++i)
		//{
		//	GP2DescriptorWriter(*textureSetLayout, *globalPool)
		//		.WriteImage(0, diffuseMap ? &diffuseMap->DescriptorImageInfo() : &defaultImageInfo)
		//		.WriteImage(1, normalMap  ? &normalMap->DescriptorImageInfo() : &defaultImageInfo)
		//		.WriteImage(2, glossinessMap ? &glossinessMap->DescriptorImageInfo() : &defaultImageInfo)
		//		.WriteImage(3, specularMap ? &specularMap->DescriptorImageInfo() : &defaultImageInfo)
		//		.Build(m_DescriptorSets[i]);
		//}
	}

	std::unique_ptr<GP2Model> GP2Model::CreateModelFromFile(GP2Device& device, const std::string& filepath)
	{
		Builder builder{};
		builder.LoadModel(filepath);

		return std::make_unique<GP2Model>(device, builder);
	}

	void GP2Model::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { m_VertexBuffer->GetBuffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (m_HasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void GP2Model::Draw(VkCommandBuffer commandBuffer)
	{
		if (m_HasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
		}
	}

	std::vector<VkVertexInputBindingDescription> GP2Model::Vertex::GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	
		return bindingDescriptions;
	}

	VkVertexInputBindingDescription GP2Model::Vertex::InputBindingDescription(uint32_t binding)
	{
		return VkVertexInputBindingDescription({ binding, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX });
	}

	std::vector<VkVertexInputAttributeDescription> GP2Model::Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
	
		attributeDescriptions.push_back({ 0,0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescriptions.push_back({ 1,0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		attributeDescriptions.push_back({ 2,0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3,0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });
		attributeDescriptions.push_back({ 4,0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangent) });
	
		return attributeDescriptions;
	}

	VkVertexInputAttributeDescription GP2Model::Vertex::InputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component)
	{
		switch (component)
		{
		case VertexComponent::Position:
			return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		case VertexComponent::Color:
			return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		case VertexComponent::Normal:
			return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		case VertexComponent::UV:
			return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });
		case VertexComponent::Tangent:
			return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangent) });
		default:
			return VkVertexInputAttributeDescription({});
		}
	}

	std::vector<VkVertexInputAttributeDescription> GP2Model::Vertex::InputAttributeDescriptions(uint32_t binding, const std::vector<VertexComponent> components)
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		uint32_t location = 0;

		for (auto component : components)
		{
			attributeDescriptions.push_back(InputAttributeDescription(binding, location, component));
			++location;
		}
		
		return attributeDescriptions;
	}

	VkPipelineVertexInputStateCreateInfo GP2Model::Vertex::GetVertexInputState(const std::vector<VertexComponent> components)
	{
		VkVertexInputBindingDescription inputBindingDescription = InputBindingDescription(0);
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions = InputAttributeDescriptions(0, components);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &inputBindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

		return vertexInputInfo;
	}

	void GP2Model::Builder::LoadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				if (index.vertex_index >= 0)
				{
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};
				}

				if (index.normal_index >= 0)
				{
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}

		// Calculate tangents
		std::vector<glm::vec3> tan1(vertices.size(), glm::vec3(0.0f));
		std::vector<glm::vec3> tan2(vertices.size(), glm::vec3(0.0f));

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			Vertex& v0 = vertices[indices[i]];
			Vertex& v1 = vertices[indices[i + 1]];
			Vertex& v2 = vertices[indices[i + 2]];

			glm::vec3 edge1 = v1.position - v0.position;
			glm::vec3 edge2 = v2.position - v0.position;

			glm::vec2 deltaUV1 = v1.uv - v0.uv;
			glm::vec2 deltaUV2 = v2.uv - v0.uv;

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			glm::vec3 tangent;
			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

			tan1[indices[i]] += tangent;
			tan1[indices[i + 1]] += tangent;
			tan1[indices[i + 2]] += tangent;
		}

		for (size_t i = 0; i < vertices.size(); ++i)
		{
			vertices[i].tangent = glm::normalize(tan1[i]);
		}
	}
}