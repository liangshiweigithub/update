#include "stdafx.h"

#include "DXUTApp.h"

#include <io.h>
#include <fcntl.h>
#include <fstream>

extern bool CALLBACK IsD3DDeviceAcceptable(const CD3D11EnumAdapterInfo*, UINT, const CD3D11EnumDeviceInfo*, DXGI_FORMAT, bool, void*);
extern bool CALLBACK IsD3DDeviceAcceptable(UINT, UINT, D3D_DRIVER_TYPE, DXGI_FORMAT, bool, void*);
extern HRESULT CALLBACK OnD3DCreateDevice(ID3D11Device* d3dDevice, const DXGI_SURFACE_DESC* bufferSurfaceDesc, void*);
extern HRESULT CALLBACK OnD3DResizedSwapChain(ID3D11Device* d3dDevice, IDXGISwapChain* swapChain, const DXGI_SURFACE_DESC* bufferSurfaceDesc, void*);
extern void CALLBACK OnD3DFrameRender(ID3D11Device*, ID3D11DeviceContext*, double, float, void*);
extern LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* noFurtherProcessing, void*);
extern void CALLBACK OnKeyboard(UINT key, bool keyDown, bool altDown, void*);
extern void CALLBACK OnMouse(bool leftDown, bool rightDown, bool middleDown, bool, bool, int wheelDelta, int x, int y, void*);
extern void CALLBACK OnFrameMove(double, float elapsedTime, void*);
extern void CALLBACK OnD3DReleasingSwapChain(void*);
extern void CALLBACK OnD3DDestroyDevice(void*);
extern void CALLBACK OnD3DGUIEvent(UINT eventId, int controlId, CDXUTControl* control, void*);

Logger DXUTApp::logger("DXUTApp");

DXUTApp::DXUTApp() :
	_width(1024),
	_height(768),
	_fullscreen(false),
	_config_fov(60),
	_config_zNear(1),
	_config_zFar(1500),
	_swapChain(0),
	_init(false),
	_gammaCorrect(true)
{
}

DXUTApp::~DXUTApp()
{
}

bool DXUTApp::Init()
{
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackFrameMove( OnFrameMove );
	DXUTSetCallbackD3D11DeviceAcceptable(IsD3DDeviceAcceptable);
	DXUTSetCallbackD3D11DeviceCreated( OnD3DCreateDevice );
	DXUTSetCallbackD3D11SwapChainResized( OnD3DResizedSwapChain );
	DXUTSetCallbackD3D11FrameRender( OnD3DFrameRender );
	DXUTSetCallbackD3D11SwapChainReleasing( OnD3DReleasingSwapChain );
	DXUTSetCallbackD3D11DeviceDestroyed( OnD3DDestroyDevice );

	_settingsDlg.Init(&_dialogResourceManager);

	HRESULT hr = S_OK;

	if ((hr = DXUTInit(true, true, 0, false)) != S_OK)
	{
		PrintErrorMsg(logger, GetDXUTErrorMsg(hr), GetDXUTErrorMsg(hr), L"Init Error");
		return false;
	}
	
	DXUTSetCursorSettings(true, true);

	return true;
}

bool DXUTApp::Create(const std::wstring& title, HINSTANCE instance, int x, int y)
{
	HRESULT hr = S_OK;

	if ((hr = DXUTCreateWindow(title.c_str(), instance, 0, 0, x, y)) != S_OK)
	{
		PrintErrorMsg(logger, GetDXUTErrorMsg(hr), GetDXUTErrorMsg(hr), L"Window Creation Error");
		return false;
	}

	DXUTSetIsInGammaCorrectMode(_gammaCorrect);

	// Default settings
	DXUTDeviceSettings deviceSettings;
	ZeroMemory(&deviceSettings, sizeof(deviceSettings));
	deviceSettings.ver = DXUT_D3D11_DEVICE;
	deviceSettings.MinimumFeatureLevel = D3D_FEATURE_LEVEL_11_0;
		
	deviceSettings.d3d11.DeviceFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	deviceSettings.d3d11.AutoCreateDepthStencil = true;
	deviceSettings.d3d11.AutoDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	deviceSettings.d3d11.DriverType = D3D_DRIVER_TYPE_HARDWARE;

	deviceSettings.d3d11.sd.BufferDesc.Format = _gammaCorrect ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	deviceSettings.d3d11.sd.BufferDesc.Width = _width;
	deviceSettings.d3d11.sd.BufferDesc.Height = _height;
	deviceSettings.d3d11.sd.BufferDesc.RefreshRate.Numerator = 60;
	deviceSettings.d3d11.sd.BufferDesc.RefreshRate.Denominator = 1;
	deviceSettings.d3d11.sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	deviceSettings.d3d11.sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	deviceSettings.d3d11.sd.BufferCount = 1;
	deviceSettings.d3d11.sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	deviceSettings.d3d11.sd.OutputWindow = DXUTGetHWND();
	deviceSettings.d3d11.sd.Windowed = TRUE;
	deviceSettings.d3d11.sd.SampleDesc.Count = 4;
	deviceSettings.d3d11.sd.SampleDesc.Quality = 4;

	#ifdef _DEBUG
		deviceSettings.d3d11.CreateFlags = D3D11_CREATE_DEVICE_DEBUG;
	#endif

	if ((hr = DXUTCreateDeviceFromSettings(&deviceSettings)) != S_OK)
	{
		PrintErrorMsg(logger, GetDXUTErrorMsg(hr), GetDXUTErrorMsg(hr), L"Device Creation Error");
		return false;
	}

	if (_fullscreen)
	{
		DXGI_MODE_DESC md;
		DXUTGetD3D11AdapterDisplayMode(deviceSettings.d3d11.AdapterOrdinal, 0, &md);
		md.Width = _width;
		md.Height = _height;
		DXUTToggleFullScreen();
		DXUTGetDXGISwapChain()->ResizeTarget(&md);
	}

	_init = true;

	return true;
}

bool DXUTApp::Run()
{
	HRESULT hr = S_OK;	

	if ((hr = DXUTMainLoop()) != S_OK)
		PrintErrorMsg(logger, GetDXUTErrorMsg(hr), GetDXUTErrorMsg(hr), L"Runtime Error");

	return hr == S_OK;
}

int DXUTApp::Exit()
{
	return DXUTGetExitCode();
}

bool DXUTApp::LoadConfigs(const std::string& configFile)
{
	int fullscreen = 0;
	int gammacorrect = 0;

	if (!FileIO::LoadValue(configFile, "width", _width) ||
		!FileIO::LoadValue(configFile, "height", _height) ||
		!FileIO::LoadValue(configFile, "fullscreen", fullscreen) ||
		!FileIO::LoadValue(configFile, "fov", _config_fov) ||
		!FileIO::LoadValue(configFile, "zNear", _config_zNear) ||
		!FileIO::LoadValue(configFile, "zFar", _config_zFar) ||
		!FileIO::LoadValue(configFile, "gammacorrect", gammacorrect))
	{
		PrintErrorMsg(logger, L"Error loading configfile.", L"Error loading configfile.", L"Config Error");
		return false;
	}
	
	_fullscreen = fullscreen == 1;
	_gammaCorrect = gammacorrect == 1;
	
	return true;
}

void DXUTApp::Update(float)
{
}

void DXUTApp::Display(float)
{
}

HRESULT DXUTApp::OnCreateDevice(ID3D11Device* d3dDevice, const DXGI_SURFACE_DESC*)
{
	HRESULT hr = S_OK;
	
	ID3D11DeviceContext* context = DXUTGetD3D11DeviceContext();
    V_RETURN(_dialogResourceManager.OnD3D11CreateDevice(d3dDevice, context));
	V_RETURN(_settingsDlg.OnD3D11CreateDevice(d3dDevice));

	_txtHelper = new CDXUTTextHelper(d3dDevice, context, &_dialogResourceManager, 22);

	if (_init)
		if (!Initialize())
			hr = S_FALSE;

	return hr;
}

HRESULT DXUTApp::OnResize(ID3D11Device* d3dDevice, IDXGISwapChain* swapchain, const DXGI_SURFACE_DESC* bufferSurfaceDesc)
{
	HRESULT hr = S_OK;

	_swapChain = swapchain;

	V_RETURN(_dialogResourceManager.OnD3D11ResizedSwapChain(d3dDevice, bufferSurfaceDesc));
	V_RETURN(_settingsDlg.OnD3D11ResizedSwapChain(d3dDevice, bufferSurfaceDesc));

	return hr;
}

void DXUTApp::OnDestroyDevice()
{
	_dialogResourceManager.OnD3D11DestroyDevice();
	_settingsDlg.OnD3D11DestroyDevice();
	DXUTGetGlobalResourceCache().OnDestroyDevice();
	SAFE_DELETE(_txtHelper);
}

void DXUTApp::OnReleasingSwapChain()
{
	_dialogResourceManager.OnD3D11ReleasingSwapChain();
}

bool DXUTApp::AllocateConsole()
{
	AllocConsole();

	*stdout = *_fdopen(_open_osfhandle(reinterpret_cast<long>(GetStdHandle(STD_OUTPUT_HANDLE)), _O_TEXT), "w" );

	setvbuf(stdout, NULL, _IONBF, 0 );

	std::ios::sync_with_stdio();

	return true;
}

bool DXUTApp::DeallocateConsole()
{
	return FreeConsole() != FALSE;
}

const std::wstring DXUTApp::GetDXUTErrorMsg(HRESULT hr) const
{
	std::wstring errorMsg(L"");

	switch (hr)
	{
		case DXUTERR_NODIRECT3D:
			errorMsg.assign(L"Could not initialize Direct3D.");
			break;
		case DXUTERR_NOCOMPATIBLEDEVICES:
			errorMsg.assign(L"Could not find any compatible Direct3D devices.");
			break;
		case DXUTERR_MEDIANOTFOUND:
			errorMsg.assign(L"Could not find required media.");
			break;
		case DXUTERR_NONZEROREFCOUNT:
			errorMsg.assign(L"The Direct3D device has a non-zero reference count, meaning some objects were not released.");
			break;
		case DXUTERR_CREATINGDEVICE:
			errorMsg.assign(L"An error occurred when attempting to create an Direct3D device.");
			break;
		case DXUTERR_RESETTINGDEVICE:
			errorMsg.assign(L"An error occurred when attempting to reset an Direct3D device.");
			break;
		case DXUTERR_CREATINGDEVICEOBJECTS:
			errorMsg.assign(L"An error occurred in the device create callback function.");
			break;
		case DXUTERR_RESETTINGDEVICEOBJECTS:
			errorMsg.assign(L"An error occurred in the device reset callback function.");
			break;
		case DXUTERR_DEVICEREMOVED:
			errorMsg.assign(L"The Direct3D device was removed.");
			break;
		default:
			errorMsg.assign(L"Unknown Error.");
			break;
	}
	return errorMsg;
}

void DXUTApp::PrintErrorMsg(Logger& logger, const std::wstring& loggerMsg, const std::wstring& msgBoxText, const std::wstring& msgBoxCaption) const
{
	if (logger.IsEnabled(Logger::Error))
		Logger::Stream(logger, Logger::Error) << Util::WStrToStr(loggerMsg).c_str();
		
	MessageBox(DXUTGetHWND(), msgBoxText.c_str() , msgBoxCaption.c_str(), MB_OK | MB_ICONERROR);	
}

void DXUTApp::EditSettingDlg()
{
	const WCHAR* types[4] =
	{
		L"D3D10_DRIVER_TYPE_REFERENCE",
		L"D3D10_DRIVER_TYPE_NULL",
		L"D3D10_DRIVER_TYPE_SOFTWARE",
		L"D3D10_DRIVER_TYPE_WARP",
	};

	CDXUTDialog* dialog = _settingsDlg.GetDialogControl();

	CDXUTComboBox* comboBox = dialog->GetComboBox(DXUTSETTINGSDLG_DEVICE_TYPE);
	// remove all device types except hardware device
	for (unsigned int i = 0; i < 4; ++i)
	{
		int index = comboBox->FindItem(types[i]);
		if (index >= 0)
			comboBox->RemoveItem(index);
	}
}

tVec2i DXUTApp::GetMousePos()
{
	POINT p;

	GetCursorPos(&p);
	ScreenToClient(DXUTGetHWND(), &p);

	return tVec2i(p.x, p.y);
}