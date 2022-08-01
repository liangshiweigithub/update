#include "stdafx.h"

#include "D3dRenderSystem.h"

/* D3dRenderer::VertexData *************************************************************/
D3dRenderer::VertexData::VertexData()
{
}

D3dRenderer::VertexData::VertexData(const tVec3f& v, const tVec3f& n) :
	pos(v.x, v.y, v.z),
	norm(n.x, n.y, n.z)
{
}

/* D3dRenderer *************************************************************************/
Logger D3dRenderer::logger("D3dRenderer");

D3dRenderer::D3dRenderer(ID3D11DeviceContext& d3dContext) :
	_d3dContext(&d3dContext)
{
}

D3dRenderer::~D3dRenderer()
{
}

bool D3dRenderer::Initialize(const D3dRenderSystem&)
{
	return true;
}

void D3dRenderer::Draw(const Geometry& geometry, const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager)
{
	_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_d3dContext->IASetInputLayout(effectManager.GetActiveD3dInputLayout());

	UINT stride = static_cast<UINT>(geometry.GetVboDataStride());
	UINT offset = static_cast<UINT>(geometry.GetVertexDataOffset());

	const D3dBufferManager::tBufferArray& buffers = bufferManager.GetBufferArray();

	_d3dContext->IASetVertexBuffers(0, 1, &buffers[geometry.GetVertexDataBindId()], &stride, &offset);
	_d3dContext->IASetIndexBuffer(buffers[geometry.GetIndexDataBindId()], DXGI_FORMAT_R32_UINT, offset);
	
	effectManager.GetActiveD3dPass()->Apply(0,_d3dContext);
	_d3dContext->DrawIndexed(geometry.GetVertexIndicesSize(), 0, 0);
}

void D3dRenderer::Draw(const BoundingBox& boundingBox, const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager,
					   unsigned int indexId, unsigned int vertexId, bool wireframe)
{
	unsigned int planes[24] =
	{
		0, 1, 2, 3, // front
		0, 3, 4, 7, // left
		4, 6, 5, 7, // back
		6, 2, 5, 1, // right
		3, 2, 6, 7, // top
		0, 5, 1, 4  // bottom
	};

	tVArray vertices(24);	
	unsigned int j = 0;
	for (unsigned int i = 0; i < 24;)
	{
		const tVec3f first(boundingBox.GetCorner(static_cast<BoundingBox::Corner>(planes[i++])));
		const tVec3f second(boundingBox.GetCorner(static_cast<BoundingBox::Corner>(planes[i++])));
		const tVec3f third(boundingBox.GetCorner(static_cast<BoundingBox::Corner>(planes[i++])));
		const tVec3f fourth(boundingBox.GetCorner(static_cast<BoundingBox::Corner>(planes[i++])));
		
		Plane plane(first, third, second, Polygon3d::ccw);
		const tVec3f& normal = plane.GetNormal();

		vertices[j++] = VertexData(first, normal);
		vertices[j++] = VertexData(second, normal);
		vertices[j++] = VertexData(third, normal);
		vertices[j++] = VertexData(fourth, normal);
	}

	tIArray indices;

	if (wireframe)
	{
		unsigned int iArray[48] =
		{
			0, 1, 1, 2, 2, 3, 3, 0,
			4, 5, 5, 7, 6, 7, 4, 6,
			9, 11, 9, 10, 8, 10, 11, 8,
			12, 13, 13, 15, 15, 14, 14, 12,
			16, 17, 17, 18, 18, 19, 19, 16,
			20, 22, 22, 21, 21, 23, 23, 20

		};

		indices = tIArray(iArray, iArray + 48);
	}
	else
	{
		unsigned int iArray[36] =
		{
			0, 2, 1, 0, 3, 2,		// front
			4, 6, 5, 6, 7, 5,		// left
			8, 10, 9, 8, 9, 11,		// back
			12, 14, 13, 14, 15, 13, // right
			16, 18, 17, 18, 16, 19, // top
			20, 22, 21, 20, 21, 23	// bottom
		};
		
		indices = tIArray(iArray, iArray + 36);
	}

	Draw(indices, vertices, indexId, vertexId, bufferManager, effectManager,
		 wireframe ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void D3dRenderer::Draw(const Frustum& frustum, const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager,
					   unsigned int indexId, unsigned int vertexId, bool wireframe)
{
	unsigned int planes[24] =
	{
		0, 1, 2, 3, // near
		0, 3, 7, 4, // left
		4, 7, 6, 5, // far
		1, 6, 2, 5, // right
		3, 2, 6, 7, // top
		0, 5, 1, 4, // bottom
	};

	tVArray vertices(24);	
	unsigned int j = 0;
	for (unsigned int i = 0; i < 24;)
	{
		const tVec3f& first = frustum.GetCorner(static_cast<Frustum::Corner>(planes[i++]));
		const tVec3f& second = frustum.GetCorner(static_cast<Frustum::Corner>(planes[i++]));
		const tVec3f& third = frustum.GetCorner(static_cast<Frustum::Corner>(planes[i++]));
		const tVec3f& fourth = frustum.GetCorner(static_cast<Frustum::Corner>(planes[i++]));
		
		Plane plane(first, second, third, Polygon3d::ccw);
		const tVec3f& normal = plane.GetNormal();

		vertices[j++] = VertexData(first, normal);
		vertices[j++] = VertexData(second, normal);
		vertices[j++] = VertexData(third, normal);
		vertices[j++] = VertexData(fourth, normal);
	}

	tIArray indices;

	if (wireframe)
	{
		unsigned int iArray[48] =
		{
			0, 1, 1, 2, 2, 3, 3, 0,
			4, 5, 5, 6, 6, 7, 7, 4,
			8, 9, 9, 10, 10, 11, 11, 8,
			12, 14, 14, 13, 13, 15, 15, 12,
			16, 17, 17, 18, 18, 19, 19, 16,
			20, 22, 22, 21, 21, 23, 23, 20

		};

		indices = tIArray(iArray, iArray + 48);
	}
	else
	{
		unsigned int iArray[36] =
		{
			0, 1, 2, 0, 2, 3,		// near
			4, 5, 6, 4, 6, 7,		// left
			8, 9, 10, 10, 11, 8,	// far
			12, 13, 14, 12, 15, 13, // right
			16, 17, 18, 16, 18, 19, // top
			20, 21, 22, 20, 23, 21	// bottom
		};

		indices = tIArray(iArray, iArray + 36);
	}

	Draw(indices, vertices, indexId, vertexId, bufferManager, effectManager,
		 wireframe ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void D3dRenderer::Draw(const PolygonBody& body, const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager,
					   unsigned int indexId, unsigned int vertexId)
{
	tVArray vertices;
	tIArray indices;

	const PolygonBody::tPolygonArray& polygons = body.GetPolygonArray();

	for (unsigned int i = 0; i < body.GetPolygonCount(); ++i)
	{
		Polygon3d* polygon = polygons[i];
		
		if (polygon->GetVertexCount() < 3)
			continue;

		const Polygon3d::tVertexArray v = polygons[i]->GetVertexArray();
		const tVec3f& n = polygon->GetNormal();

		for (unsigned int j = 1; j < polygon->GetVertexCount()-1;)
		{
			vertices.push_back(VertexData(v[0], n));
			indices.push_back(vertices.size() - 1);

			vertices.push_back(VertexData(v[j], n));
			indices.push_back(vertices.size() - 1);

			vertices.push_back(VertexData(v[++j], n));
			indices.push_back(vertices.size() - 1);
		}		
	}

	Draw(indices, vertices, indexId, vertexId, bufferManager, effectManager, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void D3dRenderer::Draw(const Line& line, const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager,
					   unsigned int indexId, unsigned int vertexId)
{
	tVArray vertices(2);
	vertices[0] = VertexData(line.GetStartPoint(), tVec3f(.0f));
	vertices[1] = VertexData(line.GetEndPoint(), tVec3f(.0f));

	tIArray indices(2);
	indices[0] = 0;
	indices[1] = 1;

	Draw(indices, vertices, indexId, vertexId, bufferManager, effectManager, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void D3dRenderer::Draw(const RenderSystem::tLineList& lineList, const D3dBufferManager& bufferManager,
					   const D3dEffectManager& effectManager, unsigned int indexId, unsigned int vertexId)
{
	tVArray vertices(2 * lineList.size());
	tIArray indices(2 * lineList.size());

	unsigned int j = 0;
	for (unsigned int i = 0; i < lineList.size(); ++i, j += 2)
	{
		vertices[j] = VertexData(lineList[i]->GetStartPoint(), tVec3f(.0f));
		vertices[j+1] = VertexData(lineList[i]->GetEndPoint(), tVec3f(.0f));

		indices[j] = j;
		indices[j+1] = j+1;
	}

	Draw(indices, vertices, indexId, vertexId, bufferManager, effectManager, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void D3dRenderer::Draw(const RenderSystem::Viewport&, const D3dEffectManager& effectManager)
{
	UINT stride = 0;
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = 0;

	_d3dContext->IASetInputLayout(0);
	_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	effectManager.GetActiveD3dPass()->Apply(0,_d3dContext);
	_d3dContext->Draw(3, 0);
}

void D3dRenderer::Draw(const tIArray& indices, const tVArray& vertices, unsigned int indexId, unsigned int vertexId,
					   const D3dBufferManager& bufferManager, const D3dEffectManager& effectManager,
					   D3D11_PRIMITIVE_TOPOLOGY topology)
{
	if (!vertices.size() || !indices.size())
		return;

	const D3dBufferManager::tBufferArray& buffers = bufferManager.GetBufferArray();

	D3D11_MAPPED_SUBRESOURCE map;

	if (SUCCEEDED(_d3dContext->Map(buffers[indexId], 0, D3D11_MAP_WRITE_DISCARD, 0, &map)))
	{
		memcpy(map.pData, &indices[0], sizeof(unsigned int) * indices.size());
		_d3dContext->Unmap(buffers[indexId],0);
	}
	
	if (SUCCEEDED(_d3dContext->Map(buffers[vertexId], 0, D3D11_MAP_WRITE_DISCARD, 0, &map)))
	{
		memcpy(map.pData, &vertices[0].pos, sizeof(VertexData) * vertices.size());
		_d3dContext->Unmap(buffers[vertexId],0);
	}

	_d3dContext->IASetInputLayout(effectManager.GetActiveD3dInputLayout());
	_d3dContext->IASetPrimitiveTopology(topology);
	
	UINT stride = static_cast<UINT>(sizeof(VertexData));
	UINT offset = 0;

	_d3dContext->IASetVertexBuffers(0, 1, &buffers[vertexId], &stride, &offset);
	_d3dContext->IASetIndexBuffer(buffers[indexId], DXGI_FORMAT_R32_UINT, offset);

	effectManager.GetActiveD3dPass()->Apply(0,_d3dContext);
	_d3dContext->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);	
}