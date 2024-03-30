#pragma once

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>

namespace GP2
{
	class GP2Camera
	{
	public:

		void SetOrthographicProjection(
			float left, float right,
			float top, float bottom,
			float near, float far);
		void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }

	private:
		glm::mat4 m_ProjectionMatrix{ 1.f };
	};
}
