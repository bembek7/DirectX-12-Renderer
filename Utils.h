#pragma once

#include <string>

namespace Utils
{
	std::string WstringToString(const std::wstring& ws) noexcept;

	std::wstring StringToWstring(const std::string& s) noexcept;
}
