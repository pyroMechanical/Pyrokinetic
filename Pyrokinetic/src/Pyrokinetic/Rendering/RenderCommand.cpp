#include "pkpch.h"
#include "Pyrokinetic/Rendering/RenderCommand.h"

namespace pk
{
	std::unique_ptr<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}