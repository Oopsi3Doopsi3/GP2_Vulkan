#include "GP2Base.h"

#include "SimpleRenderSystem.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <cassert>
#include <array>

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

		while (!m_GP2Window.ShouldClose()){
			glfwPollEvents();
			
			if (auto commandBuffer = m_GP2Renderer.BeginFrame())
			{
				m_GP2Renderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderGameObjects(commandBuffer, m_GameObjects);
				m_GP2Renderer.EndSwapChainRenderPass(commandBuffer);
				m_GP2Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_GP2Device.Device());
	}
	
	void GP2Base::LoadGameObjects()
	{
		std::vector<GP2Model::Vertex> vertices{
			{{0.f,-.5f}, {1.f, 0.f, 0.f}},
			{{.5f,.5f}, {0.f, 1.f, 0.f}},
			{{-.5f,.5f}, {0.f, 0.f, 1.f}}
		};

		auto m_GP2Model = std::make_shared<GP2Model>(m_GP2Device, vertices);

		auto triangle = GP2GameObject::CreateGameObject();
		triangle.m_Model = m_GP2Model;
		triangle.m_Color = { .1f,.8f,.1f };
		triangle.m_Transform2d.translation.x = .2f;
		triangle.m_Transform2d.scale = { 2.f, .5f };
		triangle.m_Transform2d.rotation = .25 * glm::two_pi<float>();

		m_GameObjects.push_back(std::move(triangle));
	}
}