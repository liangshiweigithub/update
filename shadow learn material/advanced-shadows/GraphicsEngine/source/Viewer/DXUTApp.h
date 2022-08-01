#pragma once

#include "DXUT/Core/DXUT.h"
#include "DXUT/Opt/SDKmisc.h"

#pragma warning(push)
#pragma warning(disable : 4100)
#include "DXUT/Opt/DXUTgui.h"
#include "DXUT/Opt/DXUTsettingsDlg.h"
#pragma warning(pop)

class DXUTApp
{
public:
	DXUTApp();
	virtual ~DXUTApp();

	virtual bool Init();
	bool Create(const std::wstring& title, HINSTANCE instance, int x, int y);
	bool Run();
	int Exit();
	bool LoadConfigs(const std::string& configFile);

	virtual bool Initialize() = 0;

	virtual void Update(float elapsedTime);
	virtual void Display(float elapsedTime);

	bool AllocateConsole();
	bool DeallocateConsole();

	virtual HRESULT OnCreateDevice(ID3D11Device* d3dDevice, const DXGI_SURFACE_DESC* bufferSurfaceDesc);
	virtual HRESULT OnResize(ID3D11Device* d3dDevice, IDXGISwapChain* swapchain, const DXGI_SURFACE_DESC* bufferSurfaceDesc);
	virtual void OnDestroyDevice();
	virtual void OnReleasingSwapChain();

	static tVec2i GetMousePos();

protected:
	const std::wstring GetDXUTErrorMsg(HRESULT hr) const;
	void PrintErrorMsg(Logger& logger, const std::wstring& loggerMsg, const std::wstring& msgBoxText, const std::wstring& msgBoxCaption) const;
	virtual void EditSettingDlg();

protected:
	IDXGISwapChain* _swapChain;
	CDXUTDialogResourceManager _dialogResourceManager;
	CD3DSettingsDlg _settingsDlg;
	CDXUTTextHelper* _txtHelper;
	bool _init;
	int _width;
	int _height;
	int _config_zNear;
	int _config_zFar;
	int _config_fov;
	bool _fullscreen;
	bool _gammaCorrect;

private:
	static Logger logger;
};