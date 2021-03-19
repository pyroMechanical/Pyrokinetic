#pragma once

#include "Pyrokinetic/Rendering/Camera.h"

namespace pk
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float fov, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		float GetPerspectiveFOV() const { return m_VerticalFOV; }
		void SetPerspectiveFOV(float fov) { m_VerticalFOV = fov; RecalculateProjection(); }

		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }

		void SetPerspectiveNearClip(float nearClip)  { m_PerspectiveNear = nearClip; RecalculateProjection(); }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
		void SetPerspectiveFarClip(float farClip)   { m_PerspectiveFar = farClip; RecalculateProjection(); }
		void SetOrthographicFarClip(float farClip)  {m_OrthographicFar = farClip; RecalculateProjection(); }

		bool IsFarPlaneInfinite() const { return m_Infinite; }
		void SetFarPlaneInfinite(bool infinite) { m_Infinite = infinite; RecalculateProjection(); }

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(int i) { m_ProjectionType = ProjectionType{ i }; RecalculateProjection(); }

	private:
		void RecalculateProjection();
	private:
		ProjectionType m_ProjectionType = ProjectionType::Perspective;

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = 0.0f, m_OrthographicFar = 10000.0f;

		float m_VerticalFOV = 70.0f;
		float m_PerspectiveNear = 0.3f, m_PerspectiveFar = 10000.0f;
		bool m_Infinite = false;
		
		float m_AspectRatio = 1.0f;
	};
}