#include <Pyrokinetic.h>

#include "Pyrokinetic/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EditorLayer.h"
namespace pk
{

	class Editor : public Application
	{
	public:
		Editor()
		{
			PushLayer(new EditorLayer());
		}

		~Editor()
		{
		}
	};

	Application* CreateApplication()
	{
		RendererAPI::SetAPI(RendererAPI::API::OpenGL);
		return new Editor();
	}
}