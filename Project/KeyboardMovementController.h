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
			int speedUp = GLFW_KEY_LEFT_SHIFT;
			int toggleRotation = GLFW_KEY_F5;
			int toggleNormalMap = GLFW_KEY_F6;
			int cycleRenderMode = GLFW_KEY_F7;
		};

		void MoveInPlaneXZ(GLFWwindow* window, float dt, GP2GameObject& gameObject);
		bool CycleRenderMode(GLFWwindow* window);
		bool ToggleRotation(GLFWwindow* window);
		bool ToggleNormalMap(GLFWwindow* window);
		
	private:
		keyMappings m_Keys{};
		float m_MoveSpeed{ 5.f };
		float m_LookSpeed{ .5f };

		glm::vec2 m_MousePos{ 0.0f, 0.0f };
		glm::vec2 m_PrevMousePos{ 0.0f, 0.0f };
		bool m_LeftMouseDown{ false };
		bool m_F5Pressed{ false };
		bool m_F6Pressed{ false };
		bool m_F7Pressed{ false };
	};
}

