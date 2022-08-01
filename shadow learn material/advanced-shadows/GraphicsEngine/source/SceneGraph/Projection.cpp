#include "stdafx.h"

#include "Projection.h"

Projection::~Projection()
{
}

float Projection::GetZNear() const
{
	return _zNear;
}

void Projection::SetZNear(float zNear)
{
	_zNear = zNear;
}

float Projection::GetZFar() const
{
	return _zFar;
}

void Projection::SetZFar(float zFar)
{
	_zFar = zFar;
}