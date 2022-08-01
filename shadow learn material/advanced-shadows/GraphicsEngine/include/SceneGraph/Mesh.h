#pragma once

#include <vector>

#include "Common/VectorMathTypes.h"

#include "SceneGraphExports.h"
#include "Common/Object.h"
#include "Geometry.h"
#include "BoundingBox.h"


class SCENEGRAPH_API Mesh : public Object
{
public:
	typedef tVec3f tVertex;
	typedef tVec3f tNormal;
	typedef tVec2f tTexCoord;

	typedef std::vector<tVertex> tVertexArray;
	typedef std::vector<tNormal> tNormalArray;
	typedef std::vector<tTexCoord> tTexCoordArray;

	typedef std::vector<Geometry*> tGeometryArray;

	Mesh();
	~Mesh();

	const tVertexArray& GetVertexArray() const;
	unsigned int GetVertexArraySize() const;
	void SetVertexArray(const tVertexArray& vertexArray);
	void AddVertex(const tVertex& vertex);
	
	const tNormalArray& GetNormalArray() const;
	unsigned int GetNormalArraySize() const;
	void SetNormalArray(const tNormalArray& normalArray);
	void AddNormal(const tNormal& normal);

	const tTexCoordArray& GetTexCoordArray() const;
	unsigned int GetTexCoordArraySize() const;
	void SetTexCoordArray(const tTexCoordArray& texCoordArray);
	void AddTexCoord(const tTexCoord& texCoord);

	unsigned int GetGeometryCount() const;
	Geometry* GetGeometry(unsigned int index) const;
	void AddGeometry(Geometry* geometry);

	unsigned int GetDisplayListId() const;
	void SetDisplayListId(unsigned int value);

	BoundingBox& GetBoundingVolume();
	const BoundingBox& GetBoundingVolume() const;

	void AddSplit(unsigned int split);
	void ResetSplits();
	int GetFirstSplit() const;
	int GetLastSplit() const; 

private:
	BoundingBox _boundingVolume;

	int _firstSplit;
	int _lastSplit;

#pragma warning(push)
#pragma warning(disable : 4251)
	tVertexArray _vertexArray;
	tNormalArray _normalArray;	
	tTexCoordArray _texCoordArray;
	tGeometryArray _geometryArray;
#pragma warning(pop)
	
};
