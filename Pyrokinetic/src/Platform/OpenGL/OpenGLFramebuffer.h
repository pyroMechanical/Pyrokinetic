#pragma once

#include "Pyrokinetic/Rendering/Framebuffer.h"

namespace pk
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Rebuild();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(const uint32_t width, const uint32_t height) override;

		virtual void* GetColorAttachment(uint32_t index = 0) const override { PK_CORE_ASSERT(index < m_ColorAttachments.size(), "Color Attachment Index out of range!");return (void*)m_ColorAttachments[index]; }

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Spec; }
	private:
		uint32_t m_RendererID = 0;
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;

		FramebufferTextureSpecification m_DepthAttachmentSpecification = ImageFormat::None;
		FramebufferSpecification m_Spec;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}