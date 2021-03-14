#include <Pyrokinetic.h>

#include "Pyrokinetic/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Viewport2D.h"

	class Editor : public pk::Application
	{
	public:
		Editor()
		{
			//PushLayer(new ExampleLayer());
			PushLayer(new Viewport2D());
		}

		~Editor()
		{
		}
	};

	pk::Application* pk::CreateApplication()
	{
		return new Editor();
	}