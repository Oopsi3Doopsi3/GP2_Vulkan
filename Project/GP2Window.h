#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace GP2
{
	class GP2Window
	{
	public:
		GP2Window(int width, int height, const std::string& name);
		~GP2Window();
		
		GP2Window(const GP2Window&) = delete;
		GP2Window& operator=(const GP2Window&) = delete;
		
		bool ShouldClose() { return glfwWindowShouldClose(m_Window); }
		VkExtent2D GetExtent() { return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) }; }
		bool WasWindowResized() const { return m_FramebufferResized; }
		void ResetWindowResizedFlag() { m_FramebufferResized = false; }
		
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
		void InitWindow();

		int m_Width;
		int m_Height;
		bool m_FramebufferResized = false;

		std::string m_WindowName;
		GLFWwindow* m_Window;
	};
}