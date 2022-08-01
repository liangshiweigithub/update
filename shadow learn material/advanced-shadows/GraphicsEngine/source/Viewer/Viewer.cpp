#include "stdafx.h"
#include "Resource.h"
#include "ViewerApp.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE /*hPrevInstance*/,
					   LPTSTR    /*lpCmdLine*/,
					   int       /*nCmdShow*/)
{
#if defined(DEBUG) || defined(_DEBUG)
	ViewerApp::GetInstance().AllocateConsole();

	// Enable run-time memory check for debug builds.
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	if (!ViewerApp::GetInstance().LoadConfigs(ViewerApp::ConfigFile))
		return S_FALSE;

	if (!ViewerApp::GetInstance().Init())
		return S_FALSE;
	
	if (!ViewerApp::GetInstance().Create(ViewerApp::Title, hInstance, 0, 0))
		return S_FALSE;

	if (!ViewerApp::GetInstance().Initialize())
		return S_FALSE;

	if (!ViewerApp::GetInstance().Run())
		return S_FALSE;

#if defined(DEBUG) || defined(_DEBUG)
	ViewerApp::GetInstance().DeallocateConsole();
#endif

	return ViewerApp::GetInstance().Exit();
}