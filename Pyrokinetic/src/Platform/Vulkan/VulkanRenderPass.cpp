#include "pkpch.h"
#include "VulkanRenderPass.h"
#include "Pyrokinetic/Rendering/Renderer.h"

namespace pk
{
	VulkanRenderPass::VulkanRenderPass(const RenderPassSpecification& spec)
		: m_Spec(spec)
	{
		VulkanContext* context = dynamic_cast<VulkanContext*>(Renderer::GetContext());

		// the renderpass will use this color attachment.
		VkAttachmentDescription color_attachment = {};
		//the attachment will have the format needed by the swapchain
		color_attachment.format = context->GetSwapchain().GetSwapchainImageFormat();
		//1 sample, we won't be doing MSAA
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
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
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	}

	VulkanRenderPass::~VulkanRenderPass()
	{
	}
}