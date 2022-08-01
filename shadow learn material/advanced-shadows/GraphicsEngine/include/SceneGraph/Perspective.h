#pragma once

#include "SceneGraphExports.h"

#include "Projection.h"

class SCENEGRAPH_API Perspective : public Projection
{
public:
	Perspective(float fov, float aspect, float zNear, float zFar);
	
	Projection::Type GetType() const;

	float GetFOV() const;
	void SetFOV(float fov);

	float GetAspect() const;
	void SetAspect(float aspect);

private:
	float _fov;
	float _aspect;


};