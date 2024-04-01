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
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class GP2Pipeline
	{
	public:
		GP2Pipeline(
			GP2Device& device, 
			const std::string& vertFilePath,
			const std::string& fragFilePath,
			const PipelineConfigInfo& configInfo
			);

		~GP2Pipeline();

		GP2Pipeline(const GP2Pipeline&) = delete;
		GP2Pipeline& operator=(const GP2Pipeline&) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	private:
		static std::vector<char> ReadFile(const std::string& filePath);

		void CreateGraphicsPipeline(
			const std::string& vertFilePath, 
			const std::string& fragFilePath,
			const PipelineConfigInfo& configInfo
		);

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		GP2Device& m_GP2Device;
		VkPipeline m_GraphicsPipeline;
		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;
	};
}