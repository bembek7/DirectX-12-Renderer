#include "Window.h"
#include <system_error>
#include <optional>
#include <cassert>
#include <string>
#include <errhandlingapi.h>

Window::Window(const unsigned int clientAreaWidth, const unsigned int clientAreaHeight)
{
	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = 0u;
	windowClass.lpfnWndProc = Window::WindowProcBeforeCreation;
	windowClass.lpszClassName = winClassName;

	RegisterClassEx(&windowClass);

	RECT windowRect{};
	windowRect.left = 100;
	windowRect.right = clientAreaWidth + windowRect.left;
	windowRect.top = 100;
	windowRect.bottom = clientAreaHeight + windowRect.top;
	if (AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE) == 0)
	{
		throw std::runtime_error("Can't establish window rect");
	}

	hWnd = CreateWindowEx(
		0,
		winClassName,
		"Rythm",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		GetModuleHandle(nullptr),
		this
	);

	if (!hWnd)
	{
		throw std::runtime_error(std::system_category().message(GetLastError()));
	}

	RAWINPUTDEVICE rawInputDevice = {};
	rawInputDevice.usUsagePage = 0x01;
	rawInputDevice.usUsage = 0x02;
	if (RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice)) == FALSE)
	{
		throw std::runtime_error("Could not register raw input device");
	}

	graphics = std::make_unique<Graphics>(hWnd, clientAreaWidth, clientAreaHeight);
	ShowWindow(hWnd, SW_SHOW);
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg = { };
	while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return (int)msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

void Window::SetWidnowTitle(const std::string& newTitle) noexcept
{
	SetWindowText(hWnd, newTitle.c_str());
}

bool Window::IsKeyPressed(const short key) const noexcept
{
	return keys[key];
}

std::optional<short> Window::ReadPressedKey() noexcept
{
	if (keyPressedEvents.empty())
	{
		return std::nullopt;
	}
	const short pressedKey = keyPressedEvents.front();
	keyPressedEvents.pop();
	return pressedKey;
}

Graphics& Window::GetGraphics() noexcept
{
	return *graphics;
}

void Window::EnableCursor() noexcept
{
	cursorEnabled = true;
	while (ShowCursor(TRUE) < 0);
	FreeCursor();
}

void Window::DisableCursor() noexcept
{
	cursorEnabled = false;
	while (ShowCursor(FALSE) >= 0);
	LockCursorToClientArea();
}

bool Window::IsCursorEnabled() const noexcept
{
	return cursorEnabled;
}

void Window::EnableRawInput() noexcept
{
	rawInputEnabled = true;
}

void Window::DisableRawInput() noexcept
{
	rawInputEnabled = false;
}

std::optional<std::pair<int, int>> Window::ReadRawDelta() noexcept
{
	if (rawDeltaEvents.empty())
	{
		return std::nullopt;
	}
	const auto rawDelta = rawDeltaEvents.front();
	rawDeltaEvents.pop();
	return rawDelta;
}

void Window::FreeCursor() noexcept
{
	ClipCursor(nullptr);
}

void Window::LockCursorToClientArea() noexcept
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

LRESULT Window::WindowProcBeforeCreation(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
	{
		const CREATESTRUCTW* const csw = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const window = static_cast<Window*>(csw->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::WindowProcAfterCreation));

		assert(window != nullptr);
		return window->HandleMessage(hWnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::WindowProcAfterCreation(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window* const window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	return window->HandleMessage(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::HandleMessage(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KILLFOCUS:
		keys.fill(false);
		break;
	case WM_LBUTTONDOWN:
		if (!keys[VK_LBUTTON])
		{
			keyPressedEvents.push((short)wParam);
		}
		keys[VK_LBUTTON] = true;
		break;
	case WM_KEYDOWN:
		if (!keys[wParam])
		{
			keyPressedEvents.push((short)wParam);
		}
		keys[wParam] = true;
		break;
	case WM_LBUTTONUP:
		keys[VK_LBUTTON] = false;
		break;
	case WM_KEYUP:
		keys[wParam] = false;
		break;
	case WM_INPUT:
	{
		if (rawInputEnabled)
		{
			UINT size = 0u;

			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1)
			{
				break;
			}
			rawBuffer.resize(size);

			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawBuffer.data(), &size, sizeof(RAWINPUTHEADER)) != size)
			{
				break;
			}
			auto& rawInput = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
			const auto mouseMotion = std::make_pair<int, int>(rawInput.data.mouse.lLastX, rawInput.data.mouse.lLastY);
			if (rawInput.header.dwType == RIM_TYPEMOUSE && (mouseMotion.first != 0 || mouseMotion.second != 0))
			{
				rawDeltaEvents.push(mouseMotion);
			}
		}
		break;
	}
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}