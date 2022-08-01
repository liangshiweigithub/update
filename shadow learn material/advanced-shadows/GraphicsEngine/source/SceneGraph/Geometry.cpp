#include "stdafx.h"
#include "Geometry.h"
#include "Mesh.h"

Geometry::Geometry() :
	_vertexDataBindId(0),
	_indexDataBindId(0),
	_vboDataStride(0),
	_vertexDataOffset(0),
	_normalDataOffset(0),
	_texCoordDataOffset(0),
	_vertexIndices(0),
	_normalIndices(0),
	_texCoordIndices(0)
{
}

Geometry::~Geometry()
{
}

const Geometry::tIndexArray& Geometry::GetVertexIndices() const
{
	return _vertexIndices;
}

unsigned int Geometry::GetVertexIndicesSize() const
{
	return static_cast<unsigned int>(_vertexIndices.size());
}

void Geometry::SetVertexIndices(const tIndexArray& vertexIndices)
{
	_vertexIndices = vertexIndices;
}

void Geometry::AddVertexIndex(const tIndex& vertexIndex)
{
	_vertexIndices.push_back(vertexIndex);
}

const Geometry::tIndexArray& Geometry::GetNormalIndices() const
{
	return _normalIndices;
}

unsigned int Geometry::GetNormalIndicesSize() const
{
	return static_cast<unsigned int>(_normalIndices.size());
}

void Geometry::SetNormalIndices(const tIndexArray& normalIndices)
{
	_normalIndices = normalIndices;
}

void Geometry::AddNormalIndex(const tIndex& normalIndex)
{
	_normalIndices.push_back(normalIndex);
}

const Geometry::tIndexArray& Geometry::GetTexCoordIndices() const
{
	return _texCoordIndices;
}

unsigned int Geometry::GetTexCoordIndicesSize() const
{
	return static_cast<unsigned int>(_texCoordIndices.size());
}

void Geometry::SetTexCoordIndices(const tIndexArray& texCoordIndices)
{
	_texCoordIndices = texCoordIndices;
}

void Geometry::AddTexCoordIndex(const Geometry::tIndex& texCoordIndex)
{
	_texCoordIndices.push_back(texCoordIndex);
}

unsigned int& Geometry::GetVertexDataBindId()
{
	return _vertexDataBindId;
}

unsigned int Geometry::GetVertexDataBindId() const
{
	return _vertexDataBindId;
}

void Geometry::SetVertexDataBindId(unsigned int id)
{
	_vertexDataBindId = id;
}

unsigned int& Geometry::GetIndexDataBindId()
{
	return _indexDataBindId;
}

unsigned int Geometry::GetIndexDataBindId() const
{
	return _indexDataBindId;
}

void Geometry::SetIndexDataBindId(unsigned int id)
{
	_indexDataBindId = id;
}

int Geometry::GetVboDataStride() const
{
	return _vboDataStride;
}

void Geometry::SetVboDataStride(unsigned int vboDataStride)
{
	_vboDataStride = static_cast<int>(vboDataStride);
}

unsigned int Geometry::GetVertexDataOffset() const
{
	return _vertexDataOffset;
}

void Geometry::SetVertexDataOffset(unsigned int offset)
{
	_vertexDataOffset = offset;
}

unsigned int Geometry::GetNormalDataOffset() const
{
	return _normalDataOffset;
}

void Geometry::SetNormalDataOffset(unsigned int offset)
{
	_normalDataOffset = offset;
}

unsigned int Geometry::GetTexCoordDataOffset() const
{
	return _texCoordDataOffset;
}

void Geometry::SetTexCoordDataOffset(unsigned int offset)
{
	_texCoordDataOffset = offset;
}

const Mesh* Geometry::GetMesh() const
{
	return _mesh;
}

Mesh* Geometry::GetMesh()
{
	return _mesh;
}

void Geometry::SetMesh(Mesh* mesh)
{
	if(mesh)
		_mesh = mesh;
}

void Geometry::Render(RenderSystem& renderSystem) const
{
	renderSystem.Draw(*this);
}
