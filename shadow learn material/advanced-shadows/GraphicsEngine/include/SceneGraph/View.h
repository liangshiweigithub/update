#pragma once

#include "Common/VectorMathTypes.h"
#include "SceneGraphExports.h"

class SCENEGRAPH_API View
{
public:
	View(const tVec3f& eye, const tVec3f& center, const tVec3f& up);

	const tVec3f& GetEye() const;
	void SetEye(const tVec3f& eye);

	const tVec3f& GetCenter() const;
	void SetCenter(const tVec3f& center);

	const tVec3f& GetUp() const;
	void SetUp(const tVec3f& up);

private:
#pragma warning(push)
#pragma warning(disable : 4251)
	tVec3f _eye;
	tVec3f _center;
	tVec3f _up;
#pragma warning(pop)
};
