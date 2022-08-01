#include "stdafx.h"
#include "Node.h"
#include "TransformNode.h"

Node::Node() :
	_parent(0)
{
}

Node::Node(Node* parent) :
	_parent(parent)
{
}

Node::~Node()
{
	DeleteAllChilds();
}

Node* Node::Search(Node& node, const std::string& name)
{
	if (node.GetName() == name)
		return &node;

	if (node.GetChildCount() == 0)
		return 0;

	for (unsigned int i = 0; i < node.GetChildCount(); ++i)
	{
		Node* child = node.GetChild(i);
		if (child->GetName() == name)
			return child;
	}

	for (unsigned int i = 0; i < node.GetChildCount(); ++i)
	{
		Node* child = node.GetChild(i);

		if (child->GetChildCount())
		{
			Node* returnNode = Node::Search(*child, name);

			if (returnNode)
				return returnNode;
		}
	}

	return 0;
}

Node* Node::GetParent() const
{
	return _parent;
}

void Node::SetParent(Node* parent)
{
	if(parent)
		_parent = parent;
}

unsigned int Node::GetChildCount() const
{
	return static_cast<unsigned int>(_childs.size());
}

Node* Node::GetChild(unsigned int index) const
{
	return (index < _childs.size() ? _childs[index] : 0);
}

void Node::AddChild(Node* child)
{
	if (child)
		_childs.push_back(child);
}

void Node::RemoveChild(Node* child)
{
	if (!child)
		return;
	
	for (tNodeArray::iterator i = _childs.begin(); i != _childs.end(); i++)
		if ((*i) == child)
		{
			_childs.erase(i);			
			return;
		}	
}

void Node::RemoveAllChilds()
{
	_childs.clear();
}

void Node::DeleteAllChilds()
{
	for (tNodeArray::size_type i = 0; i < _childs.size(); ++i)
	{
		delete _childs[i];
		_childs[i] = 0;
	}

	_childs.clear();
}

void Node::Cull(RenderSystem& renderSystem)
{
	for (unsigned int i = 0; i < _childs.size(); ++i)
		_childs[i]->Cull(renderSystem);
}

void Node::Render(RenderSystem& renderSystem) const
{
	for (unsigned int i = 0; i < _childs.size(); ++i)
		_childs[i]->Render(renderSystem);
}

void Node::Cull(RenderSystem& renderSystem, unsigned int split)
{
	for (unsigned int i = 0; i < _childs.size(); ++i)
		_childs[i]->Cull(renderSystem, split);
}

void Node::Reset()
{
	for (unsigned int i = 0; i < _childs.size(); ++i)
		_childs[i]->Reset();
}