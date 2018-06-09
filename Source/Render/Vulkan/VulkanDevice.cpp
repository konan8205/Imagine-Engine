
#include "Render/Vulkan/VulkanDevice.h"

VulkanDevice::VulkanDevice(Vulkan* _VulkanClass)
	: VulkanClass(_VulkanClass)
{

}

VulkanDevice::~VulkanDevice()
{

}

bool VulkanDevice::Initialize(const VulkanDeviceCreateInfo _createInfo)
{
#ifdef _DEBUG
	deviceExtensionNames = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
#endif

	VkResult result;

	result = EnumeratePhysicalDevice();
	if (result != VK_SUCCESS) {
		throw runtime_error("Failed to enumerate graphics devices");
		return false;
	}
	else if (pDeviceList.size() <= 0) {
		throw runtime_error("Cannot find any graphics devices");
		return false;
	}
	
	result = EnumeratePhysicalDeviceGroup();
	if (result != VK_SUCCESS) {
		throw runtime_error("Failed to enumerate graphics device groups");
		return false;
	}

	bool deviceCreated = false;
	for (uint32_t i = 0; i < pDeviceList.size(); ++i)
	{
		VulkanPhysicalDevice queueInfo;
		queueInfo.pDevice = &pDeviceList[i];
		if (SetupQueueFamilyProperties(queueInfo) && deviceCreated != true)
		{
			result = CreateDevice(pDeviceList[i], queueInfo.graphicsQueueIndex, _createInfo);
			if (result != VK_SUCCESS) {
				throw runtime_error("Failed to create logical device");
				return false;
			}
			pDeviceStruct = queueInfo;
			deviceCreated = true;
		}
	}
	return true;
}

VkResult VulkanDevice::EnumeratePhysicalDevice()
{
	VkResult result;
	uint32_t pDeviceCount;
	result = vkEnumeratePhysicalDevices(VulkanClass->instance, &pDeviceCount, NULL);
	if (result != VK_SUCCESS) {
		return result;
	}

	pDeviceList.resize(pDeviceCount);
	result = vkEnumeratePhysicalDevices(VulkanClass->instance, &pDeviceCount, pDeviceList.data());

	return result;
}

VkResult VulkanDevice::EnumeratePhysicalDeviceGroup()
{
	VkResult result;
	uint32_t pDeviceGroupCount;

	result = vkEnumeratePhysicalDeviceGroups(VulkanClass->instance, &pDeviceGroupCount, NULL);
	if (result != VK_SUCCESS) {
		return result;
	}

	pDeviceGroupList.resize(pDeviceGroupCount);
	result = vkEnumeratePhysicalDeviceGroups(VulkanClass->instance, &pDeviceGroupCount, pDeviceGroupList.data());

	return result;
}

bool VulkanDevice::SetupQueueFamilyProperties(VulkanPhysicalDevice& _pDeviceStruct)
{
	uint32_t queueFamilyPropsCount;
	vkGetPhysicalDeviceQueueFamilyProperties(*_pDeviceStruct.pDevice, &queueFamilyPropsCount, NULL);
	if (queueFamilyPropsCount <= 0) {
		return false;
	}
	_pDeviceStruct.queueFamilyProps.resize(queueFamilyPropsCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*_pDeviceStruct.pDevice, &queueFamilyPropsCount, _pDeviceStruct.queueFamilyProps.data());

	for (uint32_t i = 0; i < _pDeviceStruct.queueFamilyProps.size(); ++i)
	{
		bool validQueue = false;
		if ((_pDeviceStruct.queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) {
			if (_pDeviceStruct.graphicsQueueIndex == UINT32_MAX) {
				_pDeviceStruct.graphicsQueueIndex = i;
			}
			validQueue = true;
		}
		if ((_pDeviceStruct.queueFamilyProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT) {
			_pDeviceStruct.computeQueueIndex = i;
			validQueue = true;
		}
		if ((_pDeviceStruct.queueFamilyProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) {
			_pDeviceStruct.transferQueueIndex = i;
			validQueue = true;
		}
		if (!validQueue) {
			VkPhysicalDeviceProperties pDeviceProperties;
			vkGetPhysicalDeviceProperties(*_pDeviceStruct.pDevice, &pDeviceProperties);
			printf("[Warning]\tSkipping unnecessary queue family %d: %s\n", i, pDeviceProperties.deviceName);
		}
	}

	return (_pDeviceStruct.graphicsQueueIndex != UINT32_MAX);
}

VkResult VulkanDevice::CreateDevice(
	VkPhysicalDevice _pDevice,
	const uint32_t _queueFamilyIndex,
	const VulkanDeviceCreateInfo _createInfo)
{
	VkResult result;

	float queuePriorities[1] = { 0.0 };
	VkDeviceQueueCreateInfo queueInfo;
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = NULL;
	queueInfo.flags = 0;
	queueInfo.queueFamilyIndex = _queueFamilyIndex;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo deviceInfo;
	VkDeviceGroupDeviceCreateInfo createInfo;
	if (!_createInfo.bDisableMultiGPURendering && pDeviceList.size() > 1)
	{
		for (VkPhysicalDeviceGroupProperties iter : pDeviceGroupList) {
			for (uint32_t i = 0; i < iter.physicalDeviceCount; ++i) {
				if (iter.physicalDevices[i] == _pDevice) {
					createInfo.physicalDeviceCount = iter.physicalDeviceCount;
					createInfo.pPhysicalDevices = iter.physicalDevices;
					break;
				}
			}
		}
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO;
		createInfo.pNext = NULL;
		deviceInfo.pNext = &createInfo;
	}
	else {
		deviceInfo.pNext = NULL;
	}
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.flags = 0;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.enabledLayerCount = (uint32_t)deviceLayerNames.size();
	deviceInfo.ppEnabledLayerNames = deviceLayerNames.data();
	deviceInfo.enabledExtensionCount = (uint32_t)deviceExtensionNames.size();
	deviceInfo.ppEnabledExtensionNames = deviceExtensionNames.data();
	deviceInfo.pEnabledFeatures = NULL;

	result = vkCreateDevice(_pDevice, &deviceInfo, NULL, &device);

	return result;
}