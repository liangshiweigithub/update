#include "stdafx.h"
#include "Model.h"

Model::Model() :
	_modelNode(0)
{
}

Model::~Model()
{
	if (_modelNode)
		delete _modelNode;

	for(tMeshArray::size_type i = 0; i < _meshArray.size(); ++i)
		delete _meshArray[i];

	for(tImageArray::size_type i = 0; i < _imageArray.size(); ++i)
		delete _imageArray[i];

	for(tMaterialArray::size_type i = 0; i < _materialArray.size(); ++i)
		delete _materialArray[i];
}

unsigned int Model::GetMeshCount() const
{
	return static_cast<unsigned int>(_meshArray.size());
}

Mesh* Model::GetMesh(const std::string& name) const
{
	for (unsigned int i = 0; i < _meshArray.size(); ++i)
		if (_meshArray[i]->GetName() == name)
			return _meshArray[i];

	return 0;
}

Mesh* Model::GetMesh(unsigned int index) const
{
	return _meshArray[index];
}

void Model::AddMesh(Mesh* mesh)
{
	if (mesh)
		_meshArray.push_back(mesh);
}

Material* Model::GetMaterial(const std::string& name) const
{
	for (unsigned int i = 0; i < _materialArray.size(); ++i)
		if (_materialArray[i]->GetName() == name)
			return _materialArray[i];

	return 0;
}

void Model::AddMaterial(Material* material)
{
	if(material)
		_materialArray.push_back(material);
}


unsigned int Model::GetImageCount() const
{
	return static_cast<unsigned int>(_imageArray.size());
}

Image* Model::GetImage(const std::string& name) const
{
	for (unsigned int i = 0; i < _imageArray.size(); ++i)
		if (_imageArray[i]->GetName() == name)
			return _imageArray[i];
	
	return 0;
}

Image* Model::GetImage(unsigned int index) const
{
	return _imageArray[index];
}

void Model::AddImage(Image* image)
{
	if(image)
		_imageArray.push_back(image);
}

ModelNode* Model::GetModelNode() const
{
	return _modelNode;
}

void Model::SetModelNode(ModelNode* modelNode)
{
	_modelNode = modelNode;
}