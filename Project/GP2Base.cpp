#include "GP2Base.h"

#include "systems/SimpleRenderSystem.h"
#include "systems/PointLightSystem.h"
#include "systems/SimpleRenderSystem2D.h"
#include "systems/TessellationSystem.h"

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

		SimpleRenderSystem2D simpleRenderSystem2D{ m_GP2Device, m_GP2Renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout() };
		SimpleRenderSystem simpleRenderSystem{ m_GP2Device, m_GP2Renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout() };

		//TessellationSystem tessellationSystem{ m_GP2Device, m_GP2Renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout() };

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
				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};

				//update
				GlobalUbo ubo{};
				ubo.projection = camera.GetProjection();
				ubo.view = camera.GetView();
				ubo.inversView = camera.GetInverseView();
				pointLightSystem.Update(frameInfo, ubo, m_GameObjects);
				uboBuffers[frameIndex]->WriteToBuffer(&ubo);
				uboBuffers[frameIndex]->Flush();

				//render
				m_GP2Renderer.BeginSwapChainRenderPass(commandBuffer);

				//Order matters for transparency
				//simpleRenderSystem2D.RenderGameObjects(frameInfo, m_2DGameObjects);
				simpleRenderSystem.RenderGameObjects(frameInfo, m_GameObjects);

				//tessellationSystem.RenderGameObjects(frameInfo, m_TesGameObjects);

				pointLightSystem.Render(frameInfo, m_GameObjects);


				m_GP2Renderer.EndSwapChainRenderPass(commandBuffer);
				m_GP2Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_GP2Device.Device());
	}
	
	void GP2Base::LoadGameObjects()
	{
		//2D
		const GP2Model::Builder rectangleModel{
		{
			{{-0.95f, -0.95f, 0.f}, {1.0f, 0.0f, 0.0f}},
			{{-0.70f, -0.95f, 0.f}, {0.0f, 1.0f, 0.0f}},
			{{-0.70f, -0.70f, 0.f}, {0.0f, 0.0f, 1.0f}},
			{{-0.95f, -0.70f, 0.f}, {1.0f, 1.0f, 1.0f}},
		},
			{
				0, 1, 2, 2, 3, 0
			}
		};
		auto mesh = std::make_unique<GP2Model>(m_GP2Device, rectangleModel);
		auto rectangle = GP2GameObject::CreateGameObject();
		rectangle.m_Model = std::move(mesh);
		m_2DGameObjects.emplace(rectangle.GetId(), std::move(rectangle));

		//circle
		constexpr int nrSegments = 100;
		constexpr float radius = .25f;
		constexpr double pi = 3.14159;
		const double thetaIncrement = (2.0 * pi) / static_cast<double>(nrSegments);
		double theta = 0.0;
		
		std::vector<GP2Model::Vertex> circleVertices(nrSegments);
		for (int i{}; i < nrSegments; ++i)
		{
			circleVertices[i].position.x = static_cast<float>(cos(theta)) * radius + 0.6f;
			circleVertices[i].position.y = static_cast<float>(sin(theta)) * radius - 0.6f;
		
			circleVertices[i].color = glm::vec3{ sin(theta),cos(theta),tan(theta)};
			theta += thetaIncrement;
		}
		circleVertices.push_back(GP2Model::Vertex{ {.6f,-0.6f,0.f},{1.f,1.f,1.f} });
		
		std::vector<uint32_t> circleIndices;
		for (int i{}; i < nrSegments; ++i)
		{
			if (i < nrSegments - 1)
			{
				circleIndices.push_back(i);
				circleIndices.push_back(nrSegments);
				circleIndices.push_back(i + 1);
			}
			else
			{
				circleIndices.push_back(i);
				circleIndices.push_back(nrSegments);
				circleIndices.push_back(0);
			}
		}
		const GP2Model::Builder circleModel{ circleVertices, circleIndices };
		mesh = std::make_unique<GP2Model>(m_GP2Device, circleModel);
		auto circle = GP2GameObject::CreateGameObject();
		circle.m_Model = std::move(mesh);
		m_2DGameObjects.emplace(circle.GetId(), std::move(circle));


		//3D
		auto gp2Model = GP2Model::CreateModelFromFile(m_GP2Device, "models/vehicle.obj");
		auto flatVase = GP2GameObject::CreateGameObject();
		flatVase.m_Model = std::move(gp2Model);
		flatVase.m_Transform.translation = { 0.f,.5f, 10.f };
		flatVase.m_Transform.scale = glm::vec3{ .5f, .5f, .5f };
		flatVase.m_Transform.rotation = glm::vec3{ glm::radians(180.f),0.f,0.f};
		m_GameObjects.emplace(flatVase.GetId(), std::move(flatVase));

		//gp2Model = GP2Model::CreateModelFromFile(m_GP2Device, "models/flat_vase.obj");
		//auto flatVaseTes = GP2GameObject::CreateGameObject();
		//flatVaseTes.m_Model = std::move(gp2Model);
		//flatVaseTes.m_Transform.translation = { 0.5f,.5f,0.f };
		//flatVaseTes.m_Transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		//m_TesGameObjects.emplace(flatVaseTes.GetId(), std::move(flatVaseTes));

		//gp2Model = GP2Model::CreateModelFromFile(m_GP2Device, "models/smooth_vase.obj");
		//auto smoothVase = GP2GameObject::CreateGameObject();
		//smoothVase.m_Model = std::move(gp2Model);
		//smoothVase.m_Transform.translation = { 0.5f,.5f,0.f };
		//smoothVase.m_Transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		//m_GameObjects.emplace(smoothVase.GetId(), std::move(smoothVase));

		gp2Model = GP2Model::CreateModelFromFile(m_GP2Device, "models/quad.obj");
		auto floor = GP2GameObject::CreateGameObject();
		floor.m_Model = std::move(gp2Model);
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