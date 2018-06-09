
#include "Render/Vulkan/VulkanFrameBuffer.h"

VulkanRenderPass::VulkanRenderPass(VulkanSwapChain* _SwapChainClass, VkDevice* _device)
	: SwapChainClass(_SwapChainClass)
	, device(_device)
{

};

VulkanRenderPass::~VulkanRenderPass()
{

};

bool VulkanRenderPass::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = SwapChainClass->swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkResult result;
	result = vkCreateRenderPass(*device, &renderPassInfo, NULL, &renderPass);
	if (result != VK_SUCCESS) {
		throw runtime_error("Failed to create render pass");
		return false;
	}

	return true;
}

VulkanFrameBuffer::VulkanFrameBuffer(
	VulkanSwapChain* _SwapChainClass,
	VulkanRenderPass* _RenderPassClass,
	VkDevice* _device)
	: SwapChainClass(_SwapChainClass)
	, RenderPassClass(_RenderPassClass)
	, device(_device)
{

}

VulkanFrameBuffer::~VulkanFrameBuffer()
{

}

bool VulkanFrameBuffer::CreateFrameBuffer() {
	VkResult result;

	frameBufferList.resize(SwapChainClass->swapChainImageViews.size());;
	for (size_t i = 0; i < SwapChainClass->swapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = {
			SwapChainClass->swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = RenderPassClass->renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = SwapChainClass->swapChainExtent.width;
		framebufferInfo.height = SwapChainClass->swapChainExtent.height;
		framebufferInfo.layers = 1;
		result = vkCreateFramebuffer(*device, &framebufferInfo, NULL, &frameBufferList[i]);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer");
			return false;
		}
	}

	return true;
}