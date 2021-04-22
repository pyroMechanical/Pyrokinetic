#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <unordered_set>

namespace pk
{
	enum class ImageFormat
	{
		None = 0,

		// color formats
		RGBA8,

		// depth formats
		DEPTH24STENCIL8,
		DEPTH32,

		//defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(ImageFormat format)
			: TextureFormat(format) {}

		ImageFormat TextureFormat = ImageFormat::None;

		//add filtering/wrapping vars
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}
		std::vector<FramebufferTextureSpecification> Attachments;
	};

	enum class ImageLayout
	{
		SwapchainPresent = 0,
		ShaderReadOnly
	};

	struct RenderPassSpecification
	{
		FramebufferAttachmentSpecification Attachments;
		uint32_t samples = 1;
		ImageLayout layout = ImageLayout::SwapchainPresent;
		glm::vec4 clearColor;
	};

	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		virtual RenderPassSpecification GetSpecification() const = 0;

		static std::shared_ptr<RenderPass> Create(RenderPassSpecification& spec);
	};
}