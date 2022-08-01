#include "stdafx.h"
#include "GroupNode.h"

GroupNode::GroupNode() :
	_isCulled(false),
	_boundingVolume(0)
{
}

GroupNode::GroupNode(BoundingBox& boundingVolume) :
	_isCulled(false),
	_boundingVolume(&boundingVolume)
{
}

GroupNode::~GroupNode()
{
	if (_boundingVolume)
		delete _boundingVolume;
}

Node::NodeType GroupNode::GetType() const
{
	return Node::eGroup;
}

bool GroupNode::IsCulled() const
{
	return _isCulled;
}

void GroupNode::SetCulled(bool isCulled)
{
	_isCulled = isCulled;
}

BoundingBox* GroupNode::GetBoundingVolume()
{
	return _boundingVolume;
}

void GroupNode::SetBoundingVolume(BoundingBox& boundingVolume)
{
	if (_boundingVolume)
		delete _boundingVolume;

	_boundingVolume = &boundingVolume;
}

void GroupNode::Cull(RenderSystem& renderSystem)
{
	GetState().Push(renderSystem);

	renderSystem.GetCuller().Cull(*this, renderSystem);

	if (!_isCulled)
		Node::Cull(renderSystem);

	GetState().Pop(renderSystem);
}

void GroupNode::Cull(RenderSystem& renderSystem, unsigned int split)
{
	GetState().Push(renderSystem);

	renderSystem.GetCuller().Cull(*this, renderSystem);

	if (!_isCulled)
		Node::Cull(renderSystem, split);

	GetState().Pop(renderSystem);
}

void GroupNode::Render(RenderSystem& renderSystem) const
{
	//if (!_isCulled)
	//	TransformNode::Render(renderSystem);

	if (_isCulled)
		return;

	GetState().Push(renderSystem);

	if (_boundingVolume && renderSystem.GetDrawBoundingBoxes())
	{
		RenderSystem::PolygonMode mode = renderSystem.GetPolygonMode();
		renderSystem.SetPolygonMode(RenderSystem::Wireframe);
		renderSystem.Draw(*_boundingVolume, tVec4f(.0f, .0f, .0f, 1.0f));
		renderSystem.SetPolygonMode(mode);
	}

	Node::Render(renderSystem);
	
	GetState().Pop(renderSystem);
}