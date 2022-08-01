#pragma once

#include <windows.h>
#include "CommonExports.h"

class COMMON_API Window
{
public:
	enum Type
	{
		OpenGL = 0,
		Direct3d,
		TypeCOUNT
	};

	virtual ~Window();

	virtual HWND Create(const char* title, int width, int height, bool fullscreen = false) = 0;

	virtual BOOL SwapBuffers() = 0;

	ATOM Register(const char* className, WNDPROC wndProc, HICON icon, HCURSOR cursor);

	BOOL Show(int nCmdShow);

	HWND GetWindowHandle() const;

protected:
	HWND CreateWindowHandle(const char *title, int width, int height, bool fullscreen);

protected:
	HINSTANCE _hInstance;
	WNDCLASSEX _windowClass;
	HWND _hWnd;
	bool _fullscreen;
};