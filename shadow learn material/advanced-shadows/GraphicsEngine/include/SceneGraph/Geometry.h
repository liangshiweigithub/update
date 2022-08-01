#pragma once

#include <vector>

#include "SceneGraphExports.h"
#include "Drawable.h"

class Mesh;

class SCENEGRAPH_API Geometry : public Drawable
{
public:
	typedef unsigned int tIndex;
	typedef std::vector<tIndex> tIndexArray;

	Geometry();
	~Geometry();

	const tIndexArray& GetVertexIndices() const;
	unsigned int GetVertexIndicesSize() const;
	void SetVertexIndices(const tIndexArray& vertexIndices);
	void AddVertexIndex(const tIndex& vertexIndex);
	
	const tIndexArray& GetNormalIndices() const;
	unsigned int GetNormalIndicesSize() const;
	void SetNormalIndices(const tIndexArray& normalIndices);
	void AddNormalIndex(const tIndex& normalIndex);
	
	const tIndexArray& GetTexCoordIndices() const;
	unsigned int GetTexCoordIndicesSize() const;
	void SetTexCoordIndices(const tIndexArray& texCoordIndices);
	void AddTexCoordIndex(const tIndex& texCoordIndex);

	unsigned int& GetVertexDataBindId();
	unsigned int GetVertexDataBindId() const;
	void SetVertexDataBindId(unsigned int id);

	unsigned int& GetIndexDataBindId();
	unsigned int GetIndexDataBindId() const;
	void SetIndexDataBindId(unsigned int id);

	int GetVboDataStride() const;
	void SetVboDataStride(unsigned int vboDataStride);

	unsigned int GetVertexDataOffset() const;
	void SetVertexDataOffset(unsigned int offset);

	unsigned int GetNormalDataOffset() const;
	void SetNormalDataOffset(unsigned int offset);

	unsigned int GetTexCoordDataOffset() const;
	void SetTexCoordDataOffset(unsigned int offset);

	const Mesh* GetMesh() const;
	Mesh* GetMesh();
	void SetMesh(Mesh* mesh);

	void Render(RenderSystem& renderSystem) const;

private:
	unsigned int _vertexDataBindId;
	unsigned int _indexDataBindId;

	int _vboDataStride;
	unsigned int _vertexDataOffset;
	unsigned int _normalDataOffset;
	unsigned int _texCoordDataOffset;

	Mesh* _mesh;

#pragma warning(push)
#pragma warning(disable : 4251)
	tIndexArray _vertexIndices;
	tIndexArray _normalIndices;
	tIndexArray _texCoordIndices;
#pragma warning(pop)
};