#include "stdafx.h"

#include "float.h"
#include "BoundingBox.h"
#include "GroupNode.h"

BoundingBox::BoundingBox() :
	_minimum(FLT_MAX, FLT_MAX, FLT_MAX),
	_maximum(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{
}

BoundingBox::BoundingBox(const tVec3f& minimum, const tVec3f& maximum) :
	_minimum(minimum),
	_maximum(maximum)
{
}

const tVec3f& BoundingBox::GetMinimum() const
{
	return _minimum;
}

void BoundingBox::SetMinimum(const tVec3f& minimum)
{
	_minimum = minimum;
}

const tVec3f& BoundingBox::GetMaximum() const
{
	return _maximum;
}

void BoundingBox::SetMaximum(const tVec3f& maximum)
{
	_maximum = maximum;
}

tVec3f BoundingBox::GetCorner(Corner corner) const
{
	tVec3f returnCorner;

	switch(corner)
	{
		case MinMinMin:
			returnCorner = _minimum;
			break;
		case MaxMinMin:
			returnCorner = tVec3f(_maximum.x, _minimum.y, _minimum.z);
			break;
		case MinMaxMin:
			returnCorner = tVec3f(_minimum.x, _maximum.y, _minimum.z);
			break;
		case MaxMaxMin:
			returnCorner = tVec3f(_maximum.x, _maximum.y, _minimum.z);
			break;
		case MinMinMax:
			returnCorner = tVec3f(_minimum.x, _minimum.y, _maximum.z);
			break;
		case MaxMinMax:
			returnCorner = tVec3f(_maximum.x, _minimum.y, _maximum.z);
			break;
		case MinMaxMax:
			returnCorner = tVec3f(_minimum.x, _maximum.y, _maximum.z);
			break;
		case MaxMaxMax:
			returnCorner = _maximum;
			break;
	}

	return returnCorner;
}

tVec4f BoundingBox::GetPlaneEquation(AABBPlane plane) const
{
	tVec4f equation;

	switch(plane)
	{
		case Front:
			equation = tVec4f(.0, .0, 1.0, abs(_maximum.z));
			break;
		case Back:
			equation = tVec4f(.0, .0, -1.0, abs(_minimum.z));
			break;
		case Bottom:
			equation = tVec4f(.0, -1.0, .0, abs(_minimum.y));
			break;
		case Top:
			equation = tVec4f(.0, 1.0, .0, abs(_maximum.y));
			break;
		case Left:
			equation = tVec4f(-1.0, .0, .0, abs(_minimum.x));
			break;
		case Right:
			equation = tVec4f(1.0, .0, .0, abs(_maximum.x));
			break;
	}

	return equation;
}

const BoundingBox::tCornerArray& BoundingBox::GetCornerArray(const tMat4f& transform)
{
	if (_corners.size())
		_corners.clear();

	for (unsigned int i = 0; i < CornerCOUNT; ++i)
		_corners.push_back(vmath::transform(transform, GetCorner(static_cast<Corner>(i))));

	return _corners;
}

void BoundingBox::Expand(const tVec3f& vertex)
{
	// exclude unrealistic values
	if (abs(vertex.x) > FLT_MAX ||
		abs(vertex.y) > FLT_MAX ||
		abs(vertex.z) > FLT_MAX)
		return;

	if (vertex.x < _minimum.x)
		_minimum.x = vertex.x;
	else if(vertex.x > _maximum.x)
		_maximum.x = vertex.x;

	if (vertex.y < _minimum.y)
		_minimum.y = vertex.y;
	else if(vertex.y > _maximum.y)
		_maximum.y = vertex.y;

	if (vertex.z < _minimum.z)
		_minimum.z = vertex.z;
	else if(vertex.z > _maximum.z)
		_maximum.z = vertex.z;
}

void BoundingBox::Expand(const BoundingBox& boundingVolume)
{
	Expand(boundingVolume.GetMinimum());
	Expand(boundingVolume.GetMaximum());
}

void BoundingBox::Expand(const tVec3f& vertex, const tMat4f& transform)
{
	Expand(vmath::transform(transform, vertex));
}

void BoundingBox::Expand(Node& node)
{
	Expand(node, vmath::identity4<float>());
}

void BoundingBox::Expand(Node& node, const tMat4f& transform)
{
	GroupNode* groupNode = dynamic_cast<GroupNode*>(&node);
	TransformNode* transformNode = dynamic_cast<TransformNode*>(&node);

	tMat4f localTransform = transform;

	if (groupNode)
	{		
		localTransform *= vmath::transpose(groupNode->GetTransform().GetMatrix());
		BoundingBox* bv = groupNode->GetBoundingVolume();
	
		if (bv)
			for (unsigned int i = 0; i < CornerCOUNT; ++i)
				Expand(bv->GetCorner(static_cast<Corner>(i)), localTransform);
	}
	else if (transformNode)
		localTransform *= vmath::transpose(transformNode->GetTransform().GetMatrix());
		
	for (unsigned int i = 0; i < node.GetChildCount(); ++i)
		Expand(*node.GetChild(i), localTransform);
}