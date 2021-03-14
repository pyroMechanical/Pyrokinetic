#pragma once

#include "Pyrokinetic/Rendering/GraphicsContext.h"

struct GLFWwindow;

namespace pk
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void SetVSync(bool enabled) override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}