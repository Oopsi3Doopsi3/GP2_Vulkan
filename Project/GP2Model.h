#pragma once

#include "GP2Device.h"
#include "GP2Buffer.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>

//std
#include <memory>
#include <vector>

namespace GP2
{
	class GP2Model
	{
	public:

		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return
					position == other.position &&
					color == other.color &&
					normal == other.normal;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void LoadModel(const std::string& filepath);
		};

		GP2Model(GP2Device& device, const GP2Model::Builder& builder);
		~GP2Model();

		GP2Model(const GP2Model&) = delete;
		GP2Model& operator=(const GP2Model&) = delete;

		static std::unique_ptr<GP2Model> CreateModelFromFile(GP2Device& device, const std::string& filepath);

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);

		GP2Device& m_GP2Device;

		std::unique_ptr<GP2Buffer> m_VertexBuffer;
		uint32_t m_VertexCount;

		bool m_HasIndexBuffer = false;
		std::unique_ptr<GP2Buffer> m_IndexBuffer;
		uint32_t m_IndexCount;
	};
}