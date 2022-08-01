#pragma once

#include "SceneGraphExports.h"
#include "Projection.h"

class SCENEGRAPH_API Ortho3d : public Projection
{
public:
	Ortho3d(float left, float right, float bottom, float top, float zNear, float zFar);

	Projection::Type GetType() const;

	float GetLeft() const;
	void SetLeft(float left);

	float GetRight() const;
	void SetRight(float right);

	float GetBottom() const;
	void SetBottom(float bottom);

	float GetTop() const;
	void SetTop(float top);

private:
	float _left;
	float _right;
	float _bottom;
	float _top;
};