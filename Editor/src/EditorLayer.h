#pragma once

#include "Pyrokinetic.h"

#include "Panels/SceneHierarchy.h"
#include "Panels/Properties.h"

namespace pk 
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep t) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

		void ResizeViewport();

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
		Entity m_SquareEntity2;
		Entity m_CameraEntity;
		Entity m_ClipspaceCameraEntity;
		bool m_PrimaryCamera = false;

		glm::vec2 m_ViewportSize;

		SceneHierarchyPanel m_SceneHierarchy;
		PropertiesPanel m_Properties;

		struct TimerResult
		{
			const char* Name;
			int Time;
		};

		std::vector<TimerResult> m_TimerResults;
	};
}