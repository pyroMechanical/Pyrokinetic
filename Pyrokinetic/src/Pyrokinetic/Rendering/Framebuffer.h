#pragma once

#include "Pyrokinetic/Core/Core.h"

namespace pk
{
	struct FramebufferSpecification
	{
		uint32_t width, height;
		//FramebufferFormat format = 
		uint32_t samples = 1;

		bool SwapchainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual void Bind() = 0; 
		virtual void Unbind() = 0;

		virtual ~Framebuffer() = default;

		virtual void Resize(const uint32_t width, const uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
		//REMINDER: set spec/rebuild

		static std::shared_ptr<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}