#include "stdafx.h"

#include "Pass.h"

Pass::~Pass()
{
	RemoveAllNodes();
}

void Pass::AddNode(Node* node)
{
	if (node)
		_nodes.push_back(node);
}

void Pass::RemoveNode(Node* node)
{
	if (!node)
		return;
	
	for (tNodeArray::iterator i = _nodes.begin(); i != _nodes.end(); i++)
		if ((*i) == node)
		{
			_nodes.erase(i);
			return;
		}
}

void Pass::RemoveModel(Model* model)
{
	if (!model)
		return;
	
	RemoveNodes(model->GetModelNode());
}

void Pass::RemoveNodes(Node* node)
{
	if (!node)
		return;

	RemoveNode(node);

	for (unsigned int i = 0; i < node->GetChildCount(); ++i)
		RemoveNodes(node->GetChild(i));
}

unsigned int Pass::GetNodeCount() const
{
	return static_cast<unsigned int>(_nodes.size());
}

const Pass::tNodeArray& Pass::GetNodeArray() const
{
	return _nodes;
}

void Pass::RemoveAllNodes()
{
	_nodes.clear();
}
