#pragma once

#include "Pyrokinetic/Rendering/RenderPass.h"
#include "OpenGLFramebuffer.h"

namespace pk
{
	class OpenGLRenderPass : public RenderPass
	{
	public:
		OpenGLRenderPass(RenderPassSpecification& spec);
		virtual ~OpenGLRenderPass() = default;

		virtual RenderPassSpecification GetSpecification() const override { return m_Spec; }

		std::unordered_set<OpenGLFramebuffer*> GetFramebuffers() { return m_Framebuffers; }

		void AddFramebuffer(OpenGLFramebuffer* framebuffer) { m_Framebuffers.emplace(framebuffer); }

		void RemoveFramebuffer(OpenGLFramebuffer* framebuffer) { m_Framebuffers.erase(framebuffer); }
	private:
		RenderPassSpecification m_Spec;

		std::unordered_set<OpenGLFramebuffer*> m_Framebuffers;
	};
}