#pragma once

#include "targetver.h"

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <iostream>

// TODO: reference additional headers your program requires here

#ifndef DXUT_AUTOLIB
#define DXUT_AUTOLIB
#endif

#pragma warning(push)
#pragma warning(disable : 4201)
#include "DXUT/Core/DXUT.h"
#pragma warning(pop)

#include "Direct3D/d3d11.h"
#include "Direct3D/d3dx11.h"

#include "CommonSdk.h"
#include "SceneGraphSdk.h"
#include "GraphicsEngineSdk.h"
#include "D3dRenderSystemSdk.h"