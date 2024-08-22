#pragma once
#include "BetterWindows.h"
#include <optional>
#include <string>
#include <queue>
#include <memory>
#include "Graphics.h"

class Window
{
public:
	Window(const unsigned int clientAreaWidth = 800, const unsigned int clientAreaHeight = 600);
	~Window() = default;
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	static std::optional<int> ProcessMessages();
	void SetWidnowTitle(const std::string& newTitle) noexcept;
	bool IsKeyQueueEmpty() const noexcept;
	short PopPressedKey() noexcept;
	bool IsKeyPressed(const short key) const noexcept;
	Graphics& GetGraphics() noexcept;

private:
	static LRESULT CALLBACK WindowProcBeforeCreation(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WindowProcAfterCreation(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam);

private:
	HWND hWnd;
	std::unique_ptr<Graphics> graphics;
	std::queue<short> keyPressedEvents;
	std::array<bool, 255> keys = { false };
	static constexpr char winClassName[] = "RythmWindow";
};
