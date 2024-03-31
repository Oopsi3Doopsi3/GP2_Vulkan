#include "GP2Base.h"

#include "SimpleRenderSystem.h"
#include "GP2Camera.h"
#include "KeyboardMovementController.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <cassert>
#include <array>
#include <chrono>

namespace GP2
{
	GP2Base::GP2Base()
	{
		LoadGameObjects();
	}
	
	GP2Base::~GP2Base()
	{
		
	}
	
	void GP2Base::Run()
	{
		SimpleRenderSystem simpleRenderSystem(m_GP2Device, m_GP2Renderer.GetSwapChainRenderPass());
		GP2Camera camera{};
		camera.SetViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

		auto viewerObject = GP2GameObject::CreateGameObject();
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!m_GP2Window.ShouldClose()){
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration <float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.MoveInPlaneXZ(m_GP2Window.GetGLFWwindow(), frameTime, viewerObject);
			camera.SetViewYXZ(viewerObject.m_Transform.translation, viewerObject.m_Transform.rotation);

			float aspect = m_GP2Renderer.GetAspectRatio();
			camera.SetPerspectiveProjection(glm::radians(50.f), aspect, .1f, 10.f);
			
			if (auto commandBuffer = m_GP2Renderer.BeginFrame())
			{
				m_GP2Renderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderGameObjects(commandBuffer, m_GameObjects, camera);
				m_GP2Renderer.EndSwapChainRenderPass(commandBuffer);
				m_GP2Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_GP2Device.Device());
	}
	
	void GP2Base::LoadGameObjects()
	{
		std::shared_ptr<GP2Model> gp2Model = GP2Model::CreateModelFromFile(m_GP2Device, "C:\\Graphics Programming 2\\Vulkan\\Project\\models\\flat_vase.obj");
		auto flatVase = GP2GameObject::CreateGameObject();
		flatVase.m_Model = gp2Model;
		flatVase.m_Transform.translation = { -.5f,.5f,2.5f };
		flatVase.m_Transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		m_GameObjects.push_back(std::move(flatVase));

		gp2Model = GP2Model::CreateModelFromFile(m_GP2Device, "C:\\Graphics Programming 2\\Vulkan\\Project\\models\\smooth_vase.obj");
		auto smoothVase = GP2GameObject::CreateGameObject();
		smoothVase.m_Model = gp2Model;
		smoothVase.m_Transform.translation = { 0.5f,.5f,2.5f };
		smoothVase.m_Transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		m_GameObjects.push_back(std::move(smoothVase));
	}
}