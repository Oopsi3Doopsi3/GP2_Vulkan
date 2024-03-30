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

	std::unique_ptr<GP2Model> GP2Base::createCubeModel(GP2Device& device, glm::vec3 offset)
	{
		GP2Model::Builder modelBuilder{};
		modelBuilder.vertices = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};
		for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}

		modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
								12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<GP2Model>(device, modelBuilder);
	}
	
	void GP2Base::LoadGameObjects()
	{
		std::shared_ptr<GP2Model> gp2Model = createCubeModel(m_GP2Device, { 0.f,0.f,0.f });

		auto cube = GP2GameObject::CreateGameObject();
		cube.m_Model = gp2Model;
		cube.m_Transform.translation = { 0.f,0.f,2.5f };
		cube.m_Transform.scale = { .5f,.5f,.5f };
		m_GameObjects.push_back(std::move(cube));
	}
}