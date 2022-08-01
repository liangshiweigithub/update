#include "stdafx.h"
#include "Transform.h"

Transform::Transform() :
	_matrix(vmath::identity4<float>())
{
}

Transform::Transform(const tMat4f& matrix) :
	_matrix(matrix)
{
}

const tMat4f& Transform::GetMatrix() const
{
	return _matrix;
}

tMat4f& Transform::GetMatrix()
{
	return _matrix;
}

void Transform::SetMatrix(const tMat4f& matrix)
{
	_matrix = matrix;
}

void Transform::Translate(const tVec3f& translation)
{
	_matrix *= vmath::transpose(vmath::translation_matrix(translation));
}

tVec3f Transform::GetTranslation() const
{
	return tVec3f(_matrix.elem[3][0], _matrix.elem[3][1], _matrix.elem[3][2]);
}

void Transform::Rotate(float angle, const tVec3f& rotation)
{
	_matrix = vmath::transpose(vmath::rotation_matrix(angle, rotation)) * _matrix;
}

void Transform::Scale(const tVec3f& scale)
{
	_matrix *= vmath::scaling_matrix(scale);
}

void Transform::Push(RenderSystem& renderSystem) const
{
	renderSystem.Push(*this);
}

void Transform::Pop(RenderSystem& renderSystem) const
{
	renderSystem.Pop(*this);
}

