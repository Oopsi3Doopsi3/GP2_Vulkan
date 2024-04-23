#pragma once

#include "GP2Device.h"

//std
#include <string>
#include <vector>

namespace GP2
{
	struct PipelineConfigInfo 
	{
		PipelineConfigInfo() = default;

		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineTessellationStateCreateInfo tessellationInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	struct ShaderConfigInfo
	{
		const std::string& filePath;
		VkShaderStageFlagBits shaderStage;
	};

	class GP2Pipeline
	{
	public:
		GP2Pipeline(
			GP2Device& device,
			std::vector<ShaderConfigInfo> shaderConfigInfo,
			const PipelineConfigInfo& configInfo
		);

		~GP2Pipeline();

		GP2Pipeline(const GP2Pipeline&) = delete;
		GP2Pipeline& operator=(const GP2Pipeline&) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
		static void TessellationPipelineConfigInfo(PipelineConfigInfo& configInfo);
		static void EnableAlphaBlending(PipelineConfigInfo& configInfo);

	private:
		void CreateGraphicsPipeline(
			std::vector<ShaderConfigInfo> shaderConfigInfo,
			const PipelineConfigInfo& configInfo
		);

		VkShaderModule CreateShaderModule(const std::string& filePath);

		GP2Device& m_GP2Device;
		VkPipeline m_GraphicsPipeline;
		std::vector<VkShaderModule> m_ShaderModules;
	};
}