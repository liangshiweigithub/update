#pragma once

#include "D3dRenderSystemExports.h"

class D3DRENDERSYSTEM_API D3dMSAAMode : public Object
{
public:
	D3dMSAAMode(const std::string& name, unsigned int count, unsigned int quality);

	const DXGI_SAMPLE_DESC& GetSampleDesc() const;

private:
	DXGI_SAMPLE_DESC _sampleDesc;
};
