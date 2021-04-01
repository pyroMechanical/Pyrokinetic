#pragma once

#include "Pyrokinetic/Rendering/RenderPass.h"

namespace pk
{
	class OpenGLRenderPass : public RenderPass
	{
	public:
		OpenGLRenderPass(RenderPassSpecification& spec);
		virtual ~OpenGLRenderPass() = default;

		virtual RenderPassSpecification GetSpecification() const override { return m_Spec; }
	private:
		RenderPassSpecification m_Spec;
	};
}