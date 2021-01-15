#include "pkpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace Pyrokinetic 
{
	
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) 
		: m_WindowHandle(windowHandle)
	{
		PK_CORE_ASSERT(m_WindowHandle, "Handle is null!");
	}

	void OpenGLContext::Init()
	{
		PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PK_CORE_ASSERT(status, "Failed to initialize Glad!");

		PK_CORE_INFO("OpenGL Info");
		PK_CORE_INFO("Vendor: {0}", glGetString(GL_VENDOR));
		PK_CORE_INFO("Renderer: {0}", glGetString(GL_RENDERER));
		PK_CORE_INFO("GL Version: {0}", glGetString(GL_VERSION));

#ifdef PK_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		PK_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Requires at least OpenGL version 4.5")
#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}