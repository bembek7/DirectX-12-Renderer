#pragma once
#include "BetterWindows.h"
#include <d3d11.h>

class Gui
{
public:
	Gui(const HWND& hWnd, ID3D11Device* const device, ID3D11DeviceContext* const context);
	~Gui();
	void BeginFrame();
	void EndFrame();
	Gui(const Gui&) = delete;
	Gui& operator=(const Gui&) = delete;
};
