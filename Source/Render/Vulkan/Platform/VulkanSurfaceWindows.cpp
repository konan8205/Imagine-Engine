#ifdef _WIN32

#include "Render/Vulkan/Platform/VulkanSurfaceWindows.h"

VulkanSurfaceWindows::VulkanSurfaceWindows(VkInstance* _instance)
	: VulkanSurface(_instance)
{
	
}

VulkanSurfaceWindows::~VulkanSurfaceWindows()
{

}

bool VulkanSurfaceWindows::Render()
{
	MSG msg;
	PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	if (msg.message == WM_QUIT) {
		return false;
	}
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	RedrawWindow(hWnd, NULL, NULL, RDW_INTERNALPAINT);
	return true;
}

bool VulkanSurfaceWindows::CreateSurface()
{
	VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.hinstance = hInstance;
	surfaceInfo.hwnd = hWnd;

	if (vkCreateWin32SurfaceKHR(*instance, &surfaceInfo, NULL, &surface) != VK_SUCCESS) {
		throw runtime_error("Failed to create window surface");
		return false;
	}

	return true;
}

bool VulkanSurfaceWindows::CreateWindowWin32(const WindowCreateInfo& _createInfo)
{
	assert(_createInfo.width != INT_MAX && _createInfo.height != INT_MAX);

	width = _createInfo.width;
	height = _createInfo.height;

	int x = _createInfo.x;
	int y = _createInfo.y;

	if (x == INT_MAX || y == INT_MAX) {
		x = 0;
		y = 0;
	}

	WNDCLASSEX winInfo;
	memset(&winInfo, 0, sizeof(WNDCLASSEX));
	winInfo.cbSize = sizeof(WNDCLASSEX);
	winInfo.style = CS_HREDRAW | CS_VREDRAW;
	winInfo.lpfnWndProc = WndProc;
	winInfo.cbClsExtra = 0;
	winInfo.cbWndExtra = 0;
	winInfo.hInstance = hInstance;
	winInfo.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winInfo.hCursor = LoadCursor(NULL, IDC_ARROW);
	winInfo.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winInfo.lpszMenuName = NULL;
	winInfo.lpszClassName = _createInfo.title;
	winInfo.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&winInfo)) {
		return false;
	}

	RECT wr = { 0, 0, width, height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindowEx(0,
		CoreInfo::GetEngineName(),
		_createInfo.title,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU,
		x, y,
		wr.right - wr.left, wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!hWnd) {
		throw runtime_error("Failed to create window");
		return false;
	}
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);

	return true;
}

bool VulkanSurfaceWindows::SetWindowAttributeWin32(const WindowCreateInfo& _createInfoo) {
	return false;
}

LRESULT CALLBACK VulkanSurfaceWindows::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	VulkanSurfaceWindows* SurfaceClass =
		static_cast<VulkanSurfaceWindows*>((VulkanSurfaceWindows*)GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		return 0;

	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) {
			// SetWindowLongPtr will be called after WM_SIZE called as first
			if (SurfaceClass != NULL) {
				SurfaceClass->width = lParam & 0xffff;
				SurfaceClass->height = (lParam & 0xffff0000) >> 16;
			}
		}
		break;

	default:
		break;
	}

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}
#endif