#include "stdafx.h"

#include "View.h"

View::View(const tVec3f& eye, const tVec3f& center, const tVec3f& up) :
	_eye(eye),
	_center(center),
	_up(up)
{
}

const tVec3f& View::GetEye() const
{
	return _eye;
}

void View::SetEye(const tVec3f& eye)
{
	_eye = eye;
}

const tVec3f& View::GetCenter() const
{
	return _center;
}

void View::SetCenter(const tVec3f& center)
{
	_center = center;
}

const tVec3f& View::GetUp() const
{
	return _up;
}

void View::SetUp(const tVec3f& up)
{
	_up = up;
}