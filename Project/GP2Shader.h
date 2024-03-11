#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <array>

class GP2Shader
{
public:
	GP2Shader(
		const std::string& vertexShaderFile,
		const std::string& fragmentShaderFile
	) : m_VertexShaderFile(vertexShaderFile),
		m_FragmentShaderFile(fragmentShaderFile)
	{}

	~GP2Shader() = default;
	GP2Shader(const GP2Shader&) = delete;
	GP2Shader& operator=(const GP2Shader&) = delete;
	GP2Shader(const GP2Shader&&) = delete;
	GP2Shader& operator=(const GP2Shader&&) = delete;

	std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() { return m_ShaderStages; }

	void Initialize(const VkDevice& vkDevice);
	void DestroyShaderModules(const VkDevice& vkDevice);

	VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo();
	VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();

private:
	VkPipelineShaderStageCreateInfo CreateFragmentShaderInfo(const VkDevice& vkDevice);
	VkPipelineShaderStageCreateInfo CreateVertexShaderInfo(const VkDevice& vkDevice);
	VkShaderModule CreateShaderModule(const VkDevice& vkDevice, const std::vector<char>& code);

	std::string m_VertexShaderFile;
	std::string m_FragmentShaderFile;

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

	VkVertexInputBindingDescription m_BindingDescription;
	std::array<VkVertexInputAttributeDescription, 2> m_AttributeDescriptions;
};
