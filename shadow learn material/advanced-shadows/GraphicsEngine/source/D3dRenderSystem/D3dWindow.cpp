#include "stdafx.h"

#include "D3dWindow.h"

Logger D3dWindow::logger("D3dWindow");

D3dWindow::D3dWindow(HINSTANCE hInstance) :
	_driverType(D3D_DRIVER_TYPE_NULL),
	_d3dDevice(0),
	_swapChain(0),
	_renderTargetView(0)
{
	_hInstance = hInstance;
	_hWnd = 0;
	_fullscreen = false;
}

D3dWindow::~D3dWindow()
{
	if (_fullscreen)
	{
		ChangeDisplaySettings(0, 0);
		ShowCursor(TRUE);
	}

	CleanupDevice();
}

HWND D3dWindow::Create(const char* title, int width, int height, bool fullscreen)
{
	if (!CreateWindowHandle(title, width, height, fullscreen))
		return FALSE;

	if (!CreateDeviceAndSwapChain())
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << "Cannot create Direct3D 10 Device and SwapChain.";
		return FALSE;
	}

	if (!CreateRenderTargetView())
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << "Cannot create Direct3D 10 RenderTargetView.";
		return FALSE;
	}

	ID3D11DeviceContext *pd3dDevice11Context;
	_d3dDevice->GetImmediateContext(&pd3dDevice11Context);
	pd3dDevice11Context->OMSetRenderTargets( 1, &_renderTargetView, 0 );
	
	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	pd3dDevice11Context->RSSetViewports(1, &vp);

	if (logger.IsEnabled(Logger::Info))
		Logger::Stream(logger, Logger::Info) << "Create Direct3D 10 rendering window.";

	return _hWnd;
}

BOOL D3dWindow::CreateDeviceAndSwapChain()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(_hWnd, &rc);
	unsigned int clWidth = rc.right - rc.left;
	unsigned int clHeight = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	unsigned int numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = clWidth;
	sd.BufferDesc.Height = clHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL  FeatureLevelsSupported;

	ID3D11DeviceContext *context;
	_d3dDevice->GetImmediateContext(&context);

	for (unsigned int driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
	{
		_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(0,_driverType,0,createDeviceFlags,NULL,0,D3D11_SDK_VERSION,
			&sd,&_swapChain,&_d3dDevice,&FeatureLevelsSupported,&context);
		
		if (SUCCEEDED(hr))
			break;
	}

	return hr == S_OK;
}

BOOL D3dWindow::CreateRenderTargetView()
{
	HRESULT hr = S_OK;

	ID3D11Texture2D* pBackBuffer;
	hr = _swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	
	if (FAILED(hr))
		return FALSE;

	hr = _d3dDevice->CreateRenderTargetView( pBackBuffer, 0, &_renderTargetView );
	pBackBuffer->Release();

	return hr == S_OK;
}

BOOL D3dWindow::SwapBuffers()
{
	return _swapChain->Present(0, 0) == S_OK;
}

void D3dWindow::CleanupDevice()
{
	if (_d3dDevice)
	{
		ID3D11DeviceContext *pd3dDevice11Context;
		_d3dDevice->GetImmediateContext(&pd3dDevice11Context);
		pd3dDevice11Context->ClearState();
	}

    if (_renderTargetView)
		_renderTargetView->Release();
    
	if (_swapChain )
		_swapChain->Release();

    if (_d3dDevice )
		_d3dDevice->Release();
}

ID3D11Device& D3dWindow::GetD3dDevice() const
{
	return *_d3dDevice;
}

ID3D11RenderTargetView& D3dWindow::GetRenderTargetView() const
{
	return *_renderTargetView;
}

IDXGISwapChain& D3dWindow::GetSwapChain() const
{
	return *_swapChain;
}
