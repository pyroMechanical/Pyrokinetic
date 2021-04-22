#include "pkpch.h"
#include "VulkanRenderPass.h"
#include "Pyrokinetic/Rendering/Renderer.h"

namespace pk
{
	namespace util
	{
		static bool IsDepthFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH24STENCIL8: return true;
			}

			return false;
		}

		static VkImageLayout GetSpecificationLayout(ImageLayout layout)
		{
			switch(layout)
			{
			case ImageLayout::SwapchainPresent: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			case ImageLayout::ShaderReadOnly:   return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}
	VulkanRenderPass::VulkanRenderPass(const RenderPassSpecification& spec)
		: m_Spec(spec)
	{
		VulkanContext* context = VulkanContext::Get();

		VkDevice device = context->GetDevice()->GetVulkanDevice();

		std::vector<VkAttachmentDescription> attachmentDescriptions;
		std::vector<VkAttachmentReference> colorAttachmentReferences;
		VkAttachmentReference depthAttachmentReference = {};
		depthAttachmentReference.attachment = VK_ATTACHMENT_UNUSED;
		depthAttachmentReference.layout = VK_IMAGE_LAYOUT_UNDEFINED;
		for(auto attachment : spec.Attachments.Attachments)
		{
			if(!util::IsDepthFormat(attachment.TextureFormat))
			{
				m_ColorSpecifications.push_back(attachment);

				// the renderpass will use this color attachment.
				VkAttachmentDescription color_attachment = {};
				//the attachment will have the format needed by the swapchain
				switch(attachment.TextureFormat)
				{
					case ImageFormat::RGBA8: color_attachment.format = context->GetSwapchain().GetSwapchainImageFormat();
				}
				PK_CORE_ASSERT(color_attachment.format, "Invalid Texture Format!");
				switch(spec.samples)
				{
				case 1: color_attachment.samples = VK_SAMPLE_COUNT_1_BIT; break;
				case 2: color_attachment.samples = VK_SAMPLE_COUNT_2_BIT; break;
				case 4: color_attachment.samples = VK_SAMPLE_COUNT_4_BIT; break;
				case 8: color_attachment.samples = VK_SAMPLE_COUNT_8_BIT; break;
				}
				PK_CORE_ASSERT(color_attachment.samples, "Invalid Sample Count!");
				
				// we Clear when this attachment is loaded
				color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				// we keep the attachment stored when the renderpass ends
				color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				//we don't care about stencil
				color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				//we don't know or care about the starting layout of the attachment
				color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				//after the renderpass ends, the image has to be on a layout ready for display
				color_attachment.finalLayout = util::GetSpecificationLayout(spec.layout);

				VkAttachmentReference color_attachment_ref = {};
				color_attachment_ref.attachment = attachmentDescriptions.size();
				color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				attachmentDescriptions.push_back(color_attachment);
				colorAttachmentReferences.push_back(color_attachment_ref);
			}
			else
			{
				m_DepthSpecification = attachment;

				// the renderpass will use this color attachment.
				VkAttachmentDescription depth_attachment = {};
				//the attachment will have the format needed by the swapchain
				bool hasStencil = false;
				switch (attachment.TextureFormat)
				{
					case ImageFormat::DEPTH24STENCIL8: 
					{
						depth_attachment.format = VK_FORMAT_D24_UNORM_S8_UINT;
						hasStencil = true;
					}
					case ImageFormat::DEPTH32: 
					{
						depth_attachment.format = VK_FORMAT_D32_SFLOAT;
						hasStencil = false;
					}
				}
				PK_CORE_ASSERT(depth_attachment.format, "Invalid Texture Format!");
				switch (spec.samples)
				{
				case 1: depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
				case 2: depth_attachment.samples = VK_SAMPLE_COUNT_2_BIT;
				case 4: depth_attachment.samples = VK_SAMPLE_COUNT_4_BIT;
				case 8: depth_attachment.samples = VK_SAMPLE_COUNT_8_BIT;
				}
				PK_CORE_ASSERT(depth_attachment.samples, "Invalid Sample Count!");

				// we Clear when this attachment is loaded
				depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				// we keep the attachment stored when the renderpass ends
				depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				if(hasStencil)
				{
					depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
				}
				else
				{
					depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				}

				//we don't know or care about the starting layout of the attachment
				depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				//after the renderpass ends, the image has to be on a layout ready for display
				depth_attachment.finalLayout = util::GetSpecificationLayout(spec.layout);


				VkAttachmentReference depthAttachmentReference = {};
				depthAttachmentReference.attachment = attachmentDescriptions.size();
				if (hasStencil)
					depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				else
					depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

				attachmentDescriptions.push_back(depth_attachment);
			}
		}
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = colorAttachmentReferences.size();
		subpass.pColorAttachments = colorAttachmentReferences.data();
		subpass.pDepthStencilAttachment = &depthAttachmentReference;

		VkRenderPassCreateInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = attachmentDescriptions.size();
		render_pass_info.pAttachments = attachmentDescriptions.data();
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;

		PK_CORE_INFO("{0}", attachmentDescriptions.size());

		CHECK_VULKAN(vkCreateRenderPass(device, &render_pass_info, nullptr, &m_RenderPass));
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		
	}
}