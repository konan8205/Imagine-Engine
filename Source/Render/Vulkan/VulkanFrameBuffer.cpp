
#include "Render/Vulkan/VulkanFrameBuffer.h"

VulkanFrameBuffer::VulkanFrameBuffer(
	VkDevice* _device,
	VulkanSwapChain* _SwapChainClass,
	VulkanRenderPass* _RenderPassClass)
	: device(_device)
	, SwapChainClass(_SwapChainClass)
	, RenderPassClass(_RenderPassClass)
{

}

VulkanFrameBuffer::~VulkanFrameBuffer()
{

}

VkResult VulkanFrameBuffer::CreateFrameBuffer()
{
	VkResult result;

	frameBufferList.resize(SwapChainClass->swapChainImageViewList.size());;
	for (size_t i = 0; i < SwapChainClass->swapChainImageViewList.size(); i++)
	{
		VkImageView attachments[] = {
			SwapChainClass->swapChainImageViewList[i]
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
			throw runtime_error("Failed to create framebuffer");
			return result;
		}
	}

	return result;
}