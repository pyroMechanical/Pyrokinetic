#pragma once
#include "Pyrokinetic/Rendering/Pipeline.h"

namespace pk
{
	class OpenGLPipeline : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		virtual ~OpenGLPipeline() override;

		virtual const PipelineSpecification& GetSpecification() override { return m_Spec; }

		virtual void Rebuild() override {};

		uint32_t GetVertexArrayRendererID() const { return m_VertexArrayRendererID; }

	private:
		PipelineSpecification m_Spec;

		uint32_t m_VertexArrayRendererID;
	};
}