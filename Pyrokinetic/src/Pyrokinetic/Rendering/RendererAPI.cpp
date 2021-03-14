#include "pkpch.h"
#include "Pyrokinetic/Rendering/RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace pk
{
#ifdef PK_VULKAN_SUPPORTED
	API RendererAPI::s_API = API::Vulkan;
#else
	API RendererAPI::s_API = API::OpenGL;
#endif
	std::unique_ptr<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case API::None: PK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case API::OpenGL: return std::make_unique<OpenGLRendererAPI>();
		case API::Vulkan: return std::make_unique<VulkanRendererAPI>();
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;

	}
}