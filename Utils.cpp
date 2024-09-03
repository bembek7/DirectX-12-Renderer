#define _CRT_SECURE_NO_WARNINGS
#include "Utils.h"
#include <cstdlib>

std::string Utils::WstringToString(const std::wstring& ws) noexcept
{
	size_t len = wcstombs(nullptr, ws.c_str(), 0) + 1;

	char* buffer = new char[len];

	wcstombs(buffer, ws.c_str(), len);

	std::string str = std::string(buffer);

	delete[] buffer;
	return str;
}

std::wstring Utils::StringToWstring(const std::string& s) noexcept
{
	size_t len = mbstowcs(nullptr, s.c_str(), 0) + 1;

	wchar_t* buffer = new wchar_t[len];

	mbstowcs(buffer, s.c_str(), len);

	std::wstring ws = std::wstring(buffer);

	delete[] buffer;

	return ws;
}