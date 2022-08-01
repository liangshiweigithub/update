#include "stdafx.h"

#include "D3dRenderSystem.h"

Logger D3dBufferManager::logger("D3dBufferManager");

D3dBufferManager::D3dBufferManager(ID3D11Device& d3dDevice, ID3D11DeviceContext& d3dContext) :
	_d3dDevice(&d3dDevice),
	_d3dContext(&d3dContext),
	_buffers(1, 0),
	_srvTextures(1, 0)
{
}

D3dBufferManager::~D3dBufferManager()
{
	for (tBufferArray::size_type i = 0; i < _buffers.size(); ++i)
		if (_buffers[i])
			_buffers[i]->Release();

	for (tSRVArray::size_type i = 0; i < _srvTextures.size(); ++i)
		if (_srvTextures[i])
			_srvTextures[i]->Release();

	_buffers.clear();
	_srvTextures.clear();
}

bool D3dBufferManager::Generate(Mesh& mesh, const D3dRenderSystem& renderSystem)
{
	HRESULT hr = S_OK;

	for (unsigned int gInput = 0; gInput < mesh.GetGeometryCount(); ++gInput)
	{
		Geometry* geometry = mesh.GetGeometry(gInput);

		unsigned int indexBufferSize = geometry->GetVertexIndicesSize();
		unsigned int bufferSize = mesh.GetVertexArraySize();
		
		Geometry::tIndexArray indexArray = geometry->GetVertexIndices();

		if (bufferSize < mesh.GetNormalArraySize())
		{
			indexArray = geometry->GetNormalIndices();
			bufferSize = mesh.GetNormalArraySize();
		}

		if (bufferSize < mesh.GetTexCoordArraySize())
		{
			indexArray = geometry->GetTexCoordIndices();
			bufferSize = mesh.GetTexCoordArraySize();
		}

		// create indexbuffer
		unsigned int* indices = new unsigned int[indexBufferSize];

		for (unsigned int i = 0; i < indexBufferSize; ++i)
			indices[i] = indexArray[i];

		hr = CreateBuffer(D3D11_USAGE_DEFAULT, sizeof(unsigned int) * indexBufferSize, D3D11_BIND_INDEX_BUFFER, 0, 0, indices);
		
		delete[] indices;

		if (FAILED(hr))
		{
			if (logger.IsEnabled(Logger::Warn))
				Logger::Stream(logger, Logger::Warn) << "Error creating indexbuffer for " << geometry->GetName().c_str() <<
					" (" <<	renderSystem.GetErrorString(hr).c_str() << ")";
			continue;
		}

		geometry->SetIndexDataBindId(_buffers.size() - 1);
		
		// create vertexbuffer
		VertexData* vertices = new VertexData[bufferSize];
		const Mesh::tVertexArray& vArray = mesh.GetVertexArray();
		const Mesh::tNormalArray& nArray = mesh.GetNormalArray();
		const Mesh::tTexCoordArray& tArray = mesh.GetTexCoordArray();

		for (unsigned int i = 0; i < indexBufferSize; ++i)
		{
			vertices[indexArray[i]].position = D3DXVECTOR3(
				vArray[geometry->GetVertexIndices()[i]].x,
				vArray[geometry->GetVertexIndices()[i]].y,
				vArray[geometry->GetVertexIndices()[i]].z);
			
			vertices[indexArray[i]].normal = D3DXVECTOR3(
				nArray[geometry->GetNormalIndices()[i]].x,
				nArray[geometry->GetNormalIndices()[i]].y,
				nArray[geometry->GetNormalIndices()[i]].z);

			if (geometry->GetTexCoordIndicesSize())
			{
				vertices[indexArray[i]].texCoord = D3DXVECTOR2(
					tArray[geometry->GetTexCoordIndices()[i]].x,
					tArray[geometry->GetTexCoordIndices()[i]].y);
			}
			else
				vertices[indexArray[i]].texCoord = D3DXVECTOR2(.0f, .0f);
		}

		hr = CreateBuffer(D3D11_USAGE_DEFAULT, sizeof(VertexData) * bufferSize, D3D11_BIND_VERTEX_BUFFER, 0, 0, vertices);

		delete[] vertices;

		if (FAILED(hr))
		{
			if (logger.IsEnabled(Logger::Warn))
				Logger::Stream(logger, Logger::Warn) << "Error creating vertexbuffer for " << geometry->GetName().c_str() <<
					" (" <<	renderSystem.GetErrorString(hr).c_str() << ")";
			continue;
		}

		geometry->SetVertexDataBindId(_buffers.size() - 1);
		geometry->SetVboDataStride(sizeof(VertexData));
		geometry->SetVertexDataOffset(0);
	}
	return true;
}

void D3dBufferManager::Delete(Mesh& mesh)
{
	for (unsigned int i = 0; i < mesh.GetGeometryCount(); ++i)
	{
		unsigned int iIndex = mesh.GetGeometry(i)->GetIndexDataBindId();

		if (iIndex < _buffers.size() && _buffers[iIndex])
		{
			_buffers[iIndex]->Release();
			_buffers[iIndex] = 0;
		}

		unsigned int vIndex = mesh.GetGeometry(i)->GetVertexDataBindId();

		if (vIndex < _buffers.size() && _buffers[vIndex])
		{
			_buffers[vIndex]->Release();
			_buffers[vIndex] = 0;
		}
	}
}

bool D3dBufferManager::Generate(Image& image, const D3dRenderSystem& renderSystem)
{
	HRESULT hr = S_OK;
	
	D3DX11_IMAGE_INFO imgInfo;
	ZeroMemory( &imgInfo, sizeof(D3DX11_IMAGE_INFO) );
	hr = D3DX11GetImageInfoFromFile(image.GetFilename().c_str(), 0, &imgInfo, 0);

	if (FAILED(hr))
	{
		if (logger.IsEnabled(Logger::Warn))
			Logger::Stream(logger, Logger::Warn) << "Cannot load image info " << image.GetFilename().c_str() <<
				" (" <<	renderSystem.GetErrorString(hr).c_str() << ")";
		return false;
	}

	D3DX11_IMAGE_LOAD_INFO loadInfo;
	ZeroMemory( &loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO) );
	loadInfo.Width = imgInfo.Width;
	loadInfo.Height = imgInfo.Height;
	loadInfo.Depth = imgInfo.Depth;
	loadInfo.FirstMipLevel = 0;
	loadInfo.MipLevels = D3DX11_DEFAULT; // generate full mipmap chain
	loadInfo.Usage = D3D11_USAGE_DEFAULT;
	loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	loadInfo.CpuAccessFlags = 0;
	loadInfo.MiscFlags = 0;
	loadInfo.Format = renderSystem.IsInGammaCorrectMode() ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	loadInfo.Filter = renderSystem.IsInGammaCorrectMode() ? D3DX11_FILTER_SRGB | D3DX11_FILTER_NONE : D3DX11_FILTER_NONE;
	loadInfo.MipFilter = D3DX11_FILTER_TRIANGLE;
	loadInfo.pSrcInfo = &imgInfo;

	ID3D11ShaderResourceView* srvTexture = 0;	
	hr = D3DX11CreateShaderResourceViewFromFile(_d3dDevice, image.GetFilename().c_str(), &loadInfo, 0, &srvTexture, 0);
	
	if (FAILED(hr))
	{
		if (logger.IsEnabled(Logger::Warn))
			Logger::Stream(logger, Logger::Warn) << "Cannot load shader resource view " << image.GetFilename().c_str() <<
				" (" <<	renderSystem.GetErrorString(hr).c_str() << ")";
		return false;
	}
	
	//_d3dDevice->GenerateMips(srvTexture);

	if (logger.IsEnabled(Logger::Info))
		Logger::Stream(logger, Logger::Info) << "Generate texture " << image.GetFilename().c_str();

	_srvTextures.push_back(srvTexture);
	image.SetBindId(_srvTextures.size() - 1);
	
	return true;
}

//bool D3dBufferManager::Generate(Image& image, const D3dRenderSystem& renderSystem)
//{
//	HRESULT hr = S_OK;
//	ID3D11Texture2D* texture = NULL;
//	ID3D11Resource*  pRes   = NULL;
//
//	hr = D3DX11CreateTextureFromFile(_d3dDevice, image.GetFilename().c_str(), NULL, NULL, &pRes, NULL);
//
//	if (FAILED(hr))
//	{
//		if (logger.IsEnabled(Logger::Warn))
//			Logger::Stream(logger, Logger::Warn) << "Cannot create texture from image " << image.GetFilename().c_str() <<
//			" (" <<	renderSystem.GetErrorString(hr).c_str() << ")";
//		return false;
//	}
//
//	// Translates the ID3D11Resource object into a ID3D11Texture2D object
//	pRes->QueryInterface(__uuidof( ID3D11Texture2D ), (LPVOID*)&texture);
//	pRes->Release();
//	if (texture == NULL)
//	{
//		if (logger.IsEnabled(Logger::Error))
//			Logger::Stream(logger, Logger::Error) << "Cannot create texture from image " << image.GetFilename().c_str() <<
//			" (" <<	renderSystem.GetErrorString(hr).c_str() << ")";
//		return false;
//	}
//
//	if (logger.IsEnabled(Logger::Debug))
//	{
//		D3D11_TEXTURE2D_DESC tmp;
//		texture->GetDesc(&tmp);
//		Logger::Stream(logger, Logger::Debug) << "Texture format: " << tmp.Format;
//		Logger::Stream(logger, Logger::Debug) << "Texture miplevels: " << tmp.MipLevels;
//	}
//
//	ID3D11Texture2D* correctedTexture = renderSystem.IsInGammaCorrectMode() ? ConvertTextureToSRGB(texture) : texture;
//
//	if(correctedTexture == NULL)
//		return false;
//
//	// Get the texture details
//	D3D11_TEXTURE2D_DESC desc;
//	correctedTexture->GetDesc(&desc);
//
//	// Create a shader resource view of the texture
//	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
//	ZeroMemory(&srvDesc, sizeof(srvDesc));
//	srvDesc.Format = desc.Format;
//	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MipLevels = desc.MipLevels;
//
//	ID3D11ShaderResourceView* srvTexture = 0;
//	hr=_d3dDevice->CreateShaderResourceView(correctedTexture, &srvDesc, &srvTexture);
//
//	if (FAILED(hr))
//	{
//		if (logger.IsEnabled(Logger::Warn))
//			Logger::Stream(logger, Logger::Warn) << "Cannot create shader resource view " << image.GetFilename().c_str() <<
//			" (" <<	renderSystem.GetErrorString(hr).c_str() << ")";
//		return false;
//	}
//
//	if (logger.IsEnabled(Logger::Info))
//		Logger::Stream(logger, Logger::Info) << "Generate texture " << image.GetFilename().c_str();
//
//	_srvTextures.push_back(srvTexture);
//	image.SetBindId(_srvTextures.size() - 1);
//
//	texture->Release();
//	correctedTexture->Release();
//
//	return true;
//}

void D3dBufferManager::Delete(Image& image)
{
	unsigned int index = image.GetBindId();

	if (index < _srvTextures.size() && _srvTextures[index])
	{
		_srvTextures[index]->Release();
		_srvTextures[index] = 0;
	}
}

bool D3dBufferManager::Generate(D3dRenderTarget& renderTarget, const D3dRenderSystem& renderSystem)
{
	HRESULT hr = renderTarget.Initialize(*_d3dDevice);

	if (hr != S_OK)
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << "Cannot create rendertarget " << renderTarget.GetName().c_str() <<
				" (" << renderSystem.GetErrorString(hr).c_str() << ")";

		renderTarget.ReleaseRessources();
		return false;
	}
	return true;
}

void D3dBufferManager::Delete(D3dRenderTarget& renderTarget)
{
	renderTarget.ReleaseRessources();
}

const D3dBufferManager::tBufferArray& D3dBufferManager::GetBufferArray() const
{
	return _buffers;
}

const D3dBufferManager::tSRVArray& D3dBufferManager::GetSRVTextureArray() const
{
	return _srvTextures;
}

HRESULT D3dBufferManager::CreateBuffer(D3D11_USAGE usage, UINT byteWidth, UINT bindFlags,
									   UINT cpuAccessFlags, UINT miscFlags, const void* data)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* buffer = 0;
	
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage          = usage;
	bufferDesc.ByteWidth      = byteWidth;
	bufferDesc.BindFlags      = bindFlags;
	bufferDesc.CPUAccessFlags = cpuAccessFlags;
	bufferDesc.MiscFlags      = miscFlags;
	
	if (data)
	{
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem		  = data;
		initData.SysMemPitch      = 0;
		initData.SysMemSlicePitch = 0;

		hr = _d3dDevice->CreateBuffer(&bufferDesc, &initData, &buffer);
	}
	else
	{
		hr = _d3dDevice->CreateBuffer(&bufferDesc, 0, &buffer);
	}

	if (FAILED(hr))
		return hr;

	_buffers.push_back(buffer);
	
	return S_OK;
}


