#pragma once

#include <glm/glm.hpp>

namespace pk
{
	struct CameraData
	{
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewprojection;
	};

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
		void SetProjection(float left, float right, float bottom, float top);

		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
		const glm::vec3& GetPosition() const { return m_Position; }

		void SetRotation(const float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
		const float GetRotation() const { return m_Rotation; }

		const glm::mat4& GetProjectionMatrix() const { return m_CameraData.projection; }
		const glm::mat4& GetViewMatrix() const { return m_CameraData.view; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_CameraData.viewprojection; }

		void SetProjectionMatrix(const glm::mat4& projectionMatrix) { m_CameraData.projection = projectionMatrix; RecalculateViewMatrix(); }
		void SetViewMatrix(const glm::mat4& viewMatrix) { m_CameraData.view = viewMatrix; RecalculateViewMatrix(); }
	private:
		void RecalculateViewMatrix();
	private:
		CameraData m_CameraData;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
	};

	class OrthographicCamera3D
	{
	public:
		OrthographicCamera3D(float left, float right, float bottom, float top);

		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
		const glm::vec3& GetPosition() const { return m_Position; }

		//quaternion Rotation currently not enabled.
		//void SetRotation(const glm::quaternion& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
		//const glm::quaternion& GetRotation() const { return m_Rotation; }
		void SetRotation(const float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
		const float GetRotation() const { return m_Rotation; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		void SetProjectionMatrix(const glm::mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; RecalculateViewMatrix(); }
		void SetViewMatrix(const glm::mat4& viewMatrix) { m_ViewMatrix = viewMatrix; RecalculateViewMatrix(); }
	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation; //TODO convert to quaternion
	};
}