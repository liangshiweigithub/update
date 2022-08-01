#pragma once

#include "SceneGraphExports.h"

#include "Common/VectorMathTypes.h"
#include "Line.h"
#include "Polygon3d.h"

class SCENEGRAPH_API Plane
{
public:
	Plane(const tVec4f& equation);
	Plane(const tVec3f& normal, float distance);
	Plane(const tVec3f& v0, const tVec3f& v1, const tVec3f& v2, Polygon3d::VertexOrder order);
	Plane(const tVec3f& point, const tVec3f& normal);

	const tVec4f& GetEquation();
	void SetEquation(const tVec4f& equation);

	const tVec3f& GetNormal() const;
	float GetDistance() const;
	float GetDistance(const tVec3f& point) const;
	
	bool IsBehind(const tVec3f& point) const;
	bool Intersect(float& param, const Line& line, float epsilon = 1.0e-6f) const;
	void Transform(const tMat4f& matrix);

private:
	void CalcHessNorm();

private:
	float _distance;
#pragma warning(push)
#pragma warning(disable : 4251)
	tVec3f _normal;
	tVec4f _equation;
#pragma warning(pop)

};