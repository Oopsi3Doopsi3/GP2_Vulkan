#pragma once

#include "GP2Model.h"

//std
#include <memory>

namespace GP2
{
	struct Transform2dComponent
	{
		glm::vec2 translation{}; // position offset
		glm::vec2 scale{ 1.f,1.f };
		float rotation;

		glm::mat2 mat2() 
		{ 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMat{{c, s}, {-s, c}};

			glm::mat2 scaleMat{ {scale.x, 0.f}, {0.f, scale.y} };
			return rotMat * scaleMat;
		}
	};

	class GP2GameObject
	{
	public:
		using id_t = unsigned int;

		static GP2GameObject CreateGameObject()
		{
			static id_t currentId = 0;
			return GP2GameObject{ currentId++ };
		}

		GP2GameObject(const GP2GameObject&) = delete;
		GP2GameObject& operator=(const GP2GameObject&) = delete;
		GP2GameObject(GP2GameObject&&) = default;
		GP2GameObject& operator=(GP2GameObject&&) = default;

		id_t GetId() const { return m_Id; }

		std::shared_ptr<GP2Model> m_Model{};
		glm::vec3 m_Color{};
		Transform2dComponent m_Transform2d{};

	private:
		GP2GameObject(id_t objId) : m_Id(objId) {}

		id_t m_Id;
	};
}