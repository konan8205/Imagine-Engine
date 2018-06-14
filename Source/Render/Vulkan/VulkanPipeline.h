#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/VulkanSwapchain.h"
#include "Render/Vulkan/VulkanRenderPass.h"

class VulkanSwapChain;
class VulkanRenderPass;

class VulkanGraphicsPipeline
{
	/* Variables */
public:
	VkDevice* device;
	VulkanSwapChain* SwapChainClass;
	VulkanRenderPass* RenderPassClass;
	
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	/* Functions */
public:
	VulkanGraphicsPipeline(
		VkDevice* _device,
		VulkanSwapChain* _SwapChainClass,
		VulkanRenderPass* RenderPassClass);
	~VulkanGraphicsPipeline();

	VkShaderModule CreateShaderModule(const vector<char>& _code);
	inline void DestroyShaderModule(VkShaderModule _shaderModule) {
		vkDestroyShaderModule(*device, _shaderModule, NULL);
	}

	VkResult CreateGraphicsPipeline();
	inline void DestroyGraphicsPipeline() {
		vkDestroyPipeline(*device, graphicsPipeline, NULL);
		vkDestroyPipelineLayout(*device, pipelineLayout, NULL);
	}
};