#include "pkpch.h"
#include "Pyrokinetic/Rendering/RenderCommand.h"

namespace Pyrokinetic
{
	std::unique_ptr<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}