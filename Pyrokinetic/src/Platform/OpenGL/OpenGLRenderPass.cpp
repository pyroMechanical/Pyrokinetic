#include "pkpch.h"
#include "OpenGLRenderPass.h"

#include <glad/glad.h>

namespace pk
{
	OpenGLRenderPass::OpenGLRenderPass(RenderPassSpecification& spec)
		: m_Spec(spec) 
	{
	}
}