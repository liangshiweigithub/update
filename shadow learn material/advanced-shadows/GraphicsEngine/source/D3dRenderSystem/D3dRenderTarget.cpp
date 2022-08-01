#include "stdafx.h"

#include "D3dRenderSystem.h"

/* D3dRenderTarget *********************************************************************/
D3dRenderTarget::D3dRenderTarget() :
	_tex(0),
	_dst(0),
	_srv(0),
	_rtv(0),
	_dsv(0),
	_rtSize(0, 0),
	_arraySize(0),
	_mipmapped(false),
	_multisampled(false),
	_msaaMode("", 1, 0),
	_tempRTV(0),
	_tempDSV(0),
	_clrColor(.0f, .0f, .0f, 1.0f),
	_clrDepth(1.0f),
	_clrStencil(0),
	_nbrOfTargets(1)
{
}

D3dRenderTarget::~D3dRenderTarget()
{
	ReleaseRessources();
}

const tVec2u& D3dRenderTarget::GetSize() const
{
	return _rtSize;
}

void D3dRenderTarget::SetSize(const tVec2u& size)
{
	_rtSize = size;
}

unsigned int D3dRenderTarget::GetArraySize() const
{
	return _arraySize;
}

void D3dRenderTarget::SetArraySize(unsigned int arraySize)
{
	_arraySize = arraySize;
}

bool D3dRenderTarget::GetMipMapped() const
{
	return _mipmapped;
}

void D3dRenderTarget::SetMipMapped(bool enable)
{
	_mipmapped = enable;
}

const D3dMSAAMode& D3dRenderTarget::GetMSAAMode() const
{
	return _msaaMode;
}

void D3dRenderTarget::SetMSAAMode(const D3dMSAAMode& mode)
{
	_msaaMode = mode;
	_multisampled = true;
}

void D3dRenderTarget::SetClearColor(const tVec4f& color)
{
	_clrColor = D3DXVECTOR4(color.x, color.y, color.z, color.w);
}

void D3dRenderTarget::SetClearDepth(float depth)
{
	_clrDepth = depth;
}

void D3dRenderTarget::SetClearStencil(unsigned int stencil)
{
	_clrStencil = static_cast<UINT8>(stencil);
}

unsigned int D3dRenderTarget::GetFormat(unsigned int target) const
{
	if (!_tex[target])
		return 0;

	D3D11_TEXTURE2D_DESC desc;
	_tex[target]->GetDesc(&desc);

	return static_cast<unsigned int>(desc.Format);
}

unsigned int D3dRenderTarget::GetMipLevels() const
{
	return _texDesc.MipLevels;
}

void D3dRenderTarget::Bind(D3dRenderSystem& renderSystem)
{
	renderSystem.GetD3dContext()->OMGetRenderTargets(1, &_tempRTV, &_tempDSV);
	renderSystem.GetD3dContext()->OMSetRenderTargets(0, NULL, NULL);	
	renderSystem.GetD3dContext()->OMSetRenderTargets(_rtv ? _nbrOfTargets : 0, _rtv, _dsv);	
}

void D3dRenderTarget::UnBind(D3dRenderSystem& renderSystem)
{
	renderSystem.GetD3dContext()->OMSetRenderTargets(1, &_tempRTV, _tempDSV);

	_tempRTV->Release();
	_tempDSV->Release();
}

void D3dRenderTarget::Bind(D3dRenderSystem&, ID3DX11EffectShaderResourceVariable& variable, unsigned int target) const
{
	if (_srv[target])
		variable.SetResource(_srv[target]);
}

void D3dRenderTarget::UnBind(D3dRenderSystem& renderSystem, ID3DX11EffectShaderResourceVariable& variable) const
{
	variable.SetResource(0);	
	renderSystem.GetEffectManager()->GetActiveD3dPass()->Apply(0,renderSystem.GetD3dContext());
}

void D3dRenderTarget::Clear(D3dRenderSystem& renderSystem)
{
	if (_rtv)
	{
		for (unsigned int i = 0; i < _nbrOfTargets; ++i)
		{
			if (_rtv[i])
				renderSystem.GetD3dContext()->ClearRenderTargetView(_rtv[i], _clrColor);
		}
	}

	if (_dsv)
		renderSystem.GetD3dContext()->ClearDepthStencilView(_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, _clrDepth, _clrStencil);
}

ID3D11Texture2D* D3dRenderTarget::GetTexture2D(unsigned int target) const
{
	return _tex[target];
}

ID3D11ShaderResourceView* D3dRenderTarget::GetShaderResourceView(unsigned int target) const
{
	return _srv[target];
}

ID3D11RenderTargetView* D3dRenderTarget::GetRenderTargetView(unsigned int target) const
{
	return _rtv[target];
}

ID3D11DepthStencilView* D3dRenderTarget::GetDepthStencilView() const
{
	return _dsv;
}

HRESULT D3dRenderTarget::CreateTex2D(unsigned int bindflags, unsigned int miscflags, D3D11_USAGE usage,
									 unsigned int cpuAccess, ID3D11Device& d3dDevice, DXGI_FORMAT format)
{
	HRESULT hr = S_OK;

	for (unsigned int i = 0; i < _nbrOfTargets; ++i)
	{
		_texDesc.ArraySize          = _arraySize;
		_texDesc.Format				= format;
		_texDesc.Width              = _rtSize.x;
		_texDesc.Height             = _rtSize.y;
		_texDesc.BindFlags			= bindflags;
		_texDesc.MipLevels          = _mipmapped ? 0 : 1;
		_texDesc.MiscFlags          = _mipmapped ? D3D11_RESOURCE_MISC_GENERATE_MIPS | miscflags : miscflags;
		_texDesc.SampleDesc.Count   = _multisampled ? _msaaMode.GetSampleDesc().Count : 1;
		_texDesc.SampleDesc.Quality = _multisampled ? _msaaMode.GetSampleDesc().Quality : 0;
		_texDesc.Usage              = usage;
		_texDesc.CPUAccessFlags     = cpuAccess;

		hr = d3dDevice.CreateTexture2D(&_texDesc, 0, &_tex[i]);

		if (FAILED(hr))
			return hr;
	}

	return hr;
}

HRESULT D3dRenderTarget::CreateDSV(ID3D11Device& d3dDevice, DXGI_FORMAT format)
{
	_dsvDesc.Format = format;
	_dsvDesc.Flags  = 0;

	if (_arraySize > 1)
	{
		_dsvDesc.ViewDimension = _multisampled ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		_dsvDesc.Texture2DArray.ArraySize       = _arraySize;
		_dsvDesc.Texture2DArray.FirstArraySlice = 0;
		_dsvDesc.Texture2DArray.MipSlice        = 0;
	}
	else
	{
		_dsvDesc.ViewDimension = _multisampled ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		_dsvDesc.Texture2D.MipSlice = 0;
	}
	
	return d3dDevice.CreateDepthStencilView(_tex[0], &_dsvDesc, &_dsv);
}

HRESULT D3dRenderTarget::CreateRTV(ID3D11Device& d3dDevice, DXGI_FORMAT format)
{
	HRESULT hr = S_OK;

	for (unsigned int i = 0; i < _nbrOfTargets; ++i)
	{
		_rtvDesc.Format = format;

		if (_arraySize > 1)
		{
			_rtvDesc.ViewDimension = _multisampled ? D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			_rtvDesc.Texture2DArray.ArraySize       = _arraySize;
			_rtvDesc.Texture2DArray.FirstArraySlice = 0;
			_rtvDesc.Texture2DArray.MipSlice        = 0;
		}
		else
		{
			_rtvDesc.ViewDimension = _multisampled ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
			_rtvDesc.Texture2D.MipSlice = 0;
		}

		hr = d3dDevice.CreateRenderTargetView(_tex[i], &_rtvDesc, &_rtv[i]);

		if (FAILED(hr))
			return hr;
	}

	return hr;
}

HRESULT D3dRenderTarget::CreateSRV(ID3D11Device& d3dDevice, DXGI_FORMAT format)
{
	HRESULT hr = S_OK;

	for (unsigned int i = 0; i < _nbrOfTargets; ++i)
	{
		_srvDesc.Format = format;

		if (_arraySize > 1)
		{
			_srvDesc.ViewDimension = _multisampled ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			_srvDesc.Texture2DArray.ArraySize       = _arraySize;
			_srvDesc.Texture2DArray.FirstArraySlice = 0;
			_srvDesc.Texture2DArray.MostDetailedMip = 0;
			_srvDesc.Texture2DArray.MipLevels		= _texDesc.MipLevels;
		}
		else
		{
			_srvDesc.ViewDimension = _multisampled ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			_srvDesc.Texture2D.MostDetailedMip = 0;
			_srvDesc.Texture2D.MipLevels	   = _mipmapped ? _texDesc.MipLevels - 1 : _texDesc.MipLevels;
		}
	
		hr = d3dDevice.CreateShaderResourceView(_tex[i], &_srvDesc, &_srv[i]);

		if (FAILED(hr))
			return hr;
	}

	return hr;
}

HRESULT D3dRenderTarget::CreateCommonDSV(ID3D11Device& d3dDevice, DXGI_FORMAT format)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width              = _rtSize.x;
	texDesc.Height             = _rtSize.y;
	texDesc.MipLevels          = 1;
	texDesc.ArraySize          = 1;
	texDesc.Format             = format;
	texDesc.SampleDesc.Count   = _multisampled ? _msaaMode.GetSampleDesc().Count : 1;
	texDesc.SampleDesc.Quality = _multisampled ? _msaaMode.GetSampleDesc().Quality : 0;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;

	HRESULT hr = d3dDevice.CreateTexture2D(&texDesc, 0, &_dst);

	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format              = format;
	dsvDesc.ViewDimension       = _multisampled ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice  = 0;
	dsvDesc.Flags				= 0;

	return d3dDevice.CreateDepthStencilView(_dst, &dsvDesc, &_dsv);
}

void D3dRenderTarget::ReleaseRessources()
{
	for (unsigned int i = 0; i < _nbrOfTargets; ++i)
	{
		if (_rtv)
			if (_rtv[i])
			{
				_rtv[i]->Release();
				_rtv[i] = 0;
			}
		if (_srv) 
			if (_srv[i])
			{
				_srv[i]->Release();
				_srv[i] = 0;
			}
		if (_tex)
			if (_tex[i])
			{
				_tex[i]->Release();
				_tex[i] = 0;
			}
	}

	if (_dsv)
	{
		_dsv->Release();
		_dsv = 0;
	}

	if (_dst)
	{
		_dst->Release();
		_dst = 0;
	}

	delete[] _rtv; _rtv = 0;
	delete[] _srv; _srv = 0;
	delete[] _tex; _tex = 0;
}

/* D3dFloatDepthRT *********************************************************************/
HRESULT D3dFloatDepthRT::Initialize(ID3D11Device& d3dDevice)
{
	if (!_rtSize.x || !_rtSize.y || !_arraySize)
		return S_FALSE;

	_nbrOfTargets = 1;
	_tex = new ID3D11Texture2D*[_nbrOfTargets];
	_srv = new ID3D11ShaderResourceView*[_nbrOfTargets];

	HRESULT hr = S_OK;

	hr = CreateTex2D(D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_USAGE_DEFAULT,
		0, d3dDevice, DXGI_FORMAT_R32_TYPELESS);

	if (FAILED(hr))
		return hr;

	hr = CreateDSV(d3dDevice, DXGI_FORMAT_D32_FLOAT);

	if (FAILED(hr))
		return hr;

	hr = CreateSRV(d3dDevice, DXGI_FORMAT_R32_FLOAT);

	return hr;
}

/* D3dRGBAFloatRT **********************************************************************/
HRESULT D3dRGBAFloatRT::Initialize(ID3D11Device& d3dDevice)
{
	if (!_rtSize.x || !_rtSize.y || !_arraySize)
		return S_FALSE;

	_nbrOfTargets = 1;
	_tex = new ID3D11Texture2D*[_nbrOfTargets];
	_rtv = new ID3D11RenderTargetView*[_nbrOfTargets];
	_srv = new ID3D11ShaderResourceView*[_nbrOfTargets];

	HRESULT hr = S_OK;

	DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	hr = CreateTex2D(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_USAGE_DEFAULT, 0, d3dDevice, format);

	if (FAILED(hr))
		return hr;

	hr = CreateCommonDSV(d3dDevice);

	if (FAILED(hr))
		return hr;

	hr = CreateRTV(d3dDevice, format);

	if (FAILED(hr))
		return hr;

	hr = CreateSRV(d3dDevice, format);

	return hr;
}

/* D3dRGBAFloat2RT *********************************************************************/
HRESULT D3dRGBAFloat2RT::Initialize(ID3D11Device& d3dDevice)
{
	if (!_rtSize.x || !_rtSize.y || !_arraySize)
		return S_FALSE;

	_nbrOfTargets = 2;
	_tex = new ID3D11Texture2D*[_nbrOfTargets];
	_rtv = new ID3D11RenderTargetView*[_nbrOfTargets];
	_srv = new ID3D11ShaderResourceView*[_nbrOfTargets];

	HRESULT hr = S_OK;

	DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	hr = CreateTex2D(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_USAGE_DEFAULT, 0, d3dDevice, format);

	if (FAILED(hr))
		return hr;

	hr = CreateCommonDSV(d3dDevice);

	if (FAILED(hr))
		return hr;

	hr = CreateRTV(d3dDevice, format);

	if (FAILED(hr))
		return hr;

	hr = CreateSRV(d3dDevice, format);

	return hr;
}

/* D3dRGBAUintRT ***********************************************************************/
HRESULT D3dRGBAUintRT::Initialize(ID3D11Device& d3dDevice)
{
	if (!_rtSize.x || !_rtSize.y || !_arraySize)
		return S_FALSE;

	_nbrOfTargets = 1;
	_tex = new ID3D11Texture2D*[_nbrOfTargets];
	_rtv = new ID3D11RenderTargetView*[_nbrOfTargets];
	_srv = new ID3D11ShaderResourceView*[_nbrOfTargets];

	HRESULT hr = S_OK;

	DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_UINT;

	hr = CreateTex2D(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_USAGE_DEFAULT, 0, d3dDevice, format);

	if (FAILED(hr))
		return hr;

	hr = CreateCommonDSV(d3dDevice);

	if (FAILED(hr))
		return hr;

	hr = CreateRTV(d3dDevice, format);

	if (FAILED(hr))
		return hr;

	hr = CreateSRV(d3dDevice, format);

	return hr;
}

/* D3dRGFloatRT ************************************************************************/
HRESULT D3dRGFloatRT::Initialize(ID3D11Device& d3dDevice)
{
	if (!_rtSize.x || !_rtSize.y || !_arraySize)
		return S_FALSE;

	_nbrOfTargets = 1;
	_tex = new ID3D11Texture2D*[_nbrOfTargets];
	_rtv = new ID3D11RenderTargetView*[_nbrOfTargets];
	_srv = new ID3D11ShaderResourceView*[_nbrOfTargets];
	
	HRESULT hr = S_OK;

	DXGI_FORMAT format = DXGI_FORMAT_R32G32_FLOAT;

	hr = CreateTex2D(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_USAGE_DEFAULT, 0, d3dDevice, format);

	if (FAILED(hr))
		return hr;

	hr = CreateCommonDSV(d3dDevice);

	if (FAILED(hr))
		return hr;

	hr = CreateRTV(d3dDevice, format);

	if (FAILED(hr))
		return hr;

	hr = CreateSRV(d3dDevice, format);

	return hr;
}

/* D3dRGFloatRT ************************************************************************/
HRESULT D3dFloatRT::Initialize(ID3D11Device& d3dDevice)
{
	if (!_rtSize.x || !_rtSize.y || !_arraySize)
		return S_FALSE;

	_nbrOfTargets = 1;
	_tex = new ID3D11Texture2D*[_nbrOfTargets];
	_rtv = new ID3D11RenderTargetView*[_nbrOfTargets];
	_srv = new ID3D11ShaderResourceView*[_nbrOfTargets];
	
	HRESULT hr = S_OK;

	DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT;

	hr = CreateTex2D(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_USAGE_DEFAULT, 0, d3dDevice, format);

	if (FAILED(hr))
		return hr;

	hr = CreateCommonDSV(d3dDevice, DXGI_FORMAT_D32_FLOAT);

	if (FAILED(hr))
		return hr;

	hr = CreateRTV(d3dDevice, format);

	if (FAILED(hr))
		return hr;

	hr = CreateSRV(d3dDevice, format);

	return hr;
}

/* D3dRGFloatRT ************************************************************************/
HRESULT D3dRGUintRT::Initialize(ID3D11Device& d3dDevice)
{
	if (!_rtSize.x || !_rtSize.y || !_arraySize)
		return S_FALSE;

	_nbrOfTargets = 1;
	_tex = new ID3D11Texture2D*[_nbrOfTargets];
	_rtv = new ID3D11RenderTargetView*[_nbrOfTargets];
	_srv = new ID3D11ShaderResourceView*[_nbrOfTargets];
	
	HRESULT hr = S_OK;

	DXGI_FORMAT format = DXGI_FORMAT_R32G32_UINT;

	hr = CreateTex2D(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_USAGE_DEFAULT, 0, d3dDevice, format);

	if (FAILED(hr))
		return hr;

	hr = CreateCommonDSV(d3dDevice);

	if (FAILED(hr))
		return hr;

	hr = CreateRTV(d3dDevice, format);

	if (FAILED(hr))
		return hr;

	hr = CreateSRV(d3dDevice, format);

	return hr;
}

/* D3dRGFloatCPURT *********************************************************************/
HRESULT D3dRGFloatCPURT::Initialize(ID3D11Device& d3dDevice)
{
	if (!_rtSize.x || !_rtSize.y || !_arraySize)
		return S_FALSE;

	_nbrOfTargets = 1;
	_tex = new ID3D11Texture2D*[_nbrOfTargets];

	HRESULT hr = S_OK;

	DXGI_FORMAT format = DXGI_FORMAT_R32G32_FLOAT;

	hr = CreateTex2D(0, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ, d3dDevice, format);

	return hr;
}

/* D3dRGBAFloatCPURT *******************************************************************/
HRESULT D3dRGBAFloatCPURT::Initialize(ID3D11Device& d3dDevice)
{
	if (!_rtSize.x || !_rtSize.y || !_arraySize)
		return S_FALSE;

	_nbrOfTargets = 1;
	_tex = new ID3D11Texture2D*[_nbrOfTargets];

	HRESULT hr = S_OK;

	DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	hr = CreateTex2D(0, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ, d3dDevice, format);

	return hr;
}



