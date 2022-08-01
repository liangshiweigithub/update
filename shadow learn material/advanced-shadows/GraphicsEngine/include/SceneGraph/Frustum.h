#pragma once

#include "SceneGraphExports.h"

#include <vector>

#include "Common/VectorMathTypes.h"
#include "BoundingBox.h"

class Plane;
class View;
class Perspective;
class Camera;

class SCENEGRAPH_API Frustum
{
public:
	enum Corner
	{
		NearBottomLeft = 0,
		NearBottomRight,
		NearTopRight,
		NearTopLeft,
		FarBottomLeft,
		FarBottomRight,
		FarTopRight,
		FarTopLeft,
		CornerCOUNT
	};

	enum FrustumPlane
	{
		Near = 0,
		Far,
		Left,
		Right,
		Bottom,
		Top,
		PlaneCOUNT
	};

	Frustum(const tMat4f& invEyeViewProj, const BoundingBox& unitBB);
	~Frustum();

	const tVec3f& GetCorner(Corner corner) const;
	const Plane& GetPlane(FrustumPlane plane) const;
	const Plane& GetPlaneHessNorm(FrustumPlane plane) const;
	const BoundingBox& GetBoundingBox();
	
private:
	typedef std::vector<tVec3f> tCornerArray;
	typedef std::vector<Plane*> tPlaneArray;

	void CalcFrustum(const tMat4f& invEyeViewProj, const BoundingBox& unitBB);
	void CalcCorners(tVec3f eye, tVec3f center, tVec3f up, float fov, float aspect, float zNear, float zFar);
	void CalcPlanes(const tMat4f& eyeViewProj);

private:
	BoundingBox _boundingBox;

#pragma warning(push)
#pragma warning(disable : 4251)
	tCornerArray _corners;
	tPlaneArray _planes;
#pragma warning(pop)
};