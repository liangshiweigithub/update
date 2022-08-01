#pragma once

#include "Common/VectorMathTypes.h"

#include "SceneGraphExports.h"
#include "State.h"

class SCENEGRAPH_API Transform : public State
{
public:
	Transform();
	Transform(const tMat4f& matrix);

	const tMat4f& GetMatrix() const;
	tMat4f& GetMatrix();
	void SetMatrix(const tMat4f& matrix);

	void Translate(const tVec3f& translation);
	tVec3f GetTranslation() const;

	void Rotate(float angle, const tVec3f& rotation);

	void Scale(const tVec3f& scale);

	void Push(RenderSystem& renderSystem) const;
	void Pop(RenderSystem& renderSystem) const;

private:
#pragma warning(push)
#pragma warning(disable : 4251)
	tMat4f _matrix;
#pragma warning(pop)
};