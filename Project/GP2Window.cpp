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

	//void GP2Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	//{
	//	if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS) {
	//		throw std::runtime_error("Failed to create window surface");
	//	}
	//}
	
	void GP2Window::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	
		m_Window = glfwCreateWindow(m_Width, m_Height, m_WindowName.c_str(), nullptr, nullptr);
	}
}
