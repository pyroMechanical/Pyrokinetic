#include "pkpch.h"
#include "CameraController.h"
#include "Pyrokinetic/Core/Input.h"
#include "Pyrokinetic/Core/KeyCodes.h"

namespace Pyrokinetic
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), m_Rotation(rotation), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
			
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep timestep)
	{
		PROFILE_FUNCTION();

		if (Input::IsKeyPressed(PK_KEY_A) || Input::IsKeyPressed(PK_KEY_LEFT))
		{
			m_CameraPosition.x -= m_CameraTranslationSpeed * timestep;
		}
		if (Input::IsKeyPressed(PK_KEY_D) || Input::IsKeyPressed(PK_KEY_RIGHT))
		{
			m_CameraPosition.x += m_CameraTranslationSpeed * timestep;
		}
		if (Input::IsKeyPressed(PK_KEY_W) || Input::IsKeyPressed(PK_KEY_UP))
		{
			m_CameraPosition.y += m_CameraTranslationSpeed * timestep;
		}
		if (Input::IsKeyPressed(PK_KEY_S) || Input::IsKeyPressed(PK_KEY_DOWN))
		{
			m_CameraPosition.y -= m_CameraTranslationSpeed * timestep;
		}
		if (m_Rotation)
		{
			if (Input::IsKeyPressed(PK_KEY_Q))
			{
				m_CameraRotation += m_CameraRotationSpeed * timestep;
			}
			if (Input::IsKeyPressed(PK_KEY_E))
			{
				m_CameraRotation -= m_CameraRotationSpeed * timestep;
			}

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);

		m_CameraTranslationSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(PK_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(PK_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		PROFILE_FUNCTION();

		m_AspectRatio = (float) e.GetWidth() / (float) e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}