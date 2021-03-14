#pragma once
#include "Pyrokinetic/Core/Window.h"
#include "Pyrokinetic/Rendering/GraphicsContext.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace pk
{

	class MacWindow : public Window
	{
	public:
		MacWindow(const WindowProps& props);
		virtual ~MacWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }


		inline virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; }

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;

		std::unique_ptr<GraphicsContext> m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}
