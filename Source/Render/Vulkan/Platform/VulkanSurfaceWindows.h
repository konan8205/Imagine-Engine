#pragma once

#ifdef _WIN32
#include "Render/Vulkan/VulkanHeader.h"
#include "Render/Vulkan/Vulkan.h"

#include "Render/Vulkan/Platform/VulkanSurface.h"

#include <windows.h>
#ifndef UNICODE
#error UNICODE must be defined
#endif

class Vulkan;
class VulkanSurface;

struct WindowCreateInfo;
enum class VulkanSurfaceType;

class VulkanSurfaceWindows : public VulkanSurface
{
	/* Variables */
private:
	HINSTANCE hInstance;
	HWND hWnd;

	/* Functions */
public:
	VulkanSurfaceWindows(VkInstance* _instance);
	~VulkanSurfaceWindows();

	bool CreateWindowWin32(const WindowCreateInfo& _createInfo);
	inline void DestroyWindowWin32() {
		DestroyWindow(hWnd);
	}
	bool SetWindowAttributeWin32(const WindowCreateInfo& _createInfo);
	bool CreateSurface();
	bool Render();

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
#endif