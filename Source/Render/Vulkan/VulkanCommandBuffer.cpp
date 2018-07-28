
#include "Render/Vulkan/VulkanCommandBuffer.h"

VulkanCommandBuffer::VulkanCommandBuffer(
	VkDevice* _device,
	VulkanSwapChain* _SwapChainClass,
	VulkanFrameBuffer* _FrameBufferClass,
	VulkanRenderPass* _RenderPassClass,
	VulkanGraphicsPipeline* _GraphicsPipelineClass,
	VulkanVertexBuffer* _VertexBufferClass)
	: device(_device)
	, SwapChainClass(_SwapChainClass)
	, FrameBufferClass(_FrameBufferClass)
	, RenderPassClass(_RenderPassClass)
	, GraphicsPipelineClass(_GraphicsPipelineClass)
	, VertexBufferClass(_VertexBufferClass)
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

VkResult VulkanCommandBuffer::Update(uint32_t _cmdPoolIndex, uint32_t _cmdIndex)
{
	VkResult result;
	
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	result = vkBeginCommandBuffer(cmdStructList[_cmdPoolIndex].cmdList[_cmdIndex], &beginInfo);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer");
	}

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = RenderPassClass->renderPass;
	renderPassInfo.framebuffer = FrameBufferClass->frameBufferList[_cmdIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = SwapChainClass->swapChainExtent;

	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(cmdStructList[_cmdPoolIndex].cmdList[_cmdIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(
		cmdStructList[_cmdPoolIndex].cmdList[_cmdIndex],
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		GraphicsPipelineClass->graphicsPipeline);

	VkBuffer vertexBuffers[] = { VertexBufferClass->vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdStructList[_cmdPoolIndex].cmdList[_cmdIndex], 0, 1, vertexBuffers, offsets);

	vkCmdDraw(cmdStructList[_cmdPoolIndex].cmdList[_cmdIndex], 3, 1, 0, 0);

	vkCmdEndRenderPass(cmdStructList[_cmdPoolIndex].cmdList[_cmdIndex]);

	result = vkEndCommandBuffer(cmdStructList[_cmdPoolIndex].cmdList[_cmdIndex]);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
		return result;
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
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

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
