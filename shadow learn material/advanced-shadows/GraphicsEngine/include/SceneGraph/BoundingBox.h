#pragma once

#include <vector>
#include "SceneGraphExports.h"
#include "Common/VectorMathTypes.h"

class Node;

class SCENEGRAPH_API BoundingBox
{
public:
	enum Corner
	{
		MinMinMin = 0,
		MaxMinMin,
		MaxMaxMin,
		MinMaxMin,
		MinMinMax,
		MaxMinMax,
		MaxMaxMax,
		MinMaxMax,
		CornerCOUNT
	};

	enum AABBPlane
	{
		Front = 0,
		Back,
		Bottom,
		Top,
		Left,
		Right,
		PlaneCOUNT
	};

	typedef std::vector<tVec3f> tCornerArray;

	BoundingBox();
	BoundingBox(const tVec3f& minimum, const tVec3f& maximum);

	const tVec3f& GetMinimum() const;
	void SetMinimum(const tVec3f& minimum);
	
	const tVec3f& GetMaximum() const;
	void SetMaximum(const tVec3f& maximum);

	tVec3f GetCorner(Corner corner) const;
	tVec4f GetPlaneEquation(AABBPlane plane) const;
	const tCornerArray& GetCornerArray(const tMat4f& transform);

	void Expand(const tVec3f& vertex);
	void Expand(const tVec3f& vertex, const tMat4f& transform);
	void Expand(const BoundingBox& boundingVolume);
	void Expand(Node& node);
	void Expand(Node& node, const tMat4f& transform);

private:

private:
#pragma warning (push)
#pragma warning (disable : 4251)
	tVec3f _minimum;
	tVec3f _maximum;
	tCornerArray _corners;
#pragma warning (pop)
};