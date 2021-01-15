#pragma once

#include "Pyrokinetic/Rendering/GraphicsContext.h"

struct GLFWwindow;

namespace Pyrokinetic
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}