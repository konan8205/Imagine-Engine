#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/VulkanSwapChain.h"

class VulkanSwapChain;

class VulkanRenderPass
{
	/* Variables */
public:
	VulkanSwapChain * SwapChainClass;
	VkDevice* device;

	VkRenderPass renderPass;

	/* Functions */
public:
	VulkanRenderPass(VulkanSwapChain* _SwapChainClass, VkDevice* _device);
	~VulkanRenderPass();

	bool CreateRenderPass();
	inline void DestroyRenderPass() {
		vkDestroyRenderPass(*device, renderPass, NULL);
	}
};

class VulkanFrameBuffer
{
	/* Variables */
public:
	VulkanSwapChain* SwapChainClass;
	VulkanRenderPass* RenderPassClass;
	VkDevice* device;
	
	vector<VkFramebuffer> frameBufferList;

	/* Functions */
public:
	VulkanFrameBuffer(VulkanSwapChain* _SwapChainClass, VulkanRenderPass* RenderPassClass, VkDevice* _device);
	~VulkanFrameBuffer();

	bool CreateFrameBuffer();
	inline void DestroyFrameBuffer() {
		for (VkFramebuffer iter : frameBufferList) {
			vkDestroyFramebuffer(*device, iter, NULL);
		}
	}
};