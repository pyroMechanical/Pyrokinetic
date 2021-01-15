#include "pkpch.h"
#include "VertexArray.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Pyrokinetic
{
	Ref<VertexArray> VertexArray::Create()
	{

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:       PK_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:     return CreateRef<OpenGLVertexArray>();
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
		
	}
}