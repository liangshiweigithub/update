#include "stdafx.h"
#include "PluginColladaExports.h"
#include "ColladaModelLoader.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE /*hModule*/,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

extern "C" PLUGINCOLLADA_API ModelLoader* Register()
{
	static ColladaModelLoader colladaModelLoader;

	return &colladaModelLoader;
}

extern "C" PLUGINCOLLADA_API void Unregister()
{
}
