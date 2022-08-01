#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh() :
	_vertexArray(0),
	_normalArray(0),
	_texCoordArray(0),
	_geometryArray(0),
	_firstSplit(INT_MAX),
	_lastSplit(INT_MIN)
{	
}

Mesh::~Mesh()
{
	for (tGeometryArray::size_type i = 0; i < _geometryArray.size(); ++i)
		delete _geometryArray[i];
}

const Mesh::tVertexArray& Mesh::GetVertexArray() const
{
	return _vertexArray;
}

unsigned int Mesh::GetVertexArraySize() const
{
	return static_cast<unsigned int>(_vertexArray.size());
}

void Mesh::SetVertexArray(const tVertexArray& vertexArray)
{
	_vertexArray = vertexArray;
}

void Mesh::AddVertex(const tVertex& vertex)
{
	_vertexArray.push_back(vertex);
}

const Mesh::tNormalArray& Mesh::GetNormalArray() const
{
	return _normalArray;
}

unsigned int Mesh::GetNormalArraySize() const
{
	return static_cast<unsigned int>(_normalArray.size());
}

void Mesh::SetNormalArray(const tVertexArray& normalArray)
{
	_normalArray = normalArray;
}

void Mesh::AddNormal(const tNormal& normal)
{
	_normalArray.push_back(normal);
}

const Mesh::tTexCoordArray& Mesh::GetTexCoordArray() const
{
	return _texCoordArray;
}

unsigned int Mesh::GetTexCoordArraySize() const
{
	return static_cast<unsigned int>(_texCoordArray.size());
}

void Mesh::SetTexCoordArray(const tTexCoordArray& texCoordArray)
{
	_texCoordArray = texCoordArray;
}

void Mesh::AddTexCoord(const tTexCoord& texCoord)
{
	_texCoordArray.push_back(texCoord);
}

unsigned int Mesh::GetGeometryCount() const
{
	return static_cast<unsigned int>(_geometryArray.size());
}

Geometry* Mesh::GetGeometry(unsigned int index) const
{
	if (index < _geometryArray.size())
		return _geometryArray[index];

	return 0;
}

void Mesh::AddGeometry(Geometry* geometry)
{
	if (geometry)
		_geometryArray.push_back(geometry);
}

BoundingBox& Mesh::GetBoundingVolume()
{
	return _boundingVolume;
}

const BoundingBox& Mesh::GetBoundingVolume() const
{
	return _boundingVolume;
}

void Mesh::AddSplit(unsigned int split)
{
	int current = static_cast<int>(split);

	if (current < _firstSplit)
		_firstSplit = current;
	
	if (current > _lastSplit)
		_lastSplit = current;
}

void Mesh::ResetSplits()
{
	_firstSplit = INT_MAX;
	_lastSplit = INT_MIN;
}

int Mesh::GetFirstSplit() const
{
	return _firstSplit;
}

int Mesh::GetLastSplit() const
{
	return _lastSplit;
}