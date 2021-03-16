#pragma once

#include "Pyrokinetic.h"

namespace pk 
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(pk::Timestep timestep) override;
		virtual void OnImGuiRender() override;
		void OnEvent(pk::Event& e) override;

	private:
		OrthographicCameraController m_CameraController;



		//TODO: put in renderer
		std::shared_ptr<Texture2D> m_Texture;
		std::shared_ptr<Texture2D> m_Spritesheet;
		std::shared_ptr<SubTexture2D> m_StairSprite;
		std::shared_ptr<SubTexture2D> m_LargeTree;

		std::shared_ptr<Framebuffer> m_Framebuffer;

		std::shared_ptr<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_ClipspaceCameraEntity;
		bool m_PrimaryCamera = false;

		glm::vec2 m_ViewportSize;

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		struct TimerResult
		{
			const char* Name;
			int Time;
		};

		std::vector<TimerResult> m_TimerResults;
	};
}