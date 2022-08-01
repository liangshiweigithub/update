#pragma once

#include "SceneGraphExports.h"
#include <vector>

#include "Plane.h"
#include "Polygon3d.h"
#include "Frustum.h"

class SCENEGRAPH_API PolygonBody
{
public:
	typedef std::vector<Polygon3d*> tPolygonArray;
	typedef std::vector<Line*> tEdgeArray;

	PolygonBody();
	~PolygonBody();

	unsigned int GetPolygonCount() const;
	const tPolygonArray& GetPolygonArray() const;
	void Add(Polygon3d* polygon);
	void Add(const Polygon3d::tVertexArray& vertices, const tVec3f& normal);
	void Add(const Frustum& frustum);

	unsigned int GetVertexCount() const;
	const Polygon3d::tVertexArray& GetVertexArray() const;
	
	void Clip(const Plane& plane);
	void Clip(const BoundingBox& boundingBox);
	void Clip(const Frustum& frustum);
	void Extrude(const tVec3f& direction, const BoundingBox& boundingBox);
	void CreateHull(const tVec3f& point);
	void CreateConvexHull(const tVec3f& point);
	void CreateConvexHull();
	void CreateUniqueVertexArray(float epsilon = 1.0e-6f);
	void Clear();

private:
	void AddUniqueEdge(tEdgeArray& outEdgeList, const Line& line) const;
	void AddUniqueVertex(const Polygon3d::tVertex& vertex, float epsilon);
	void RemoveVisiblePolygons(const tVec3f& point);
	void Intersect(const Line& line, const tVec3f& min, const tVec3f& max, Polygon3d::tVertexArray& vertices);
	bool Clip(float p, float q, float& u1, float& u2) const;
	void ExtrudePolygons(const tVec3f& direction, const BoundingBox& boundingBox);
	void CreateConvexHull(const Polygon3d::tVertexArray& vertices);
	
private:
#pragma warning(push)
#pragma warning(disable : 4251)
	tPolygonArray _polygons;
	Polygon3d::tVertexArray _uniqueVertices;
#pragma warning(pop)
};
