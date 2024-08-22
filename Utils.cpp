#define _CRT_SECURE_NO_WARNINGS
#include "Utils.h"
#include <cstdlib>

std::string WstringToString(const std::wstring& ws) noexcept
{
	size_t len = wcstombs(nullptr, ws.c_str(), 0) + 1;

	char* buffer = new char[len];

	wcstombs(buffer, ws.c_str(), len);

	std::string str = std::string(buffer);

	delete[] buffer;
	return str;
}