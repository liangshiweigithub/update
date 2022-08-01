#include "stdafx.h"

#include "Spectator.h"

const tVec2i& Spectator::GetReferencePoint() const
{
	return _reference;
}

void Spectator::SetReferencePoint(const tVec2i& point)
{
	_reference = point;
}

void Spectator::Move(Camera& camera, const float dist) const
{
	const View& view = camera.GetView();
	const tVec3f viewVector(vmath::normalize(view.GetCenter() - view.GetEye()));
	const tVec3f eye(view.GetEye() + viewVector * dist);

	camera.SetView(View(eye, eye + viewVector, view.GetUp()));
}

void Spectator::Lift(Camera& camera, const float dist) const
{
	const View& view = camera.GetView();
	const tVec3f upVector(vmath::normalize(view.GetUp()) * dist);

	camera.SetView(View(view.GetEye() + upVector, view.GetCenter() + upVector, view.GetUp()));
}

void Spectator::Strafe(Camera& camera, const float dist) const
{
	const View& view = camera.GetView();
	const tVec3f viewVector(vmath::normalize(view.GetCenter() - view.GetEye()));
	const tVec3f eye(view.GetEye() + vmath::cross(view.GetUp(), viewVector) * dist);

	camera.SetView(View(eye, eye + viewVector, view.GetUp()));
}

void Spectator::Yaw(Camera& camera, const float angle, const int x) const
{
	const View& view = camera.GetView();
	const float rotAngle = angle * (_reference.x - x) * static_cast<float>(M_PI/180.0);
	const tVec3f viewDir(view.GetCenter() - view.GetEye());
	const tVec3f viewVector(vmath::normalize(viewDir * cos(rotAngle)) + vmath::cross(view.GetUp(), viewDir) * sin(rotAngle));

	camera.SetView(View(view.GetEye(), view.GetEye() + viewVector, view.GetUp()));
}

void Spectator::Pitch(Camera& camera, const float angle, const int y) const
{
	const View& view = camera.GetView();
	const float rotAngle = angle * (_reference.y - y) * static_cast<float>(M_PI/180.0);
	const tVec3f viewDir(view.GetCenter() - view.GetEye());
	const tVec3f viewVector(vmath::normalize(viewDir * cos(rotAngle)) +	view.GetUp() * sin(rotAngle));

	camera.SetView(View(view.GetEye(), view.GetEye() + viewVector, view.GetUp()));
}

//void Spectator::Roll(Camera& camera, const float angle, const int x) const
//{
//	const View& view = camera.GetView();
//	const float rotAngle = angle * (_reference.x - x) * static_cast<float>(M_PI/180.0);
//	const tVec3f viewDir(view.GetCenter() - view.GetEye());
//	const tVec3f upVector(vmath::normalize(view.GetUp() * cos(rotAngle) + vmath::cross(view.GetUp(), viewDir) * sin(rotAngle)));
//
//	camera.SetView(View(view.GetEye(), view.GetCenter(), upVector));
//}

void Spectator::Roll(Camera& camera, const float angle) const
{
	const View& view = camera.GetView();
	const float rotAngle = angle * static_cast<float>(M_PI/180.0);
	const tVec3f viewDir(view.GetCenter() - view.GetEye());
	const tVec3f upVector(vmath::normalize(view.GetUp() * cos(rotAngle) + vmath::cross(view.GetUp(), viewDir) * sin(rotAngle)));

	camera.SetView(View(view.GetEye(), view.GetCenter(), upVector));
}
