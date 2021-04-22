#pragma once

#include "RenderPass.h"
#include "imgui.h"

namespace pk
{

	struct FramebufferSpecification
	{
		uint32_t width, height;
		std::shared_ptr<RenderPass> renderPass;
		bool SwapchainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual ~Framebuffer() = default;

		virtual void Resize(const uint32_t width, const uint32_t height) = 0;

		virtual void* GetColorAttachment(uint32_t index = 0) const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
		//REMINDER: set spec/rebuild

		static std::shared_ptr<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}