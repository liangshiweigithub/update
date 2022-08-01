#pragma once

#include "D3dRenderSystemExports.h"

#include "Direct3d/d3d11.h"
#include "Direct3d/d3dx11.h"

class D3dEffectManager;
class D3dBufferManager;
class D3dRenderSystem;

class D3DRENDERSYSTEM_API D3dStateManager
{
public:
	typedef std::vector<tMat4f> tMatrixArray;
	typedef std::vector<tMatrixArray> tMatricesArray;

	D3dStateManager(ID3D11Device& d3dDevice, ID3D11DeviceContext& d3dContext);
	~D3dStateManager();

	bool Initialize(const D3dRenderSystem& renderSystem);

	void Push(const Material& material, D3dEffectManager& effectManager, D3dBufferManager& bufferManager);
	void Pop(const Material& material);

	void Push(const Transform& transform, D3dEffectManager& effectManager);
	void Pop(const Transform& transform);

	void Push();
	void Pop();

	RenderSystem::CullState GetCullState() const;
	void SetCullState(RenderSystem::CullState state);

	RenderSystem::PolygonMode GetPolygonMode() const;
	void SetPolygonMode(RenderSystem::PolygonMode mode);

	float* GetWorldMatrix();

private:
	typedef std::vector< std::vector<ID3D11RasterizerState*> > tRSStateArray;
	
	HRESULT GenerateRasterizerStates(const D3dRenderSystem& renderSystem);
	
private:
	static Logger logger;

	ID3D11Device* _d3dDevice;
	ID3D11DeviceContext* _d3dContext;
	RenderSystem::CullState _cullState;
	RenderSystem::PolygonMode _polygonMode;

#pragma warning(push)
#pragma warning(disable : 4251)
	D3DXMATRIX _world;
	
	tMat4f _worldMtx;
	tMatrixArray _worldMtxStack;

	tMatrixArray _texMtx;
	tMatricesArray _texMtxStack;

	tRSStateArray _rsStates;
#pragma warning(pop)
};
