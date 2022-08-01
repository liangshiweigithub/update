#pragma once

#include <vector>

#include "SceneGraphExports.h"
#include "Common/Object.h"
#include "Mesh.h"
#include "Material.h"
#include "Image.h"
#include "ModelNode.h"


class SCENEGRAPH_API Model : public Object
{
public:
	typedef std::vector<Mesh*> tMeshArray;
	typedef std::vector<Material*> tMaterialArray;
	typedef std::vector<Image*> tImageArray;

	Model();
	~Model();

	unsigned int GetMeshCount() const;
	Mesh* GetMesh(const std::string& name) const;
	Mesh* GetMesh(unsigned int index) const;
	void AddMesh(Mesh* mesh);

	Material* GetMaterial(const std::string& name) const;
	void AddMaterial(Material* material);

	unsigned int GetImageCount() const;
	Image* GetImage(const std::string& name) const;
	Image* GetImage(unsigned int index) const;
	void AddImage(Image* image);

	ModelNode* GetModelNode() const;
	void SetModelNode(ModelNode* modelNode);

private:
	ModelNode* _modelNode;

#pragma warning(push)
#pragma warning(disable : 4251)
	tMeshArray _meshArray;
	tMaterialArray _materialArray;
	tImageArray _imageArray;
#pragma warning(pop)

};