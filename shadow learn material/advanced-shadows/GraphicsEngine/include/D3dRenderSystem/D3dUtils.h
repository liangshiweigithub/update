#pragma once

#include "D3dRenderSystemExports.h"

#include "Direct3d/D3DX10math.h"

class D3DRENDERSYSTEM_API D3dUtils
{
public:
	static void ConvertToD3d(const tMat4f& in, D3DXMATRIX& out);
	static void ConvertToLib(const D3DXMATRIX& in, tMat4f& out);
	static bool IsInside(int x, int y, unsigned int rectX, unsigned int rectY, unsigned int rectWth, unsigned int rectHgt);
};