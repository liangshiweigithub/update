#include "stdafx.h"

#include "Plane.h"

Plane::Plane(const tVec4f& equation) :
	_normal(vmath::normalize(tVec3f(equation))),
	_distance(equation.w)
{
}

Plane::Plane(const tVec3f& normal, float distance) :
	_normal(vmath::normalize(normal)),
	_distance(distance)
{
}

Plane::Plane(const tVec3f& v0, const tVec3f& v1, const tVec3f& v2, Polygon3d::VertexOrder order)
{
	switch(order)
	{
		case Polygon3d::ccw:
			_normal = vmath::normalize(vmath::cross(v1-v0, v2-v0));
			break;
		case Polygon3d::cw:
			_normal = vmath::normalize(vmath::cross(v2-v0, v1-v0));
			break;
	}
	_distance = vmath::dot(v0, _normal);
}

Plane::Plane(const tVec3f& point, const tVec3f& normal)
{
	_normal = vmath::normalize(normal);

	_distance = vmath::dot(_normal, point);
}

const tVec4f& Plane::GetEquation()
{
	_equation = tVec4f(_normal, _distance);
	return _equation;
}

void Plane::SetEquation(const tVec4f& equation)
{
	_normal = vmath::normalize(tVec3f(equation));
	_distance = equation.w;
}

const tVec3f& Plane::GetNormal() const
{
	return _normal;
}

float Plane::GetDistance() const
{
	return _distance;
}

float Plane::GetDistance(const tVec3f& point) const
{
	return vmath::dot(point, _normal) - _distance;
}

bool Plane::IsBehind(const tVec3f& point) const
{
	bool behind = false;

	if (GetDistance(point) < .0)
		behind = true;

	return behind;
}

bool Plane::Intersect(float& param, const Line& line, float epsilon) const
{
	float prod = vmath::dot(_normal, line.GetDirection());
	
	// line and plane closely parallel
	if (abs(prod) < epsilon)
	{
		param = .0f;
		return false;
	}

	param = (_distance - vmath::dot(_normal, line.GetStartPoint())) / prod;

	// intersection outside of line
	if(param < -epsilon || param > (1.0f + epsilon))
	{
		param = .0;
		return false;
	}

	return true;
}

void Plane::Transform(const tMat4f& matrix)
{
	tVec4f transEquation = matrix * tVec4f(_normal, -_distance);

	_normal = tVec3f(transEquation);
	_distance = -transEquation.w;

	CalcHessNorm();
}

void Plane::CalcHessNorm()
{
	float len = vmath::length(_normal);
	
	_normal = _normal / len;
	_distance = _distance / len;
}

