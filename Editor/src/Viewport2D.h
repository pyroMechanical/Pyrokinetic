#pragma once

#include "Pyrokinetic.h"

class Viewport2D : public Pyrokinetic::Layer
{
public:
	Viewport2D();

	virtual ~Viewport2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Pyrokinetic::Timestep timestep) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Pyrokinetic::Event& e) override;

private:
	Pyrokinetic::OrthographicCameraController m_CameraController;



	//TODO: put in renderer
	std::shared_ptr<Pyrokinetic::Texture2D> m_Texture;
	std::shared_ptr<Pyrokinetic::Texture2D> m_Spritesheet;
	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	struct TimerResult
	{
		const char* Name;
		int Time;
	};

	std::vector<TimerResult> m_TimerResults;
};