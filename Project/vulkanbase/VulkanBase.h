#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "VulkanUtil.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include <corecrt_math_defines.h>

#include "GP2Shader.h"
#include "Vertex.h"
#include "GP2CommandPool.h"
#include "GP2CommandBuffer.h"
#include "GP2RenderPass.h"
#include "GP2GraphicsPipeline.h"
#include "GP2Mesh.h"

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulkanBase {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	void initVulkan() {
		// week 06
		createInstance();
		setupDebugMessenger();
		createSurface();

		// week 05
		pickPhysicalDevice();
		createLogicalDevice();

		// week 04 
		createSwapChain();
		createImageViews();

		// week 03
		m_GradientShader.Initialize(device);
		m_RenderPass.CreateRenderPass(swapChainImageFormat, device);
		createFrameBuffers();
		m_GraphicsPipeline.CreateGraphicsPipeline(device, m_GradientShader, m_RenderPass);
		// week 02
		m_CommandPool.CreateCommandPool(findQueueFamilies(physicalDevice), device);
		m_CommandBuffer.SetCommandBuffer(m_CommandPool.CreateCommandBuffer(device));

		MakeCircle(m_CircleMesh, 100, .8f);
		MakeTriangle(m_TriangleMesh);

		// week 06
		createSyncObjects();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			// week 06
			drawFrame();
		}
		vkDeviceWaitIdle(device);
	}

	void cleanup() {
		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		vkDestroyFence(device, inFlightFence, nullptr);

		m_CommandPool.DestroyCommandPool(device);

		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		m_TriangleMesh.DestroyMesh(device);
		m_CircleMesh.DestroyMesh(device);

		m_GraphicsPipeline.Destroy(device);
		m_RenderPass.Destroy(device);

		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroySwapchainKHR(device, swapChain, nullptr);
		vkDestroyDevice(device, nullptr);

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}


	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	GP2Shader m_GradientShader{
		"shaders/shader.vert.spv",
		"shaders/shader.frag.spv"
	};

	GP2CommandPool m_CommandPool{};
	GP2CommandBuffer m_CommandBuffer{};

	GP2Mesh m_TriangleMesh{};
	GP2Mesh m_CircleMesh{};

	GP2RenderPass m_RenderPass{};
	GP2GraphicsPipeline m_GraphicsPipeline{};

	void MakeTriangle(GP2Mesh& mesh)
	{
		mesh.AddVertex({ 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f });
		mesh.AddVertex({ 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f });
		mesh.AddVertex({ -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f });

		mesh.Initialize(physicalDevice, device);
	}

	void MakeCircle(GP2Mesh& mesh, int nrSegments, float radius)
	{
		const double thetaIncrement = (2.0 * M_PI) / static_cast<double>(nrSegments);
		double theta = 0.0;
		
		std::vector<Vertex> circleVertices(nrSegments);
		for(int i{}; i < nrSegments; ++i)
		{
			circleVertices[i].pos.x = static_cast<float>(cos(theta)) * radius;
			circleVertices[i].pos.y = static_cast<float>(sin(theta)) * radius;
		
			circleVertices[i].color = glm::vec3{ 1.f,0.f,0.f};
			theta += thetaIncrement;
		}

		constexpr Vertex centerVertex{ {0.f,0.f},{1.f,1.f,1.f} };
		for(int i{}; i < nrSegments; ++i)
		{
			if(i == nrSegments - 1)
			{
				mesh.AddVertex(circleVertices[0]);
				mesh.AddVertex(centerVertex);
				mesh.AddVertex(circleVertices[i]);
			}
			else
			{
				mesh.AddVertex(circleVertices[i + 1]);
				mesh.AddVertex(centerVertex);
				mesh.AddVertex(circleVertices[i]);
			}
		}

		mesh.Initialize(physicalDevice, device);
	}

	// Week 01: 
	// Actual window
	// simple fragment + vertex shader creation functions
	// These 5 functions should be refactored into a separate C++ class
	// with the correct internal state.

	GLFWwindow* window;
	void initWindow();

	// Week 02
	// Queue families
	// CommandBuffer concept

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	void RecordCommandBuffer(uint32_t imageIndex);

	void drawFrame(uint32_t imageIndex, const VkCommandBuffer& commandBuffer);

	// Week 03
	// Renderpass concept
	// Graphics pipeline

	std::vector<VkFramebuffer> swapChainFramebuffers;

	void createFrameBuffers();

	// Week 04
	// Swap chain and image view support

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void createSwapChain();
	void createImageViews();

	// Week 05 
	// Logical and physical device

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	void createLogicalDevice();

	// Week 06
	// Main initialization

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();
	std::vector<const char*> getRequiredExtensions();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	void createInstance();

	void createSyncObjects();
	void drawFrame();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};