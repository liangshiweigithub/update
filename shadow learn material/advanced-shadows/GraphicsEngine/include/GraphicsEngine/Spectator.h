#pragma once

#include "SceneGraph/Camera.h"
#include "Common/VectorMathTypes.h"

#include "GraphicsEngineExports.h"

class GRAPHICSENGINE_API Spectator
{
public:
	const tVec2i& GetReferencePoint() const;
	void SetReferencePoint(const tVec2i& point);

	void Move(Camera& camera, const float dist) const;
	void Lift(Camera& camera, const float dist) const;
	void Strafe(Camera& camera, const float dist) const;
	void Yaw(Camera& camera, const float angle, int x) const;
	void Pitch(Camera& camera, const float angle, int y) const;
	//void Roll(Camera& camera, const float angle, int x) const;
	void Roll(Camera& camera, const float angle) const;

private:
#pragma warning(push)
#pragma warning(disable : 4251)
	tVec2i _reference;
#pragma warning(pop)
	
};