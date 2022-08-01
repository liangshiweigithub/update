#include "stdafx.h"

#include "D3dRenderSystem.h"

void D3dCuller::Cull(GroupNode& groupNode, RenderSystem& renderSystem)
{
	if (!groupNode.GetBoundingVolume())
		return;

	const tVec3f& bvMin = groupNode.GetBoundingVolume()->GetMinimum();
	const tVec3f& bvMax = groupNode.GetBoundingVolume()->GetMaximum();
	
	bool outside = false;
	
	//if (abs(bvMin.x) >= FLT_MAX || abs(bvMin.y) >= FLT_MAX || abs(bvMin.z) >= FLT_MAX ||
	//	abs(bvMax.x) >= FLT_MAX || abs(bvMax.y) >= FLT_MAX || abs(bvMax.z) >= FLT_MAX)
	//{
	//	groupNode.SetCulled(outside);
	//	return;
	//}

	D3DXMATRIX& view  = *reinterpret_cast<D3DXMATRIX*>(renderSystem.GetViewMatrix());
	D3DXMATRIX& proj  = *reinterpret_cast<D3DXMATRIX*>(renderSystem.GetProjMatrix());
	D3DXMATRIX& world = *reinterpret_cast<D3DXMATRIX*>(renderSystem.GetWorldMatrix());

	D3DXMATRIX frustum = world * view * proj;

	tVec4f planes[Culler::PlaneCount];

	planes[Culler::Left].x = frustum._14 + frustum._11;
	planes[Culler::Left].y = frustum._24 + frustum._21;
	planes[Culler::Left].z = frustum._34 + frustum._31;
	planes[Culler::Left].w = frustum._44 + frustum._41;

	planes[Culler::Right].x = frustum._14 - frustum._11;
	planes[Culler::Right].y = frustum._24 - frustum._21;
	planes[Culler::Right].z = frustum._34 - frustum._31;
	planes[Culler::Right].w = frustum._44 - frustum._41;

	planes[Culler::Top].x = frustum._14 - frustum._12;
	planes[Culler::Top].y = frustum._24 - frustum._22;
	planes[Culler::Top].z = frustum._34 - frustum._32;
	planes[Culler::Top].w = frustum._44 - frustum._42;

	planes[Culler::Bottom].x = frustum._14 + frustum._12;
	planes[Culler::Bottom].y = frustum._24 + frustum._22;
	planes[Culler::Bottom].z = frustum._34 + frustum._32;
	planes[Culler::Bottom].w = frustum._44 + frustum._42;

	planes[Culler::Near].x = frustum._13;
	planes[Culler::Near].y = frustum._23;
	planes[Culler::Near].z = frustum._33;
	planes[Culler::Near].w = frustum._43;

	planes[Culler::Far].x = frustum._14 - frustum._13;
	planes[Culler::Far].y = frustum._24 - frustum._23;
	planes[Culler::Far].z = frustum._34 - frustum._33;
	planes[Culler::Far].w = frustum._44 - frustum._43;

	for (unsigned int i = 0; i < PlaneCount && !outside; ++i)
	{
		const float mag = vmath::length(tVec3f(planes[i]));		
		
		const tVec3f plane = tVec3f(planes[i]) / mag;

		const float dist = planes[i].w / mag;
		
		const tVec3f pVertex(((plane.x >= 0) ? bvMax.x : bvMin.x),
							 ((plane.y >= 0) ? bvMax.y : bvMin.y),
							 ((plane.z >= 0) ? bvMax.z : bvMin.z));

		if ((vmath::dot(plane, pVertex) + dist) < 0)
			outside = true;

		//const tVec3f nVertex(((plane.x <= 0) ? bvMax.x : bvMin.x),
		//					 ((plane.y <= 0) ? bvMax.y : bvMin.y),
		//					 ((plane.z <= 0) ? bvMax.z : bvMin.z));
	}

	groupNode.SetCulled(outside);
}

