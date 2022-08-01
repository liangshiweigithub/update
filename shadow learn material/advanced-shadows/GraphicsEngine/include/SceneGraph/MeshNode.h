#pragma once

#include "SceneGraphExports.h"

#include "Mesh.h"
#include "GroupNode.h"

class SCENEGRAPH_API MeshNode : public GroupNode
{
public:
	MeshNode(Mesh& mesh);
	~MeshNode();
	
	NodeType GetType() const;
	const Mesh& GetMesh() const;

	void Cull(RenderSystem& rendersystem, unsigned int split);
	void Reset();

private:
	Mesh* _mesh;
};