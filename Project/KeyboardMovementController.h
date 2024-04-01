#pragma once

#include "GP2GameObject.h"
#include "GP2Window.h"

namespace GP2
{
	class KeyboardMovementController
	{
	public:
		struct keyMappings
		{
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveUp = GLFW_KEY_E;
			int moveDown = GLFW_KEY_Q;
			int rotate = GLFW_MOUSE_BUTTON_1;
		};

		void MoveInPlaneXZ(GLFWwindow* window, float dt, GP2GameObject& gameObject);
		
	private:
		keyMappings m_Keys{};
		float m_MoveSpeed{ 3.f };
		float m_LookSpeed{ .5f };

		glm::vec2 m_MousePos{ 0.0f, 0.0f };
		glm::vec2 m_PrevMousePos{ 0.0f, 0.0f };
		bool m_LeftMouseDown{ false };
	};
}

