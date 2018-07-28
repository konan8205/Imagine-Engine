
#include "Render/Vulkan/Vulkan.h"

Vulkan::Vulkan()
	: deviceClass(nullptr)
	, cmdClass(nullptr)
	, surfaceClass(nullptr)
	, swapChainClass(nullptr)
	, renderPassClass(nullptr)
	, frameBufferClass(nullptr)
	, graphicsPipelineClass(nullptr)
	, queueClass(nullptr)
	, currentFrame(UINT32_MAX)
	, maxFrame(UINT32_MAX)
	, bDisableMultiThreadRendering(false)
	, bDisableMultiGPURendering(false)
{
	
}

Vulkan::~Vulkan()
{

}

bool Vulkan::Initialize(const VulkanCreateInfo _createInfo)
{
#ifdef _DEBUG
	/* Debug options of vulkan instance */
	if (!IsInstanceLayerSupport()) {
		throw runtime_error("Validation layers requested, but not available");
		return false;
	}

	instanceLayerNames = {
		"VK_LAYER_LUNARG_standard_validation"
	};
#endif

	instanceExtensionNames = {
#ifdef _DEBUG
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
		VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(__ANDROID__)
		VK_USE_PLATFORM_ANDROID_KHR,
#elif defined(__APPLE__)
		VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
#elif defined(__linux__)
		VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
		VK_KHR_XCB_SURFACE_EXTENSION_NAME,
		VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
		VK_EXT_ACQUIRE_XLIB_DISPLAY_EXTENSION_NAME,
#elif defined(_WIN32)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
	};

	option = _createInfo;

	/* Create the vulkan instance */
	if (CreateInstance() != VK_SUCCESS) {
		throw runtime_error("Failed to create vulkan instance");
		return false;
	}

	/* Create surface */
	WindowCreateInfo windowCreateInfo;
	windowCreateInfo.width = 1280;
	windowCreateInfo.height = 720;
	windowCreateInfo.title = option.appName;
	windowCreateInfo.resizable = true;

#if defined(_WIN32)
	surfaceClass = new VulkanSurfaceWindows(&instance);
	if (!surfaceClass->CreateWindowWin32(windowCreateInfo) ||
		!surfaceClass->CreateSurface()) {
		return false;
	}
#endif

	/* Create the logical device */
	VulkanDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.bDisableMultiGPURendering = bDisableMultiGPURendering;
	deviceClass = new VulkanDevice(this, surfaceClass);
	if (!deviceClass->Initialize(deviceCreateInfo)) {
		return false;
	}

	/* Create swap Chain */
	swapChainClass = new VulkanSwapChain(
		deviceClass->pDevice,
		&deviceClass->queueFamilyStruct,
		&deviceClass->device,
		surfaceClass);

	if (!swapChainClass->Initialize()) {
		return false;
	}
	maxFrame = (uint32_t)swapChainClass->swapChainImageList.size() - 1;

	/* Create render pass */
	renderPassClass = new VulkanRenderPass(&deviceClass->device, swapChainClass);
	if (renderPassClass->CreateRenderPass() != VK_SUCCESS) {
		return false;
	}

	/* Create frame buffers */
	frameBufferClass = new VulkanFrameBuffer(&deviceClass->device, swapChainClass, renderPassClass);
	if (frameBufferClass->CreateFrameBuffer() != VK_SUCCESS) {
		return false;
	}

	/* Create and allocate vertex buffers */
	vertexBufferClass = new VulkanVertexBuffer(deviceClass->pDevice, &deviceClass->device);
	if (vertexBufferClass->CreateVertexBuffer(sizeof(VulkanVertex) * vertices.size())) {
		return false;
	}
	if (vertexBufferClass->AllocateMemory(vertices.data(), sizeof(VulkanVertex) * vertices.size())) {
		return false;
	}

	/* Create graphics pipelines */
	graphicsPipelineClass = new VulkanGraphicsPipeline(&deviceClass->device, swapChainClass, renderPassClass, vertexBufferClass);
	if (graphicsPipelineClass->CreateGraphicsPipeline() != VK_SUCCESS) {
		return false;
	}

	/* Allocate command buffers */
	VulkanCommandBufferCreateInfo cmdCreateInfo;
	cmdCreateInfo.queueFamilyIndex = deviceClass->queueFamilyStruct.graphicsQueueIndex;
	cmdCreateInfo.cmdPoolCount = 1;
	cmdCreateInfo.cmdCount = (uint32_t)frameBufferClass->frameBufferList.size();
	cmdClass = new VulkanCommandBuffer(
		&deviceClass->device,
		swapChainClass,
		frameBufferClass,
		renderPassClass,
		graphicsPipelineClass,
		vertexBufferClass);

	if (!cmdClass->Initialize(cmdCreateInfo)) {
		return false;
	}

	queueClass = new VulkanQueue(&deviceClass->queueFamilyStruct, &deviceClass->device, swapChainClass, cmdClass);
	queueClass->GetDeviceQueue();
	currentFrame = 0;

	return true;
}

void Vulkan::DeInitialize()
{
	vkDeviceWaitIdle(deviceClass->device);
	delete queueClass;

	/* Free command buffers */
	cmdClass->DeInitialize();
	delete cmdClass;

	/* Clear graphics pipelines */
	graphicsPipelineClass->DestroyGraphicsPipeline();
	delete graphicsPipelineClass;

	/* Free vertex buffers */
	vertexBufferClass->FreeMemory();
	vertexBufferClass->DestroyVertexBuffer();
	delete vertexBufferClass;

	/* Clear frame buffers */
	frameBufferClass->DestroyFrameBuffer();
	delete frameBufferClass;

	/* Clear render passes */
	renderPassClass->DestroyRenderPass();
	delete renderPassClass;

	/* Clear swap chains */
	swapChainClass->DeInialize();
	delete swapChainClass;

	/* Clear surface */
	surfaceClass->DestroySurface();
#if defined(_WIN32)
	surfaceClass->DestroyWindowWin32();
#endif
	delete surfaceClass;

	/* Clear logical device */
	deviceClass->DeInitialize();
	delete deviceClass;

	/* Destroy instance */
#ifdef _DEBUG
	DestroyDebugReportCallbackEXT(instance, callback, NULL);
#endif
	DestroyInstance();
}

bool Vulkan::Update()
{
	return false;
}

bool Vulkan::Render()
{
	cmdClass->Update(0, currentFrame);

	swapChainClass->AcquireImage(currentFrame);

	queueClass->SubmitQueue(currentFrame);
	queueClass->PresentQueue(currentFrame);

	currentFrame = (currentFrame + 1) % (maxFrame + 1);

	vkQueueWaitIdle(queueClass->presentQueue);
	return false;
}

VkResult Vulkan::CreateInstance()
{
	// Convert app name
	size_t appNameSize = wcslen(option.appName) + 1;
	char* appName = new char[appNameSize];
	wcstombs(appName, option.appName, appNameSize);
	
	// Convert engine name
	size_t engineNameSize = wcslen(CoreInfo::GetEngineName()) + 1;
	char* engineName = new char[engineNameSize];
	wcstombs(engineName, CoreInfo::GetEngineName(), engineNameSize);

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = option.appVersion;
	appInfo.pEngineName = engineName;
	appInfo.engineVersion = CoreInfo::GetEngineVer();
#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
	appInfo.apiVersion = VK_API_VERSION_1_0;
#else
	appInfo.apiVersion = VK_API_VERSION_1_1;
#endif

	VkInstanceCreateInfo instInfo = {};
	instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instInfo.pNext = NULL;
	instInfo.flags = 0;
	instInfo.pApplicationInfo = &appInfo;
	instInfo.enabledLayerCount = (uint32_t)instanceLayerNames.size();
	instInfo.ppEnabledLayerNames = instanceLayerNames.size() ? instanceLayerNames.data() : NULL;
	instInfo.enabledExtensionCount = (uint32_t)instanceExtensionNames.size();
	instInfo.ppEnabledExtensionNames = instanceExtensionNames.data();

	VkResult result = vkCreateInstance(&instInfo, NULL, &instance);

#ifdef _DEBUG
	if (result == VK_SUCCESS) {
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = DebugCallback;
		createInfo.pUserData = this;

		result = CreateDebugReportCallbackEXT(instance, &createInfo, NULL, &callback);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug callback!");
		}
	}
#endif

	return result;
}

#ifdef _DEBUG
bool Vulkan::IsInstanceLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, NULL);

	vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : instanceLayerNames)
	{
		bool layerFound = false;
		for (const VkLayerProperties& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			return false;
		}
	}

	return true;
}

VkResult Vulkan::CreateDebugReportCallbackEXT(
	VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != NULL) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Vulkan::DestroyDebugReportCallbackEXT(
	VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != NULL) {
		func(instance, callback, pAllocator);
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::DebugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData)
{
	printf("[VkLayer]:\t %s\n", msg);

	return VK_FALSE;
}

#endif