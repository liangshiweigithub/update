#include "stdafx.h"

#include "Frustum.h"
#include "View.h"
#include "Perspective.h"
#include "Plane.h"

Frustum::Frustum(const tMat4f& invEyeViewProj, const BoundingBox& unitBB)
{
	CalcFrustum(invEyeViewProj, unitBB);
}

Frustum::~Frustum()
{
	for (unsigned int i = 0; i < _planes.size(); ++i)
		delete _planes[i];
}

const tVec3f& Frustum::GetCorner(Corner corner) const
{
	return _corners[corner];
}

const Plane& Frustum::GetPlane(FrustumPlane plane) const
{
	return *_planes[plane];
}

const Plane& Frustum::GetPlaneHessNorm(FrustumPlane plane) const
{
	return *_planes[plane+PlaneCOUNT];
}

const BoundingBox& Frustum::GetBoundingBox()
{
	BoundingBox tempBB;

	for (unsigned int i = 0; i < CornerCOUNT; ++i)
		tempBB.Expand(_corners[i]);

	return (_boundingBox = tempBB);
}

void Frustum::CalcFrustum(const tMat4f& invEyeViewProj, const BoundingBox& unitBB)
{
	if (!_corners.size())
		for (unsigned int i = 0; i < CornerCOUNT; ++i)
			_corners.push_back(tVec3f(.0f, .0f, .0f));

	for (unsigned int i = 0; i < CornerCOUNT; ++i)
	{
		tVec3f corner = unitBB.GetCorner(static_cast<BoundingBox::Corner>(i));
		_corners[i] = vmath::transform(invEyeViewProj, corner);
	}

	CalcPlanes(vmath::transpose(vmath::inverse(invEyeViewProj)));
}

void Frustum::CalcCorners(tVec3f eye, tVec3f center, tVec3f up, float fov, float aspect, float zNear, float zFar)
{
	if (!_corners.size())
		for (unsigned int i = 0; i < CornerCOUNT; ++i)
			_corners.push_back(tVec3f(.0, .0, .0));

	const tVec3f viewVec(vmath::normalize(center - eye));

	const tVec3f leftVec(vmath::normalize(vmath::cross(up, viewVec)));

	const tVec3f upVec(vmath::normalize(vmath::cross(viewVec, leftVec)));

	const float nearHalfHeight = tanf((static_cast<float>(M_PI) / 360.0f) * fov) * zNear;
	const float nearHalfWidth = nearHalfHeight * aspect;

	const float farHalfHeight = tanf((static_cast<float>(M_PI) / 360.0f) * fov) * zFar;
	const float farHalfWidth = farHalfHeight * aspect;

	const tVec3f nearCenter(eye + viewVec * zNear);

	_corners[NearBottomRight] = tVec3f(nearCenter - (leftVec * nearHalfWidth) - (upVec * nearHalfHeight));
	_corners[NearTopRight]    = tVec3f(nearCenter - (leftVec * nearHalfWidth) + (upVec * nearHalfHeight));
	_corners[NearTopLeft]	  = tVec3f(nearCenter + (leftVec * nearHalfWidth) + (upVec * nearHalfHeight));
	_corners[NearBottomLeft]  = tVec3f(nearCenter + (leftVec * nearHalfWidth) - (upVec * nearHalfHeight));

	const tVec3f farCenter(eye + viewVec * zFar);

	_corners[FarBottomRight] = tVec3f(farCenter - (leftVec * farHalfWidth) - (upVec * farHalfHeight));
	_corners[FarTopRight]	 = tVec3f(farCenter - (leftVec * farHalfWidth) + (upVec * farHalfHeight));
	_corners[FarTopLeft]	 = tVec3f(farCenter + (leftVec * farHalfWidth) + (upVec * farHalfHeight));
	_corners[FarBottomLeft]	 = tVec3f(farCenter + (leftVec * farHalfWidth) - (upVec * farHalfHeight));
}

void Frustum::CalcPlanes(const tMat4f& eyeViewProj)
{
	if (!_planes.size())
		for (unsigned int i = 0; i < 2 * PlaneCOUNT; ++i)
			_planes.push_back(new Plane(tVec3f(.0, .0, .0), .0));

	const tVec4f nearPl(
		eyeViewProj.elem[0][3] + eyeViewProj.elem[0][2],
		eyeViewProj.elem[1][3] + eyeViewProj.elem[1][2],
		eyeViewProj.elem[2][3] + eyeViewProj.elem[2][2],
		eyeViewProj.elem[3][3] + eyeViewProj.elem[3][2]);

	float len = vmath::length(tVec3f(nearPl));
	
	_planes[Near]->SetEquation(nearPl);
	_planes[Near+PlaneCOUNT]->SetEquation(tVec4f(nearPl.x, nearPl.y, nearPl.z, nearPl.w/len));

	const tVec4f farPl(
		eyeViewProj.elem[0][3] - eyeViewProj.elem[0][2],
		eyeViewProj.elem[1][3] - eyeViewProj.elem[1][2],
		eyeViewProj.elem[2][3] - eyeViewProj.elem[2][2],
		eyeViewProj.elem[3][3] - eyeViewProj.elem[3][2]);

	len = vmath::length(tVec3f(farPl));
	
	_planes[Far]->SetEquation(farPl);
	_planes[Far+PlaneCOUNT]->SetEquation(tVec4f(farPl.x, farPl.y, farPl.z, farPl.w/len));

	const tVec4f leftPl(
		eyeViewProj.elem[0][3] + eyeViewProj.elem[0][0],
		eyeViewProj.elem[1][3] + eyeViewProj.elem[1][0],
		eyeViewProj.elem[2][3] + eyeViewProj.elem[2][0],
		eyeViewProj.elem[3][3] + eyeViewProj.elem[3][0]);

	len = vmath::length(tVec3f(leftPl));
	
	_planes[Left]->SetEquation(leftPl);
	_planes[Left+PlaneCOUNT]->SetEquation(tVec4f(leftPl.x, leftPl.y, leftPl.z, leftPl.w/len));

	const tVec4f rightPl(
		eyeViewProj.elem[0][3] - eyeViewProj.elem[0][0],
		eyeViewProj.elem[1][3] - eyeViewProj.elem[1][0],
		eyeViewProj.elem[2][3] - eyeViewProj.elem[2][0],
		eyeViewProj.elem[3][3] - eyeViewProj.elem[3][0]);

	len = vmath::length(tVec3f(rightPl));
	
	_planes[Right]->SetEquation(rightPl);
	_planes[Right+PlaneCOUNT]->SetEquation(tVec4f(rightPl.x, rightPl.y, rightPl.z, rightPl.w/len));

	const tVec4f bottomPl(
		eyeViewProj.elem[0][3] + eyeViewProj.elem[0][1],
		eyeViewProj.elem[1][3] + eyeViewProj.elem[1][1],
		eyeViewProj.elem[2][3] + eyeViewProj.elem[2][1],
		eyeViewProj.elem[3][3] + eyeViewProj.elem[3][1]);

	len = vmath::length(tVec3f(bottomPl));
	
	_planes[Bottom]->SetEquation(bottomPl);
	_planes[Bottom+PlaneCOUNT]->SetEquation(tVec4f(bottomPl.x, bottomPl.y, bottomPl.z, bottomPl.w/len));

	const tVec4f topPl(
		eyeViewProj.elem[0][3] - eyeViewProj.elem[0][1],
		eyeViewProj.elem[1][3] - eyeViewProj.elem[1][1],
		eyeViewProj.elem[2][3] - eyeViewProj.elem[2][1],
		eyeViewProj.elem[3][3] - eyeViewProj.elem[3][1]);

	len = vmath::length(tVec3f(topPl));
	
	_planes[Top]->SetEquation(topPl);
	_planes[Top+PlaneCOUNT]->SetEquation(tVec4f(topPl.x, topPl.y, topPl.z, topPl.w/len));
}
