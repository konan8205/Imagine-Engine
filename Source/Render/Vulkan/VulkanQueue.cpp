
#include "Render/Vulkan/VulkanQueue.h"

VulkanQueue::VulkanQueue(
	VulkanQueueFamilyStruct* _queueFamilyStruct,
	VkDevice* _device,
	VulkanSwapChain* _SwapChainClass,
	VulkanCommandBuffer* _CmdClass)
	: device(_device)
	, queueFamilyStruct(_queueFamilyStruct)
	, SwapChainClass(_SwapChainClass)
	, CmdClass(_CmdClass)
{

}

VulkanQueue::~VulkanQueue()
{

}

void VulkanQueue::GetDeviceQueue()
{
	vkGetDeviceQueue(*device, queueFamilyStruct->graphicsQueueIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(*device, queueFamilyStruct->presentQueueIndex, 0, &presentQueue);
}

VkResult VulkanQueue::SubmitQueue(const uint32_t _imageIndex)
{
	vkQueueWaitIdle(graphicsQueue);
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore waitSemaphores[] = { SwapChainClass->imageAvailableSemaphoreList[_imageIndex] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkCommandBuffer* cmdList = new VkCommandBuffer[CmdClass->cmdStructList.size()];
	for (uint32_t i = 0; i < CmdClass->cmdStructList.size(); ++i) {
		cmdList[i] = CmdClass->cmdStructList[i].cmdList[_imageIndex];
	}

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = (uint32_t)CmdClass->cmdStructList.size();
	submitInfo.pCommandBuffers = cmdList;

	VkSemaphore signalSemaphores[] = { SwapChainClass->renderFinishedSemaphoreList[_imageIndex] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

	if (result != VK_SUCCESS) {
#ifdef _DEBUG
		
#endif
		throw std::runtime_error("Failed to submit draw command buffer");
	}

	return result;
}

VkResult VulkanQueue::PresentQueue(const uint32_t _imageIndex)
{
	vkQueueWaitIdle(presentQueue);
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	VkSemaphore signalSemaphores[] = { SwapChainClass->renderFinishedSemaphoreList[_imageIndex] };
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { SwapChainClass->swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &_imageIndex;

	VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swapchain");
	}

	return result;
}