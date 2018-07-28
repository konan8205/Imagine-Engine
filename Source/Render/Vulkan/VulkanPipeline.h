#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/VulkanSwapChain.h"
#include "Render/Vulkan/VulkanRenderPass.h"
#include "Render/Vulkan/VulkanVertexBuffer.h"
#include "Render/Vulkan/VulkanVertexBuffer.h"

class VulkanSwapChain;
class VulkanRenderPass;
class VulkanVertexBuffer;

class VulkanGraphicsPipeline
{
	/* Variables */
public:
	VkDevice* device;
	VulkanSwapChain* SwapChainClass;
	VulkanRenderPass* RenderPassClass;
	VulkanVertexBuffer* VertexBufferClass;
	
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	/* Functions */
public:
	VulkanGraphicsPipeline(
		VkDevice* _device,
		VulkanSwapChain* _SwapChainClass,
		VulkanRenderPass* RenderPassClass,
		VulkanVertexBuffer* _VertexBufferClass);
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