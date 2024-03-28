#include "GP2Window.h"

//std
#include <stdexcept>

namespace GP2 
{
	GP2Window::GP2Window(int width, int height, const std::string& name) :
		m_Width(width),
		m_Height(height),
		m_WindowName(name)
	{
		InitWindow();
	}

	GP2Window::~GP2Window()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}
	
	void GP2Window::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	
		m_Window = glfwCreateWindow(m_Width, m_Height, m_WindowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);
	}

	void GP2Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface");
		}
	}

	void GP2Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto gp2Window = reinterpret_cast<GP2Window*>(glfwGetWindowUserPointer(window));
		gp2Window->m_FramebufferResized = true;
		gp2Window->m_Width = width;
		gp2Window->m_Height = height;
	}
}
