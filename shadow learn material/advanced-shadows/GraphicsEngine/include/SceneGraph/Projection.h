#pragma once

#include "SceneGraphExports.h"

class SCENEGRAPH_API Projection
{
public:
	enum Type
	{
		Perspective = 0,
		Ortho3d
	};

	virtual ~Projection();

	virtual Type GetType() const = 0;

	float GetZNear() const;
	void SetZNear(float zNear);

	float GetZFar() const;
	void SetZFar(float zFar);

protected:
	float _zNear;
	float _zFar;
};