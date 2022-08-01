#include "stdafx.h"
#include "ModelNode.h"

ModelNode::ModelNode(Model& model) :
	_model(&model)
{
}

ModelNode::~ModelNode()
{
	//delete _model;
}

Node::NodeType ModelNode::GetType() const
{
	return Node::eModel;
}