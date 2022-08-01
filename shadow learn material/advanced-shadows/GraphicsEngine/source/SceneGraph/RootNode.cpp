#include "stdafx.h"
#include "RootNode.h"

RootNode::RootNode()
{
	Object::SetName("Root");
}

Node::NodeType RootNode::GetType() const
{
	return Node::eRoot;
}
