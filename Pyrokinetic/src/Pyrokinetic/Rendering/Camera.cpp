#include "pkpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace pk
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	{
		PROFILE_FUNCTION();

		m_CameraData.projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_CameraData.view = glm::mat4(1.0f);

		m_CameraData.viewprojection = m_CameraData.projection * m_CameraData.view;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position);
		transform = glm::rotate(transform, glm::radians(m_Rotation), { 0, 0, 1 });
		m_CameraData.view = glm::inverse(transform);
		m_CameraData.viewprojection = m_CameraData.projection * m_CameraData.view;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		PROFILE_FUNCTION();

		m_CameraData.projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_CameraData.viewprojection = m_CameraData.projection * m_CameraData.view;
	}
}