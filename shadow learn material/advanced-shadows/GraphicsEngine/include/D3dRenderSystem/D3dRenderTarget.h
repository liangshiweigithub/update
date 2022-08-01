#pragma once

#include "D3dRenderSystemExports.h"

#include "Direct3d/d3d11.h"
#include "Direct3d/d3dx11.h"

#include "D3dMSAAMode.h"

#include "Effects11/d3dx11effect.h"

#include "Direct3d/D3DX10math.h"

class D3dRenderSystem;

class D3DRENDERSYSTEM_API D3dRenderTarget : public Object
{
friend class D3dBufferManager;
public:
	D3dRenderTarget();
	virtual ~D3dRenderTarget();

	const tVec2u& GetSize() const;
	void SetSize(const tVec2u& size);

	unsigned int GetArraySize() const;
	void SetArraySize(unsigned int arraySize);

	bool GetMipMapped() const;
	void SetMipMapped(bool enable);

	const D3dMSAAMode& GetMSAAMode() const;
	void SetMSAAMode(const D3dMSAAMode& mode);

	void SetClearColor(const tVec4f& color);
	void SetClearDepth(float depth);
	void SetClearStencil(unsigned int stencil);

	unsigned int GetFormat(unsigned int target = 0) const;
	unsigned int GetMipLevels() const;

	void Bind(D3dRenderSystem& renderSystem);
	void UnBind(D3dRenderSystem& renderSystem);

	void Bind(D3dRenderSystem& renderSystem, ID3DX11EffectShaderResourceVariable& variable, unsigned int target = 0) const;
	void UnBind(D3dRenderSystem& renderSystem, ID3DX11EffectShaderResourceVariable& variable) const;

	void Clear(D3dRenderSystem& renderSystem);

	ID3D11Texture2D* GetTexture2D(unsigned int target = 0) const;
	ID3D11ShaderResourceView* GetShaderResourceView(unsigned int target = 0) const;
	ID3D11RenderTargetView* GetRenderTargetView(unsigned int target = 0) const;
	ID3D11DepthStencilView* GetDepthStencilView() const;

protected:
	HRESULT CreateTex2D(unsigned int bindflags, unsigned int miscflags, D3D11_USAGE usage,
		unsigned int cpuAccess, ID3D11Device& d3dDevice, DXGI_FORMAT format);
	HRESULT CreateDSV(ID3D11Device& d3dDevice, DXGI_FORMAT format);
	HRESULT CreateRTV(ID3D11Device& d3dDevice, DXGI_FORMAT format);
	HRESULT CreateSRV(ID3D11Device& d3dDevice, DXGI_FORMAT format);
	HRESULT CreateCommonDSV(ID3D11Device& d3dDevice, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);

private:
	virtual HRESULT Initialize(ID3D11Device& d3dDevice) = 0;
	void ReleaseRessources();

protected:
	D3D11_TEXTURE2D_DESC _texDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC _dsvDesc;
	D3D11_RENDER_TARGET_VIEW_DESC _rtvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC _srvDesc;
	ID3D11Texture2D** _tex;
	ID3D11Texture2D* _dst;
	ID3D11ShaderResourceView** _srv;
	ID3D11RenderTargetView** _rtv;
	ID3D11DepthStencilView* _dsv;

	ID3D11RenderTargetView* _tempRTV;
	ID3D11DepthStencilView* _tempDSV;

	D3dMSAAMode _msaaMode;
	unsigned int _arraySize;
	bool _multisampled;
	bool _mipmapped;
	unsigned int _nbrOfTargets;

	float _clrDepth;
	UINT8 _clrStencil;

#pragma warning(push)
#pragma warning(disable : 4251)
	tVec2u _rtSize;
	D3DXVECTOR4 _clrColor;
#pragma warning(pop)
};

class D3DRENDERSYSTEM_API D3dFloatDepthRT : public D3dRenderTarget
{
private:
	HRESULT Initialize(ID3D11Device& d3dDevice);	
};

class D3DRENDERSYSTEM_API D3dRGBAFloatRT : public D3dRenderTarget
{
private:
	HRESULT Initialize(ID3D11Device& d3dDevice);
};

class D3DRENDERSYSTEM_API D3dRGBAFloat2RT : public D3dRenderTarget
{
private:
	HRESULT Initialize(ID3D11Device& d3dDevice);
};

class D3DRENDERSYSTEM_API D3dRGBAUintRT : public D3dRenderTarget
{
private:
	HRESULT Initialize(ID3D11Device& d3dDevice);
};

class D3DRENDERSYSTEM_API D3dRGFloatRT : public D3dRenderTarget
{
private:
	HRESULT Initialize(ID3D11Device& d3dDevice);
};

class D3DRENDERSYSTEM_API D3dFloatRT : public D3dRenderTarget
{
private:
	HRESULT Initialize(ID3D11Device& d3dDevice);
};

class D3DRENDERSYSTEM_API D3dRGUintRT : public D3dRenderTarget
{
private:
	HRESULT Initialize(ID3D11Device& d3dDevice);
};

class D3DRENDERSYSTEM_API D3dRGFloatCPURT : public D3dRenderTarget
{
private:
	HRESULT Initialize(ID3D11Device& d3dDevice);
};

class D3DRENDERSYSTEM_API D3dRGBAFloatCPURT : public D3dRenderTarget
{
private:
	HRESULT Initialize(ID3D11Device& d3dDevice);
};