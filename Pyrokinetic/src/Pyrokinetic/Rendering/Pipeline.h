#pragma once

#include "Pyrokinetic/Core/Core.h"
#include "RenderPass.h"
#include "Buffer.h"
#include "Shader.h"

namespace pk
{
	struct PipelineSpecification
	{
		std::shared_ptr<Shader> Shader;
		BufferLayout vertexBufferLayout;
		std::shared_ptr<RenderPass> RenderPass;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual const PipelineSpecification& GetSpecification() = 0;

		virtual void Rebuild() = 0;
		
		static std::shared_ptr<Pipeline> Create(const PipelineSpecification& spec);
	};
}