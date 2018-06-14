#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/VulkanSwapChain.h"
#include "Render/Vulkan/VulkanRenderPass.h"

class VulkanSwapChain;
class VulkanRenderPass;

class VulkanFrameBuffer
{
	/* Variables */
public:
	VkDevice* device;
	VulkanSwapChain* SwapChainClass;
	VulkanRenderPass* RenderPassClass;
	
	vector<VkFramebuffer> frameBufferList;

	/* Functions */
public:
	VulkanFrameBuffer(VkDevice* _device, VulkanSwapChain* _SwapChainClass, VulkanRenderPass* RenderPassClass);
	~VulkanFrameBuffer();

	VkResult CreateFrameBuffer();
	inline void DestroyFrameBuffer() {
		for (VkFramebuffer iter : frameBufferList) {
			vkDestroyFramebuffer(*device, iter, NULL);
		}
	}
};