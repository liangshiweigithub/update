#pragma once

#include "D3dRenderSystemExports.h"

#include "Direct3d/d3dx11.h"

class D3dRenderTarget;
class D3dRenderSystem;

class D3DRENDERSYSTEM_API D3dBufferManager
{
friend class D3dRenderSystem;

public:
	typedef std::vector<ID3D11Buffer*> tBufferArray;
	typedef std::vector<ID3D11ShaderResourceView*> tSRVArray;

	D3dBufferManager(ID3D11Device& d3dDevice, ID3D11DeviceContext& d3dContext);
	~D3dBufferManager();

	bool Generate(Mesh& mesh, const D3dRenderSystem& renderSystem);
	void Delete(Mesh& mesh);
	
	bool Generate(Image& image, const D3dRenderSystem& renderSystem);
	void Delete(Image& image);

	bool Generate(D3dRenderTarget& renderTarget, const D3dRenderSystem& renderSystem);
	void Delete(D3dRenderTarget& renderTarget);

	const tBufferArray& GetBufferArray() const;
	const tSRVArray& GetSRVTextureArray() const;

private:
	struct VertexData
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 texCoord;
	};

	HRESULT CreateBuffer(D3D11_USAGE usage, UINT byteWidth, UINT bindFlags, UINT cpuAccessFlags, UINT miscFlags, const void* data);
	ID3D11Texture2D* ConvertTextureToSRGB(ID3D11Texture2D* srcTexture);

private:
	static Logger logger;

	ID3D11Device* _d3dDevice;
	ID3D11DeviceContext* _d3dContext;

#pragma warning(push)
#pragma warning(disable : 4251)
	tBufferArray _buffers;
	tSRVArray _srvTextures;
#pragma warning(pop)
};