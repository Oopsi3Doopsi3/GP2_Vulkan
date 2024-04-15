#pragma once

#include "GP2Model.h"

//lib
#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>
#include <unordered_map>

namespace GP2
{
	struct TransformComponent
	{
		glm::vec3 translation{}; // position offset
		glm::vec3 scale{ 1.f,1.f,1.f };
		glm::vec3 rotation{};

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct PointLightComponent
	{
		float lightIntensity = 1.f;
	};

	class GP2GameObject
	{
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, GP2GameObject>;

		static GP2GameObject CreateGameObject()
		{
			static id_t currentId = 0;
			return GP2GameObject{ currentId++ };
		}

		static GP2GameObject MakePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

		GP2GameObject(const GP2GameObject&) = delete;
		GP2GameObject& operator=(const GP2GameObject&) = delete;
		GP2GameObject(GP2GameObject&&) = default;
		GP2GameObject& operator=(GP2GameObject&&) = default;

		id_t GetId() const { return m_Id; }

		glm::vec3 m_Color{};
		TransformComponent m_Transform{};

		//Optional components
		std::unique_ptr<GP2Model> m_Model{};
		std::unique_ptr<PointLightComponent> m_PointLight = nullptr;

	private:
		GP2GameObject(id_t objId) : m_Id(objId) {}

		id_t m_Id;
	};
}