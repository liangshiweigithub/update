#pragma once

#include "SceneGraphExports.h"
#include "GroupNode.h"

class Model;

class SCENEGRAPH_API ModelNode : public GroupNode
{
public:
	ModelNode(Model& model);
	~ModelNode();

	NodeType GetType() const;

private:
	Model* _model;
};