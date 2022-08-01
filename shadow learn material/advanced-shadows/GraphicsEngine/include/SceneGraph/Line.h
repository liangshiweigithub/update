#pragma once

#include "SceneGraphExports.h"

#include "Common/VectorMathTypes.h"

class SCENEGRAPH_API Line
{
public:
	Line(const tVec3f& point, const tVec3f& direction);

	const tVec3f& GetStartPoint() const;
	const tVec3f& GetEndPoint() const;
	tVec3f GetPoint(float parameter) const;
	void SetStartPoint(const tVec3f& point);

	const tVec3f& GetDirection() const;
	void SetDirection(const tVec3f& direction);

	static bool CmpLines(const Line& l0, const Line& l1, float epsilon = 1.0e-6f);

private:
#pragma warning(push)
#pragma warning(disable : 4251)
	tVec3f _startPoint;
	tVec3f _endPoint;
	tVec3f _direction;
#pragma warning(pop)
};