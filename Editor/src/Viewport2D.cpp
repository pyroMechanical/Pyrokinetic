#include "Viewport2D.h"
#include "imgui.h"
#include "implot.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Pyrokinetic/Profiling/Instrumentor.h"

Viewport2D::Viewport2D()
	: Layer("2D Viewport"), m_CameraController(1280.0f/720.0f, true)
{
}
void Viewport2D::OnAttach()
{
	PROFILE_FUNCTION();

	m_Texture = Pyrokinetic::Texture2D::Create("assets/textures/checker.png");
	m_Spritesheet = Pyrokinetic::Texture2D::Create("assets/textures/RPGpack_sheet_2X.png");
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
	Pyrokinetic::Renderer2D::ResetStats();
	{
		PROFILE_SCOPE("Viewport2D::RenderPreparation()");
		Pyrokinetic::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1 });
		Pyrokinetic::RenderCommand::Clear();
	}

	{
		PROFILE_SCOPE("Viewport2D::RenderScene()");
		/*Pyrokinetic::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Pyrokinetic::Renderer2D::DrawQuad({ 1.5, 1.5 }, { 1.0f, 1.0f }, m_Texture, 45.0f);
		Pyrokinetic::Renderer2D::DrawQuad({ -1.5, -1.5 }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 0.5f, 1.0f }, -30.0f);

		Pyrokinetic::Renderer2D::EndScene();*/

		Pyrokinetic::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Pyrokinetic::Renderer2D::DrawQuad({ 0,0 }, { 1.0f, 1.0f }, m_Spritesheet, 0.0f);
		Pyrokinetic::Renderer2D::EndScene();
	}
}

void Viewport2D::OnImGuiRender()
{
	PROFILE_FUNCTION();
	auto stats = Pyrokinetic::Renderer2D::GetStats();
	ImGui::Begin("2D Render Stats");
	ImGui::Text("Draw Calls: %d", stats.drawCalls);
	ImGui::Text("Quads: %d", stats.quadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::End();
}

void Viewport2D::OnEvent(Pyrokinetic::Event& e)
{
	m_CameraController.OnEvent(e);
}