#pragma once

#include "Pyrokinetic.h"

class Viewport2D : public pk::Layer
{
public:
	Viewport2D();

	virtual ~Viewport2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(pk::Timestep timestep) override;
	virtual void OnImGuiRender() override;
	void OnEvent(pk::Event& e) override;

private:
	pk::OrthographicCameraController m_CameraController;



	//TODO: put in renderer
	std::shared_ptr<pk::Texture2D> m_Texture;
	std::shared_ptr<pk::Texture2D> m_Spritesheet;
	std::shared_ptr<pk::SubTexture2D> m_StairSprite;
	std::shared_ptr<pk::SubTexture2D> m_LargeTree;

	std::shared_ptr<pk::Framebuffer> m_Framebuffer;
	glm::vec2 m_ViewportSize;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	struct TimerResult
	{
		const char* Name;
		int Time;
	};

	std::vector<TimerResult> m_TimerResults;
};