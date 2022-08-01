#include "stdafx.h"
#include "TransformNode.h"

TransformNode::TransformNode() :
	StateNode(_transform)
{
}

TransformNode::TransformNode(const Transform& transform) :
	_transform(transform),
	StateNode(_transform)
{
}

TransformNode::~TransformNode()
{
}

Node::NodeType TransformNode::GetType() const
{
	return Node::eTransform;
}

const Transform& TransformNode::GetTransform() const
{
	return _transform;
}

Transform& TransformNode::GetTransform()
{
	return _transform;
}
