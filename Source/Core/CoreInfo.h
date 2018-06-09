#pragma once

#include "Core/CoreHeader.h"

namespace CoreInfo
{
	inline const char* GetEngineName() {
		return "Imagine Engine";
	}
	inline const wchar_t* GetEngineNameL() {
		return L"Imagine Engine";
	}
	inline const uint32_t GetEngineVer() {
		return 1;
	}
};