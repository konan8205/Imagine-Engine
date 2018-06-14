
#include "Render/Vulkan/VulkanCommandBuffer.h"

VulkanCommandBuffer::VulkanCommandBuffer(
	VkDevice* _device,
	VulkanSwapChain* _SwapChainClass,
	VulkanFrameBuffer* _FrameBufferClass,
	VulkanRenderPass* _RenderPassClass,
	VulkanGraphicsPipeline* _GraphicsPipelineClass)
	: device(_device)
	, SwapChainClass(_SwapChainClass)
	, FrameBufferClass(_FrameBufferClass)
	, RenderPassClass(_RenderPassClass)
	, GraphicsPipelineClass(_GraphicsPipelineClass)
	, queueFamilyIndex(UINT32_MAX)
{

}

VulkanCommandBuffer::~VulkanCommandBuffer()
{

}

bool VulkanCommandBuffer::Initialize(VulkanCommandBufferCreateInfo _createInfo)
{
	VkResult result;

	assert(_createInfo.queueFamilyIndex != UINT32_MAX
		&& _createInfo.cmdPoolCount != UINT32_MAX
		&& _createInfo.cmdCount != UINT32_MAX);

	cmdStructList.resize(_createInfo.cmdPoolCount);

	for (uint32_t i = 0; i < _createInfo.cmdPoolCount; ++i) {
		result = CreateCommandPool(i, _createInfo.queueFamilyIndex);
		if (result != VK_SUCCESS) {
			throw runtime_error("Failed to create the command pool");
			--i;
			do {
				DestroyCommandPool(i);
				--i;
			} while (i != 0);
			cmdStructList.clear();
			return false;
		}

		result = CreateCommandBuffer(i, _createInfo.cmdCount);
		if (result != VK_SUCCESS) {
			throw runtime_error("Failed to allocate the command buffer");
			return false;
		}
	}
	queueFamilyIndex = _createInfo.queueFamilyIndex;

	return true;
}

VkResult VulkanCommandBuffer::Update()
{
	VkResult result;

	for (uint32_t i = 0; i < cmdStructList.size(); i++) {
		for (uint32_t j = 0; j < cmdStructList[i].cmdList.size(); j++) {

			VkCommandBufferInheritanceInfo cmdBufInheritInfo = {};
			cmdBufInheritInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			cmdBufInheritInfo.pNext = NULL;
			cmdBufInheritInfo.renderPass = VK_NULL_HANDLE;
			cmdBufInheritInfo.subpass = 0;
			cmdBufInheritInfo.framebuffer = VK_NULL_HANDLE;
			cmdBufInheritInfo.occlusionQueryEnable = VK_FALSE;
			cmdBufInheritInfo.queryFlags = 0;
			cmdBufInheritInfo.pipelineStatistics = 0;

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			beginInfo.pInheritanceInfo = &cmdBufInheritInfo;

			result = vkBeginCommandBuffer(cmdStructList[i].cmdList[j], &beginInfo);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = RenderPassClass->renderPass;
			renderPassInfo.framebuffer = FrameBufferClass->frameBufferList[j];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = SwapChainClass->swapChainExtent;

			VkClearValue* clearValues = new VkClearValue[2];
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };

			renderPassInfo.clearValueCount = 2;
			renderPassInfo.pClearValues = clearValues;

			vkCmdBeginRenderPass(cmdStructList[i].cmdList[j], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(
				cmdStructList[i].cmdList[j],
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				GraphicsPipelineClass->graphicsPipeline);

			//VkBuffer vertexBuffers[] = { vertexBuffer };
			//VkDeviceSize offsets[] = { 0 };
			//vkCmdBindVertexBuffers(cmdStructList[i].cmdList[j], 0, 1, vertexBuffers, offsets);

			//vkCmdBindIndexBuffer(ccmdStructList[i].cmdList[j], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

			//vkCmdBindDescriptorSets(cmdStructList[i].cmdList[j], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

			//vkCmdDrawIndexed(cmdStructList[i].cmdList[j], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

			vkCmdEndRenderPass(cmdStructList[i].cmdList[j]);

			result = vkEndCommandBuffer(cmdStructList[i].cmdList[j]);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Failed to record command buffer!");
				return result;
			}
		}
	}

	return result;
}

VkResult VulkanCommandBuffer::CreateCommandPool(const uint32_t _cmdPoolIndex, const uint32_t _queueFamilyIndex)
{
	assert(_cmdPoolIndex < cmdStructList.size());

	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.pNext = NULL;
	cmdPoolInfo.queueFamilyIndex = _queueFamilyIndex;
	cmdPoolInfo.flags = 0;

	VkResult result;
	result = vkCreateCommandPool(*device, &cmdPoolInfo, NULL, &cmdStructList[_cmdPoolIndex].cmdPool);

	return result;
}

VkResult VulkanCommandBuffer::CreateCommandBuffer(const uint32_t _cmdPoolIndex, const uint32_t _count)
{
	assert(_cmdPoolIndex < cmdStructList.size());

	VkCommandBufferAllocateInfo cmdInfo = {};
	cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdInfo.pNext = NULL;
	cmdInfo.commandPool = cmdStructList[_cmdPoolIndex].cmdPool;
	cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdInfo.commandBufferCount = _count;

	cmdStructList[_cmdPoolIndex].cmdList.resize(_count);

	VkResult result;
	result = vkAllocateCommandBuffers(*device, &cmdInfo, cmdStructList[_cmdPoolIndex].cmdList.data());
	
	return result;
}
