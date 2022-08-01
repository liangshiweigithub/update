#include "stdafx.h"
#include "Camera.h"

Camera::Camera() :
	_view(tVec3f(.0f, .0f, 1.0f), tVec3f(.0f, .0f, .0f), tVec3f(.0f, 1.0f, .0f)),
	_perspective(Perspective(45.0f, 1024 / 768.0f, 1.0f, 1000.0f))
{
}

Camera::~Camera()
{
}

const View& Camera::GetView() const
{
	return _view;
}

View& Camera::GetView()
{
	return _view;
}

void Camera::SetView(const View& view)
{
	_view = view;
}

const Perspective& Camera::GetPerspective() const
{
	return _perspective;
}

void Camera::SetPerspective(const Perspective& perspective)
{
	_perspective = perspective;
}
