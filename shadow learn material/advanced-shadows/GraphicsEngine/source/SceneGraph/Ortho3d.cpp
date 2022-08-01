#include "stdafx.h"
#include "Ortho3d.h"

Ortho3d::Ortho3d(float left, float right, float bottom, float top, float zNear, float zFar) :
	_left(left),
	_right(right),
	_bottom(bottom),
	_top(top)
{
	_zNear = zNear;
	_zFar = zFar;
}

Projection::Type Ortho3d::GetType() const
{
	return Projection::Ortho3d;
}

float Ortho3d::GetLeft() const
{
	return _left;
}

void Ortho3d::SetLeft(float left)
{
	_left = left;
}

float Ortho3d::GetRight() const
{
	return _right;
}

void Ortho3d::SetRight(float right)
{
	_right = right;
}

float Ortho3d::GetBottom() const
{
	return _bottom;
}

void Ortho3d::SetBottom(float bottom)
{
	_bottom = bottom;
}

float Ortho3d::GetTop() const
{
	return _top;
}

void Ortho3d::SetTop(float top)
{
	_top = top;
}
