#include "GP2Base.h"

#include "systems/SimpleRenderSystem.h"
#include "systems/PointLightSystem.h"
#include "GP2Camera.h"
#include "KeyboardMovementController.h"
#include "GP2Buffer.h"

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
		m_GlobalPool = GP2DescriptorPool::Builder(m_GP2Device)
			.SetMaxSets(GP2SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, GP2SwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();

		LoadGameObjects();
	}
	
	GP2Base::~GP2Base()
	{
		
	}
	
	void GP2Base::Run()
	{
		std::vector<std::unique_ptr<GP2Buffer>> uboBuffers(GP2SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i{}; i < uboBuffers.size(); ++i)
		{
			uboBuffers[i] = std::make_unique<GP2Buffer>(
				m_GP2Device,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->Map();
		}

		auto globalSetLayout = GP2DescriptorSetLayout::Builder(m_GP2Device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.Build();

		std::vector<VkDescriptorSet> globalDescriptorSets(GP2SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i{}; i < globalDescriptorSets.size(); ++i)
		{
			auto bufferInfo = uboBuffers[i]->DescriptorInfo();
			GP2DescriptorWriter(*globalSetLayout, *m_GlobalPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ m_GP2Device, m_GP2Renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout() };
		PointLightSystem pointLightSystem{ m_GP2Device, m_GP2Renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout() };
		GP2Camera camera{};
		camera.SetViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

		auto viewerObject = GP2GameObject::CreateGameObject();
		viewerObject.m_Transform.translation.z = -2.5f;
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
			camera.SetPerspectiveProjection(glm::radians(50.f), aspect, .1f, 100.f);
			
			if (auto commandBuffer = m_GP2Renderer.BeginFrame())
			{
				int frameIndex = m_GP2Renderer.GetFrameIndex();
				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex], m_GameObjects };

				//update
				GlobalUbo ubo{};
				ubo.projection = camera.GetProjection();
				ubo.view = camera.GetView();
				ubo.inversView = camera.GetInverseView();
				pointLightSystem.Update(frameInfo, ubo);
				uboBuffers[frameIndex]->WriteToBuffer(&ubo);
				uboBuffers[frameIndex]->Flush();

				//render
				m_GP2Renderer.BeginSwapChainRenderPass(commandBuffer);

				//Order matters for transparency
				simpleRenderSystem.RenderGameObjects(frameInfo);
				pointLightSystem.Render(frameInfo);


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
		flatVase.m_Transform.translation = { -.5f,.5f,0.f };
		flatVase.m_Transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		m_GameObjects.emplace(flatVase.GetId(), std::move(flatVase));

		gp2Model = GP2Model::CreateModelFromFile(m_GP2Device, "C:\\Graphics Programming 2\\Vulkan\\Project\\models\\smooth_vase.obj");
		auto smoothVase = GP2GameObject::CreateGameObject();
		smoothVase.m_Model = gp2Model;
		smoothVase.m_Transform.translation = { 0.5f,.5f,0.f };
		smoothVase.m_Transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		m_GameObjects.emplace(smoothVase.GetId(), std::move(smoothVase));

		gp2Model = GP2Model::CreateModelFromFile(m_GP2Device, "C:\\Graphics Programming 2\\Vulkan\\Project\\models\\quad.obj");
		auto floor = GP2GameObject::CreateGameObject();
		floor.m_Model = gp2Model;
		floor.m_Transform.translation = { 0.f,.5f,0.f };
		floor.m_Transform.scale = glm::vec3{ 3.f, 1.f, 3.f };
		m_GameObjects.emplace(floor.GetId(), std::move(floor));

		std::vector<glm::vec3> lightColors{
		{1.f, .1f, .1f},
		{.1f, .1f, 1.f},
		{.1f, 1.f, .1f},
		{1.f, 1.f, .1f},
		{.1f, 1.f, 1.f},
		{1.f, 1.f, 1.f}
		};

		for (int i{}; i < lightColors.size(); ++i)
		{
			auto pointLight = GP2GameObject::MakePointLight(.2f);
			pointLight.m_Color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f });
			pointLight.m_Transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			m_GameObjects.emplace(pointLight.GetId(), std::move(pointLight));
		}
	}
}