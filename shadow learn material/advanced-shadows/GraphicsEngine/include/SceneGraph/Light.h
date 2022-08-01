#pragma once

#include "SceneGraphExports.h"
#include "Common/VectorMathTypes.h"
#include "View.h"

class SCENEGRAPH_API Light
{
public:
	Light();

	const tVec4f& GetAmbient() const;
	void SetAmbient(const tVec4f& ambient);

	const tVec4f& GetDiffuse() const;
	void SetDiffuse(const tVec4f& diffuse);

	const tVec4f& GetSpecular() const;
	void SetSpecular(const tVec4f& specular);

	float GetConstantAttenuation() const;
	void SetConstantAttenuation(float attenuation);

	float GetLinearAttenuation() const;
	void SetLinearAttenuation(float attenuation);

	float GetQuadraticAttenuation() const;
	void SetQuadraticAttenuation(float attenuation);

	int GetSpotExponent() const;
	void SetSpotExponent(int exponent);

	int GetSpotCutoff() const;
	void SetSpotCutoff(int cutoff);

	const tVec3f& GetSpotDirection() const;	
	void SetSpotDirection(const tVec3f& direction);

	const tVec4f& GetPosition() const;
	void SetPosition(const tVec4f& position);

	const View& GetView();

private:
	void UpdateView();

private:
	float _constAttenuation;
	float _linAttenuation;
	float _quadAttenuation;
	int _spotExponent;
	int _spotCutoff;
	View _view;

#pragma warning(push)
#pragma warning(disable : 4251)
	tVec4f _ambient;
	tVec4f _diffuse;
	tVec4f _specular;

	tVec3f _spotDirection;
	tVec4f _position;
#pragma warning(pop)
};
