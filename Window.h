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
	bool IsKeyPressed(const short key) const noexcept;
	std::optional<short> ReadPressedKey() noexcept;
	Graphics& GetGraphics() noexcept;
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool IsCursorEnabled() const noexcept;
	void EnableRawInput() noexcept;
	void DisableRawInput() noexcept;
	std::optional<std::pair<int, int>> ReadRawDelta() noexcept;

private:
	void FreeCursor() noexcept;
	void LockCursorToClientArea() noexcept;
	static LRESULT CALLBACK WindowProcBeforeCreation(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WindowProcAfterCreation(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam);

private:
	HWND hWnd;
	std::unique_ptr<Graphics> graphics;
	std::queue<short> keyPressedEvents;
	std::queue<std::pair<int, int>> rawDeltaEvents;
	std::array<bool, 255> keys = { false };
	std::vector<BYTE> rawBuffer;
	bool cursorEnabled = true;
	bool rawInputEnabled = false;
	static constexpr char winClassName[] = "RythmWindow";
};
