#include "stdafx.h"
#include "Light.h"

Light::Light() :
	_constAttenuation(1),
	_linAttenuation(0),
	_quadAttenuation(0),
	_spotExponent(0),
	_spotCutoff(180),
	_ambient(.2f, .2f, .2f, 1.0f),
	_diffuse(1.0f, 1.0f, 1.0f, 1.0f),
	_specular(1.0f, 1.0f, 1.0f, 1.0f),
	_spotDirection(.0f, .0f, .0f),
	_position(.0f, 1.0f, .0f, .0f),
	_view(tVec3f(.0f, 1.0f, .0f), tVec3f(.0f, .0f, .0f), tVec3f(.0f, .0f, 1.0f))
{
}

const tVec4f& Light::GetAmbient() const
{
	return _ambient;
}

void Light::SetAmbient(const tVec4f& ambient)
{
	_ambient = ambient;
}

const tVec4f& Light::GetDiffuse() const
{
	return _diffuse;
}

void Light::SetDiffuse(const tVec4f& diffuse)
{
	_diffuse = diffuse;
}

const tVec4f& Light::GetSpecular() const
{
	return _specular;;
}

void Light::SetSpecular(const tVec4f& specular)
{
	_specular = specular;
}

float Light::GetConstantAttenuation() const
{
	return _constAttenuation;
}
void Light::SetConstantAttenuation(float attenuation)
{
	_constAttenuation = attenuation;
}

float Light::GetLinearAttenuation() const
{
	return _linAttenuation;
}
void Light::SetLinearAttenuation(float attenuation)
{
	_linAttenuation = attenuation;
}

float Light::GetQuadraticAttenuation() const
{
	return _quadAttenuation;
}

void Light::SetQuadraticAttenuation(float attenuation)
{
	_quadAttenuation = attenuation;
}

int Light::GetSpotExponent() const
{
	return _spotExponent;
}

void Light::SetSpotExponent(int exponent)
{
	_spotExponent = exponent;
}

int Light::GetSpotCutoff() const
{
	return _spotCutoff;
}

void Light::SetSpotCutoff(int cutoff)
{
	_spotCutoff = cutoff;
}

const tVec3f& Light::GetSpotDirection() const
{
	return _spotDirection;
}

void Light::SetSpotDirection(const tVec3f& direction)
{
	if(direction == _position)
		return;

	_spotDirection = direction;
}

const tVec4f& Light::GetPosition() const
{
	return _position;
}

void Light::SetPosition(const tVec4f& position)
{
	if(position == _spotDirection)
		return;
	
	_position = position;
}

const View& Light::GetView()
{
	tVec3f viewDir = _spotDirection - tVec3f(_position);

	tVec3f viewVec = vmath::normalize(viewDir);

	// cross product with perpendicular to viewVec
	tVec3f leftVec = vmath::cross(tVec3f(.0, -viewVec.z, viewVec.y), viewVec);

	// cross product to get right handed system
	tVec3f upVec = vmath::normalize(vmath::cross(viewVec, leftVec));

	_view.SetEye(tVec3f(_position));
	_view.SetCenter(_spotDirection);
	_view.SetUp(upVec);
	
	return _view;
}

