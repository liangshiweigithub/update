#include "stdafx.h"

#include "D3dRenderSystem.h"

Logger D3dStateManager::logger("D3dStateManager");

D3dStateManager::D3dStateManager(ID3D11Device& d3dDevice, ID3D11DeviceContext& d3dContext) :
	_d3dDevice(&d3dDevice),
	_d3dContext(&d3dContext),
	_worldMtx(vmath::identity4<float>()),
	_worldMtxStack(0),
	_texMtx(Texture::TypeCOUNT, vmath::identity4<float>()),
	_texMtxStack(Texture::TypeCOUNT),
	_rsStates(2),
	_cullState(RenderSystem::CullBackFace),
	_polygonMode(RenderSystem::Fill)
{
	for (unsigned int i = 0; i < _rsStates.size(); ++i)
		_rsStates[i].resize(3, 0);

	D3DXMatrixIdentity(&_world);
}

D3dStateManager::~D3dStateManager()
{
	for (unsigned int i = 0; i < _rsStates.size(); ++i)
		for (unsigned int j = 0; j < _rsStates[i].size(); ++j)
			if (_rsStates[i][j])
				_rsStates[i][j]->Release();
}

bool D3dStateManager::Initialize(const D3dRenderSystem& renderSystem)
{
	bool success = GenerateRasterizerStates(renderSystem) == S_OK;	

	if (success)
	{
		SetCullState(_cullState);
		SetPolygonMode(_polygonMode);
	}

	return success;
}

void D3dStateManager::Push(const Material& material, D3dEffectManager& effectManager, D3dBufferManager& bufferManager)
{
	effectManager.UniformCallBack(material);

	if (!material.GetTextureCount())
		return;
	
	unsigned int i = 0;
	for (Texture* texture = material.GetTexture(i); texture != 0; texture = material.GetTexture(++i))
	{		
		unsigned int index = static_cast<unsigned int>(texture->GetTextureType());

		_texMtxStack[index].push_back(_texMtx[index]);

		_texMtx[index]  = vmath::transpose(vmath::rotation_matrix(texture->GetRotation(), .0f, .0f, 1.0f)) * _texMtx[index];
		_texMtx[index] *= vmath::scaling_matrix(texture->GetTiling().x, -texture->GetTiling().y /* invert v-coord for D3d */, 1.0f);
		_texMtx[index]  = vmath::transpose(vmath::translation_matrix(texture->GetOffset().x, texture->GetOffset().y, .0f)) * _texMtx[index];

		effectManager.UniformCallBack(*texture,	*bufferManager.GetSRVTextureArray()[texture->GetImage().GetBindId()], &_texMtx[index].elem[0][0]);				
	}		
}

void D3dStateManager::Pop(const Material& material)
{
	if (!material.GetTextureCount())
		return;

	unsigned int i = 0;
	for (Texture* texture = material.GetTexture(i); texture != 0; texture = material.GetTexture(++i))
	{
		unsigned int index = static_cast<unsigned int>(texture->GetTextureType());

		_texMtx[index] = _texMtxStack[index].back();
		_texMtxStack[index].pop_back();
	}
}

void D3dStateManager::Push(const Transform& transform, D3dEffectManager& effectManager)
{
	_worldMtxStack.push_back(_worldMtx);
	_worldMtx = transform.GetMatrix() * _worldMtx;

	effectManager.UniformCallBack(Transform(_worldMtx)); 
}

void D3dStateManager::Pop(const Transform&)
{
	_worldMtx = _worldMtxStack.back();
	_worldMtxStack.pop_back();
}

void D3dStateManager::Push()
{
	_worldMtxStack.push_back(_worldMtx);

	for (unsigned int i = 0; i < Texture::TypeCOUNT; ++i)
		_texMtxStack[i].push_back(_texMtx[i]);
}

void D3dStateManager::Pop()
{
	_worldMtx = _worldMtxStack.back();
	_worldMtxStack.pop_back();

	for (unsigned int i = 0; i < Texture::TypeCOUNT; ++i)
	{
		_texMtx[i] = _texMtxStack[i].back();
		_texMtxStack[i].pop_back();
	}
}

RenderSystem::CullState D3dStateManager::GetCullState() const
{
	return _cullState;
}

void D3dStateManager::SetCullState(RenderSystem::CullState state)
{
	_cullState = state;

	_d3dContext->RSSetState(_rsStates[_polygonMode][_cullState]);
}

RenderSystem::PolygonMode D3dStateManager::GetPolygonMode() const
{
	return _polygonMode;
}

void D3dStateManager::SetPolygonMode(RenderSystem::PolygonMode mode)
{
	_polygonMode = mode;

	_d3dContext->RSSetState(_rsStates[_polygonMode][_cullState]);
}

float* D3dStateManager::GetWorldMatrix()
{
	for (unsigned int i = 0; i < 4; ++i)
		for (unsigned int j = 0; j < 4; ++j)
			_world.m[i][j] = _worldMtx.elem[i][j];

	return &_world.m[0][0];
}

HRESULT D3dStateManager::GenerateRasterizerStates(const D3dRenderSystem& renderSystem)
{
    D3D11_FILL_MODE fill[] =
    {
        D3D11_FILL_SOLID,
        D3D11_FILL_WIREFRAME
    };

    D3D11_CULL_MODE cull[] =
    {
        D3D11_CULL_BACK,
        D3D11_CULL_FRONT,
        D3D11_CULL_NONE
    };

	HRESULT hr = S_OK;

	for (unsigned int i = 0; i < _rsStates.size(); ++i)
    {
		for (unsigned int j = 0; j < _rsStates[i].size(); ++j)
		{
			D3D11_RASTERIZER_DESC rsState;
			rsState.FillMode = fill[i];
			rsState.CullMode = cull[j];
			rsState.FrontCounterClockwise = true;
			rsState.DepthBias = false;
			rsState.DepthBiasClamp = .0f;
			rsState.SlopeScaledDepthBias = .0f;
			rsState.DepthClipEnable = true;
			rsState.ScissorEnable = false;
			rsState.MultisampleEnable = true;
			rsState.AntialiasedLineEnable = true;
			hr = _d3dDevice->CreateRasterizerState( &rsState, &_rsStates[i][j] );

			if (FAILED(hr))
			{
				if (logger.IsEnabled(Logger::Error))
					Logger::Stream(logger, Logger::Error) << "Cannot create rasterizerstate (Index: " <<
						i << "," << j << ")." << " (" << renderSystem.GetErrorString(hr) << ")";
				return hr;
			}
		}
    }

	return hr;
}

