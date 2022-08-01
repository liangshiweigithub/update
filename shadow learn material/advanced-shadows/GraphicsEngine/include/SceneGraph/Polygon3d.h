#pragma once

#include "SceneGraphExports.h"
#include "Common/VectorMathTypes.h"
#include <vector>

class Line;
class Plane;

class SCENEGRAPH_API Polygon3d
{
public:
	enum VertexOrder
	{
		ccw = 0,
		cw
	};

	typedef tVec3f tVertex;
	typedef std::vector<tVertex> tVertexArray;
	
	Polygon3d();
	Polygon3d(const tVertex& p0, const tVertex& p1, const tVertex& p2);
	Polygon3d(const tVertex& p0, const tVertex& p1, const tVertex& p2, const tVertex& p3);

	unsigned int GetVertexCount() const;
	const tVertexArray& GetVertexArray() const;
	void AddVertex(const tVertex& vertex);
	void AddUniqueVertex(const tVertex& vertex, float epsilon = 1.0e-6f);

	void Clip(const Plane& plane, tVertexArray& intersectionPoints);
	void ReverseVertices();

	const tVec3f& GetNormal();
	void SetNormal(const tVec3f& normal);

	static bool CmpVertices(const tVec3f& v0, const tVec3f& v1, float epsilon = 1.0e-6f);
	static bool CmpCoords(float c0, float c1, float epsilon = 1.0e-6f);

private:
#pragma warning(push)
#pragma warning(disable : 4251)
	tVertexArray _vertices;
	tVec3f _normal;
#pragma warning(pop)
};