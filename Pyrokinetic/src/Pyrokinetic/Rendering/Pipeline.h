#pragma once

#include "Pyrokinetic/Core/Core.h"
#include "Buffer.h"
#include "Shader.h"

namespace pk
{
	struct PipelineSpecification
	{
		std::shared_ptr<Shader> Shader;
		BufferLayout vertexBufferLayout;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual const PipelineSpecification& GetSpecification() = 0;

		static std::shared_ptr<Pipeline> Create(const PipelineSpecification& spec);
	};
}