#pragma once
#include "Framebuffer.h"

#include <memory>

namespace pk
{
	struct RenderPassSpecification
	{
		std::shared_ptr<Framebuffer> TargetFramebuffer;
	};

	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		virtual RenderPassSpecification GetSpecification() const = 0;

		static std::shared_ptr<RenderPass> Create(RenderPassSpecification& spec);
	};
}