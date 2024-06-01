#pragma once

#include "GP2Device.h"
#include "GP2Buffer.h"

#include "GP2Texture.h"
#include "GP2Descriptors.h"

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

		enum class VertexComponent
		{
			Position,
			Color,
			Normal,
			UV,
			Tangent
		};

		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};
			glm::vec3 tangent{};

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static VkVertexInputBindingDescription InputBindingDescription(uint32_t binding);
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
			static VkVertexInputAttributeDescription InputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component);
			static std::vector<VkVertexInputAttributeDescription> InputAttributeDescriptions(uint32_t binding, const std::vector<VertexComponent> components);
			static VkPipelineVertexInputStateCreateInfo GetVertexInputState(const std::vector<VertexComponent> components);

			bool operator==(const Vertex& other) const
			{
				return
					position == other.position &&
					color == other.color &&
					normal == other.normal;
					uv == other.uv;
					tangent == other.tangent;
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
		void InitDescriptorSet(const GP2Texture* diffuseMap, const GP2Texture* normalMap, const GP2Texture* glossinessMap, const GP2Texture* specularMap);

		std::vector<VkDescriptorSet> GetDescriptorSets() const { return m_DescriptorSets; }

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

		
		static void InitTextureSetLayout(GP2Device& device);
		static void DestroyTextureSetLayout() { textureSetLayout.reset(); }
		static GP2DescriptorSetLayout* GetTextureSetLayout() { return textureSetLayout.get(); }

	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);

		GP2Device& m_GP2Device;

		std::unique_ptr<GP2Buffer> m_VertexBuffer;
		uint32_t m_VertexCount;

		bool m_HasIndexBuffer = false;
		std::unique_ptr<GP2Buffer> m_IndexBuffer;
		uint32_t m_IndexCount;


		std::vector<VkDescriptorSet> m_DescriptorSets;
		std::unique_ptr<GP2DescriptorPool> m_Pool{};

		static std::unique_ptr<GP2DescriptorSetLayout> textureSetLayout;
	};
}