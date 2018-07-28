#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/VulkanSwapChain.h"

class VulkanSwapChain;

class VulkanRenderPass
{
	/* Variables */
public:
	VkDevice * device;
	VulkanSwapChain * SwapChainClass;

	VkRenderPass renderPass;

	/* Functions */
public:
	VulkanRenderPass(VkDevice* _device, VulkanSwapChain* _SwapChainClass);
	~VulkanRenderPass();

	VkResult CreateRenderPass();
	inline void DestroyRenderPass() {
		vkDestroyRenderPass(*device, renderPass, NULL);
	}
};