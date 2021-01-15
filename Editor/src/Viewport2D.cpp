#include "Viewport2D.h"
#include "imgui.h"
#include "implot.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Pyrokinetic/Profiling/Instrumentor.h"

Viewport2D::Viewport2D()
	: Layer("2D Viewport"), m_CameraController(1280.0f/720.0f)
{
}
void Viewport2D::OnAttach()
{
	PROFILE_FUNCTION();

	m_Texture = Pyrokinetic::Texture2D::Create("assets/textures/ferengi.png");
}

void Viewport2D::OnDetach()
{
	PROFILE_FUNCTION();

}

void Viewport2D::OnUpdate(Pyrokinetic::Timestep timestep)
{
	PROFILE_FUNCTION();
	{
		PROFILE_SCOPE("OrthographicCamera::OnUpdate()");
		m_CameraController.OnUpdate(timestep);
	}

	{
		PROFILE_SCOPE("Viewport2D::RenderPreparation()");
		Pyrokinetic::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1 });
		Pyrokinetic::RenderCommand::Clear();
	}

	{
		PROFILE_SCOPE("Viewport2D::RenderScene()");
		Pyrokinetic::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Pyrokinetic::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.3f, 0.2f, 0.8f, 1.0f }, 0.5f);
		Pyrokinetic::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.01f }, { 0.5f, 0.75f }, m_Texture, -0.5f, 20.0f);
		Pyrokinetic::Renderer2D::EndScene();
	}
}

void Viewport2D::OnImGuiRender()
{
	PROFILE_FUNCTION();
}

void Viewport2D::OnEvent(Pyrokinetic::Event& e)
{
	m_CameraController.OnEvent(e);
}