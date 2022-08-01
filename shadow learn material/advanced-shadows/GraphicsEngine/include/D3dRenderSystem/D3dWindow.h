#pragma once

#include "D3dRenderSystemExports.h"

#include "Common/Window.h"

#include "Direct3d/d3dx11.h"

class D3DRENDERSYSTEM_API D3dWindow : public Window
{
public:
	D3dWindow(HINSTANCE hInstance);
	~D3dWindow();

	HWND Create(const char* title, int width, int height, bool fullscreen = false);

	BOOL SwapBuffers();

	ID3D11Device& GetD3dDevice() const;
	ID3D11RenderTargetView& GetRenderTargetView() const;
	IDXGISwapChain& GetSwapChain() const;

private:
	BOOL CreateDeviceAndSwapChain();
	BOOL CreateRenderTargetView();
	void CleanupDevice();

private:
	static Logger logger;

	D3D_DRIVER_TYPE _driverType;
	ID3D11Device* _d3dDevice;
	IDXGISwapChain* _swapChain;
	ID3D11RenderTargetView* _renderTargetView;
	
};