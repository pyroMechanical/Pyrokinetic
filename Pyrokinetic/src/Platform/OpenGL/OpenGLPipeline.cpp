#include "pkpch.h"
#include "OpenGLPipeline.h"

#include <glad/glad.h>

namespace pk
{
	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec)
		:m_Spec(spec)
	{
		glCreateVertexArrays(1, &m_VertexArrayRendererID);
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
		glDeleteVertexArrays(1, &m_VertexArrayRendererID);
	}
}