#include "stdafx.h"
#include "Perspective.h"

Perspective::Perspective(float fov, float aspect, float zNear, float zFar) :
	_fov(fov),
	_aspect(aspect)
{
	_zNear = zNear;
	_zFar = zFar;
}

Projection::Type Perspective::GetType() const
{
	return Projection::Perspective;
}

float Perspective::GetFOV() const
{
	return _fov;
}

void Perspective::SetFOV(float fov)
{
	_fov = fov;
}

float Perspective::GetAspect() const
{
	return _aspect;
}

void Perspective::SetAspect(float aspect)
{
	_aspect = aspect;
}
