
#include "Render/Vulkan/VulkanDevice.h"

VulkanDevice::VulkanDevice(Vulkan* _VulkanClass, VulkanSurface* _SurfaceClass)
	: VulkanClass(_VulkanClass)
	, SurfaceClass(_SurfaceClass)
	, pDevice(NULL)
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
		VulkanQueueFamilyStruct queueInfo = GetQueueFamilyProperties(&pDeviceList[i]);
		if (queueInfo.graphicsQueueIndex != UINT32_MAX && 
			queueInfo.presentQueueIndex != UINT32_MAX &&
			deviceCreated != true)
		{
			result = CreateDevice(&pDeviceList[i], queueInfo, _createInfo);
			if (result != VK_SUCCESS) {
				throw runtime_error("Failed to create logical device");
				return false;
			}
			queueFamilyStruct = queueInfo;
			pDevice = &(pDeviceList[i]);
			deviceCreated = true;
		}
	}

	return deviceCreated;
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

VulkanQueueFamilyStruct VulkanDevice::GetQueueFamilyProperties(VkPhysicalDevice* _pDevice)
{
	VulkanQueueFamilyStruct queueIndexStructTemp;

	uint32_t queueFamilyPropsCount;
	vkGetPhysicalDeviceQueueFamilyProperties(*_pDevice, &queueFamilyPropsCount, NULL);
	if (queueFamilyPropsCount <= 0) {
		return queueIndexStructTemp;
	}
	queueIndexStructTemp.queueFamilyProps.resize(queueFamilyPropsCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*_pDevice, &queueFamilyPropsCount, queueIndexStructTemp.queueFamilyProps.data());

	for (uint32_t i = 0; i < queueIndexStructTemp.queueFamilyProps.size(); ++i)
	{
		bool validQueue = false;
		if ((queueIndexStructTemp.queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) {
			if (queueIndexStructTemp.graphicsQueueIndex == UINT32_MAX) {
				queueIndexStructTemp.graphicsQueueIndex = i;
			}
			validQueue = true;
		}
		if ((queueIndexStructTemp.queueFamilyProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT) {
			queueIndexStructTemp.computeQueueIndex = i;
			validQueue = true;
		}
		if ((queueIndexStructTemp.queueFamilyProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) {
			queueIndexStructTemp.transferQueueIndex = i;
			validQueue = true;
		}
		if (validQueue) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(*_pDevice, i, SurfaceClass->surface, &presentSupport);
			if (presentSupport) {
				queueIndexStructTemp.presentQueueIndex = i;
			}
		}
		else {
			VkPhysicalDeviceProperties pDeviceProperties;
			vkGetPhysicalDeviceProperties(*_pDevice, &pDeviceProperties);
			printf("[Warning]\tSkipping unnecessary queue family %d: %s\n", i, pDeviceProperties.deviceName);
		}
	}

	return queueIndexStructTemp;
}

VkResult VulkanDevice::CreateDevice(
	VkPhysicalDevice* _pDevice,
	VulkanQueueFamilyStruct _queueFamilyStruct,
	const VulkanDeviceCreateInfo _createInfo)
{
	VkResult result;

	vector<VkDeviceQueueCreateInfo> queueInfoList;
	uint32_t queueFamilyList[] = {
		_queueFamilyStruct.graphicsQueueIndex,
		_queueFamilyStruct.presentQueueIndex
	};
	float queuePriority = 1.0f;
	for (int queueFamily : queueFamilyList) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueInfoList.emplace_back(queueCreateInfo);
	}

	VkDeviceCreateInfo deviceInfo;
	VkDeviceGroupDeviceCreateInfo createInfo;
	if (!_createInfo.bDisableMultiGPURendering && pDeviceList.size() > 1)
	{
		for (VkPhysicalDeviceGroupProperties pDeviceGroup : pDeviceGroupList) {
			for (uint32_t i = 0; i < pDeviceGroup.physicalDeviceCount; ++i) {
				if (pDeviceGroup.physicalDevices[i] == *_pDevice) {
					createInfo.physicalDeviceCount = pDeviceGroup.physicalDeviceCount;
					createInfo.pPhysicalDevices = pDeviceGroup.physicalDevices;
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
	deviceInfo.queueCreateInfoCount = (uint32_t)queueInfoList.size();
	deviceInfo.pQueueCreateInfos = queueInfoList.data();
	deviceInfo.enabledLayerCount = (uint32_t)deviceLayerNames.size();
	deviceInfo.ppEnabledLayerNames = deviceLayerNames.data();
	deviceInfo.enabledExtensionCount = (uint32_t)deviceExtensionNames.size();
	deviceInfo.ppEnabledExtensionNames = deviceExtensionNames.data();
	deviceInfo.pEnabledFeatures = NULL;

	result = vkCreateDevice(*_pDevice, &deviceInfo, NULL, &device);
	if (result != VK_SUCCESS) {
		return result;
	}



	return result;
}