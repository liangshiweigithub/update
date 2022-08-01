#pragma once

#include "D3dRenderSystemExports.h"

#include "Direct3d/d3dx11.h"
#include "Direct3d/d3dx10math.h"

class D3dEffectManager;
class D3dBufferManager;
class D3dRenderSystem;

class D3DRENDERSYSTEM_API D3dRenderer
{
public:
	struct VertexData
	{
		VertexData();
		VertexData(const tVec3f& v, const tVec3f& n);
		D3DXVECTOR3 pos;
		D3DXVECTOR3 norm;
	};

	D3dRenderer(ID3D11DeviceContext& d3dContext);
	~D3dRenderer();

	bool Initialize(const D3dRenderSystem& renderSystem);	

	void Draw(const Geometry& geometry, const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager);
	void Draw(const BoundingBox& boundingBox, const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager,
			  unsigned int indexId, unsigned int vertexId, bool wireframe);
	void Draw(const Frustum& frustum, const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager,
			  unsigned int indexId, unsigned int vertexId, bool wireframe);
	void Draw(const PolygonBody& body, const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager,
			  unsigned int indexId, unsigned int vertexId);
	void Draw(const Line& line, const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager,
			  unsigned int indexId, unsigned int vertexId);
	void Draw(const RenderSystem::tLineList& lineList, const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager,
			  unsigned int indexId, unsigned int vertexId);
	void Draw(const RenderSystem::Viewport& viewport, const D3dEffectManager& effectManager);

private:
	typedef std::vector<VertexData> tVArray;
	typedef std::vector<unsigned int> tIArray;

	void Draw(const tIArray& indices, const tVArray& vertices, unsigned int indexId, unsigned int vertexId,
			  const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager,
			  D3D11_PRIMITIVE_TOPOLOGY topology);

private:
	static Logger logger;

	ID3D11DeviceContext* _d3dContext;
};