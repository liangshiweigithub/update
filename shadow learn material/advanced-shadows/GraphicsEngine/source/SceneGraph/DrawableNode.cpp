#include "stdafx.h"
#include "DrawableNode.h"


DrawableNode::DrawableNode(Drawable& drawable) :
	_drawable(&drawable)
{
}

DrawableNode::~DrawableNode()
{
}

Node::NodeType DrawableNode::GetType() const
{
	return Node::eDrawable;
}

const Drawable& DrawableNode::GetDrawable() const
{
	return *_drawable;
}

void DrawableNode::SetDrawable(Drawable& drawable)
{
	_drawable = &drawable;
}

void DrawableNode::Render(RenderSystem& renderSystem) const
{
	_drawable->Render(renderSystem);

	Node::Render(renderSystem);
}