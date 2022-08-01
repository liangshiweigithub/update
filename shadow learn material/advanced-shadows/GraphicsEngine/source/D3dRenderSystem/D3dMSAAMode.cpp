#include "stdafx.h"

#include "D3dMSAAMode.h"

D3dMSAAMode::D3dMSAAMode(const std::string& name, unsigned int count, unsigned int quality)
{
	SetName(name);
	_sampleDesc.Count = count;
	_sampleDesc.Quality = quality;
}

const DXGI_SAMPLE_DESC& D3dMSAAMode::GetSampleDesc() const
{
	return _sampleDesc;
}
