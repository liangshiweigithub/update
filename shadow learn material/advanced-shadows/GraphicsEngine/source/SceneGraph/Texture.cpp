#include "stdafx.h"
#include "Texture.h"

Texture::Texture(Image& image, Texture::Type type,
				 tVec2f tiling, tVec2f offset, float rotation) :
	_image(&image),
	_type(type),
	_tiling(tiling),
	_offset(offset),
	_rotation(rotation)
{
}

Texture::~Texture()
{
}

const Image& Texture::GetImage() const
{
	return *_image;
}

void Texture::SetImage(Image& image)
{
	_image = &image;
}

Texture::Type Texture::GetTextureType() const
{
	return _type;
}

void Texture::SetTextureType(Texture::Type type)
{
	_type = type;
}

const tVec2f& Texture::GetTiling() const
{
	return _tiling;
}

void Texture::SetTiling(tVec2f& tiling)
{
	_tiling = tiling;
}

const tVec2f& Texture::GetOffset() const
{
	return _offset;
}

void Texture::SetOffset(tVec2f& offset)
{
	_offset = offset;
}

const float Texture::GetRotation() const
{
	return _rotation;
}

void Texture::SetRotation(float roation)
{
	_rotation = roation;
}