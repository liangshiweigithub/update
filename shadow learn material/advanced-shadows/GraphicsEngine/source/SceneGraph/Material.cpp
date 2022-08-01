#include "stdafx.h"
#include "Material.h"

Material::Material()
 :	_shininess(.0f),
	_ambient(.2f, .2f, .2f, 1.0f),
	_diffuse(.8f, .8f, .8f, 1.0f),
	_specular(.0f, .0f, .0f, 1.0f),
	_emission(.0f, .0f, .0f, 1.0f)
{
}

Material::~Material()
{
	for(unsigned int i = 0; i < _textureArray.size(); ++i)
		delete _textureArray[i];
}

const tVec4f& Material::GetAmbient() const
{
	return _ambient;
}

void Material::SetAmbient(const tVec4f& ambient)
{
	_ambient = ambient;
}

const tVec4f& Material::GetDiffuse() const
{
	return _diffuse;
}

void Material::SetDiffuse(const tVec4f& diffuse)
{
	_diffuse = diffuse;
}

const tVec4f& Material::GetSpecular() const
{
	return _specular;
}

void Material::SetSpecular(const tVec4f& specular)
{
	_specular = specular;
}

const tVec4f& Material::GetEmission() const
{
	return _emission;
}

void Material::SetEmission(const tVec4f& emission)
{
	_emission = emission;
}

float Material::GetShininess() const
{
	return _shininess;
}

void Material::SetShininess(float shininess)
{
	_shininess = shininess;
}

bool Material::IsTranslucent() const
{
	return (_ambient.w != 1.0f || _diffuse.w != 1.0f || _specular.w != 1.0f);
}

unsigned int Material::GetTextureCount() const
{
	return static_cast<unsigned int>(_textureArray.size());
}

Texture* Material::GetTexture(unsigned int index) const
{
	if (index < _textureArray.size())
		return _textureArray[index];
		
	return 0;
}

void Material::AddTexture(Texture* texture)
{
	if(texture)
		_textureArray.push_back(texture);
}

void Material::Push(RenderSystem& renderSystem) const
{
	renderSystem.Push(*this);
}

void Material::Pop(RenderSystem& renderSystem) const
{
	renderSystem.Pop(*this);
}

