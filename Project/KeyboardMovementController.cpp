#include "KeyboardMovementController.h"

namespace GP2
{
	void KeyboardMovementController::MoveInPlaneXZ(GLFWwindow* window, float dt, GP2GameObject& gameObject)
	{
		// Rotation
		if (glfwGetMouseButton(window, m_Keys.rotate) == GLFW_PRESS) 
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			if (!m_LeftMouseDown) 
			{
				m_PrevMousePos = glm::vec2{ xpos, ypos };
				m_LeftMouseDown = true;
			}
			glm::vec2 currentMousePos = glm::vec2{ xpos, ypos };
			glm::vec2 deltaMousePos = currentMousePos - m_PrevMousePos;

			gameObject.m_Transform.rotation.y += deltaMousePos.x * m_LookSpeed * dt;
			gameObject.m_Transform.rotation.x -= deltaMousePos.y * m_LookSpeed * dt;

			m_PrevMousePos = currentMousePos;
		}
		else 
		{
			m_LeftMouseDown = false;
		}

		gameObject.m_Transform.rotation.x = glm::clamp(gameObject.m_Transform.rotation.x, -1.5f, 1.5f);
		gameObject.m_Transform.rotation.y = glm::mod(gameObject.m_Transform.rotation.y, glm::two_pi<float>());

		// Movement
		float yaw = gameObject.m_Transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f,-1.f,0.f };
		float speed = m_MoveSpeed;

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(window, m_Keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, m_Keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window, m_Keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, m_Keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, m_Keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, m_Keys.moveDown) == GLFW_PRESS) moveDir -= upDir;
		if (glfwGetKey(window, m_Keys.speedUp) == GLFW_PRESS) speed *= 2;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			gameObject.m_Transform.translation += speed * dt * glm::normalize(moveDir);
		}
	}

	bool KeyboardMovementController::CycleRenderMode(GLFWwindow* window)
	{
		if(glfwGetKey(window, GLFW_KEY_F7) == GLFW_PRESS) 
		{
			if(!m_F7Pressed)
			{
				m_F7Pressed = true;
				return true;
			}
			
			return false;
		}
		m_F7Pressed = false;
		return false;
	}

	bool KeyboardMovementController::ToggleRotation(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
		{
			if (!m_F5Pressed)
			{
				m_F5Pressed = true;
				return true;
			}

			return false;
		}
		m_F5Pressed = false;
		return false;
	}

	bool KeyboardMovementController::ToggleNormalMap(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS)
		{
			if (!m_F6Pressed)
			{
				m_F6Pressed = true;
				return true;
			}

			return false;
		}
		m_F6Pressed = false;
		return false;
	}
}