#pragma once

#include "Core/CoreHeader.h"

#include <fstream>

namespace FileManager
{
	inline static std::vector<char> ReadBinary(const std::string& filename)
	{
		vector<char> buffer;
		
		ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			char* errorMessage = new char[256];
			snprintf(errorMessage, 256, "Failed to open file %s", filename.data());
			throw std::runtime_error(errorMessage);
		}
		else {
			size_t fileSize = (size_t)file.tellg();
			buffer.resize(fileSize);

			file.seekg(0);
			file.read(buffer.data(), fileSize);

			file.close();
		}

		return buffer;
	}
};
