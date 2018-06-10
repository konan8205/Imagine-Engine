#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/VulkanSwapchain.h"
#include "Render/Vulkan/VulkanFrameBuffer.h"

class VulkanSwapChain;
class VulkanRenderPass;

class VulkanGraphicsPipeline
{
	/* Variables */
public:
	VulkanSwapChain* SwapChainClass;
	VulkanRenderPass* RenderPassClass;
	VkDevice* device;
	
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	/* Functions */
public:
	VulkanGraphicsPipeline(
		VulkanSwapChain* _SwapChainClass,
		VulkanRenderPass* RenderPassClass,
		VkDevice* _device);
	~VulkanGraphicsPipeline();

	VkShaderModule CreateShaderModule(const vector<char>& _code);
	inline void DestroyShaderModule(VkShaderModule _shaderModule) {
		vkDestroyShaderModule(*device, _shaderModule, NULL);
	}

	bool CreateGraphicsPipeline();
	inline void DestroyGraphicsPipeline() {
		vkDestroyPipeline(*device, graphicsPipeline, NULL);
		vkDestroyPipelineLayout(*device, pipelineLayout, NULL);
	}
};