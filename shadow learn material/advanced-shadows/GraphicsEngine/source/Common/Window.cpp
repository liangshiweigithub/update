#include "stdafx.h"

#include "Window.h"

Window::~Window()
{
}

ATOM Window::Register(const char* className, WNDPROC wndProc, HICON icon, HCURSOR cursor)
{
	_windowClass.cbSize = sizeof(WNDCLASSEX);

	_windowClass.style			= CS_HREDRAW | CS_VREDRAW;
	_windowClass.lpfnWndProc	= wndProc;
	_windowClass.cbClsExtra		= 0;
	_windowClass.cbWndExtra		= 0;
	_windowClass.hInstance		= _hInstance;
	_windowClass.hIcon			= icon;
	_windowClass.hCursor		= cursor;
	_windowClass.hbrBackground	= 0;
	_windowClass.lpszMenuName	= 0;
	_windowClass.lpszClassName	= className;
	_windowClass.hIconSm		= icon;

	return ::RegisterClassEx(&_windowClass);
}

HWND Window::CreateWindowHandle(const char *title, int width, int height, bool fullscreen)
{
	if (_hWnd)
		return FALSE;

	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	//DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	DWORD dwStyle = (WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS) & ~(WS_THICKFRAME);

	// Attempt Fullscreen Mode?
	if (fullscreen)
	{
		_fullscreen = fullscreen;

		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= width;
		dmScreenSettings.dmPelsHeight	= height;
		dmScreenSettings.dmBitsPerPel	= 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		
		if (!ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			_fullscreen = false;
	}

	RECT windowRect;
	windowRect.left = 0;
	windowRect.right = width;
	windowRect.top = 0;
	windowRect.bottom = height;

	if (_fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(TRUE);
	}
	else
	{
		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
		windowRect.left = abs(windowRect.left);
		windowRect.top = abs(windowRect.top);
	}

	_hWnd = ::CreateWindowEx(dwExStyle,
		_windowClass.lpszClassName,
		title,
		dwStyle,
		0, 0,
		windowRect.left + windowRect.right,
		windowRect.top + windowRect.bottom,
		0,
		0,
		_hInstance,
		0);

	return _hWnd;
}

BOOL Window::Show(int nCmdShow)
{
	if (!_hWnd)
		return FALSE;

	return ::ShowWindow(_hWnd, nCmdShow);
}

HWND Window::GetWindowHandle() const
{
	return _hWnd;
}