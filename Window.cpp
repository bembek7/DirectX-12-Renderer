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

bool Window::IsKeyQueueEmpty() const noexcept
{
	return keyPressedEvents.empty();
}

short Window::PopPressedKey() noexcept
{
	assert(!keyPressedEvents.empty());
	const short pressedKey = keyPressedEvents.front();
	keyPressedEvents.pop();
	return pressedKey;
}

bool Window::IsKeyPressed(const short key) const noexcept
{
	return keys[key];
}

Graphics& Window::GetGraphics() noexcept
{
	return *graphics;
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
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}