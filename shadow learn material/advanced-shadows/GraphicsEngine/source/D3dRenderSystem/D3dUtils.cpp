#include "stdafx.h"

#include "D3dUtils.h"

void D3dUtils::ConvertToD3d(const tMat4f& in, D3DXMATRIX& out)
{
	for (unsigned int i = 0; i < 4; ++i)
		for (unsigned int j = 0; j < 4; ++j)
			out.m[i][j] = in.elem[j][i];
}

void D3dUtils::ConvertToLib(const D3DXMATRIX& in, tMat4f& out)
{
	for (unsigned int i = 0; i < 4; ++i)
		for (unsigned int j = 0; j < 4; ++j)
			out.elem[i][j] = in.m[j][i];
}

bool D3dUtils::IsInside(int x, int y, unsigned int rectX, unsigned int rectY, unsigned int rectWth, unsigned int rectHgt)
{
	return static_cast<unsigned int>((rectX + rectWth) - x) < rectWth &&
		   static_cast<unsigned int>((rectY + rectHgt) - y) < rectHgt;
}