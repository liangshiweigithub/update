#include "stdafx.h"

#include "MeshNode.h"

MeshNode::MeshNode(Mesh& mesh) :
	_mesh(&mesh),
	GroupNode(*new BoundingBox(mesh.GetBoundingVolume()))
{
}

MeshNode::~MeshNode()
{
}

Node::NodeType MeshNode::GetType() const
{
	return Node::eMesh;
}

const Mesh& MeshNode::GetMesh() const
{
	return *_mesh;
}

void MeshNode::Cull(RenderSystem& renderSystem, unsigned int split)
{
	GetState().Push(renderSystem);

	renderSystem.GetCuller().Cull(*this, renderSystem);

	if (!IsCulled())
	{
		_mesh->AddSplit(split);
		Node::Cull(renderSystem, split);
	}

	GetState().Pop(renderSystem);
}

void MeshNode::Reset()
{
	_mesh->ResetSplits();
	Node::Reset();
}