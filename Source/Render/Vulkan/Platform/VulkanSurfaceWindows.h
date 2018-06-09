#ifdef _WIN32
#pragma once

#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/Vulkan.h"

#include <windows.h>

class Vulkan;

struct VulkanSurfaceCreateInfo {
	int width;
	int height;
};

class VulkanSurface
{
	/* Variables */
public:
	// Parent class
	Vulkan* VulkanClass;
	VkInstance* instance;

	VkSurfaceKHR surface;
	int width;
	int height;

private:
	HINSTANCE hInstance;
	HWND hWnd;

	/* Functions */
public:
	VulkanSurface(Vulkan* _VulkanClass);
	~VulkanSurface();

	bool Initialize(VulkanSurfaceCreateInfo _createInfo);
	inline void DeInitialize() {
		DestroySurface();
		DestroyWindow(hWnd);
	}
	bool Render();
	
private:
	bool CreateSurface();
	void DestroySurface() {
		vkDestroySurfaceKHR(*instance, surface, NULL);
	}

	bool CreateWindowWin32(const int& width, const int& height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
#endif