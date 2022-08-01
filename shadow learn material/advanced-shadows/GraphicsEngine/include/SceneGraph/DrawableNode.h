#pragma once

#include "SceneGraphExports.h"
#include "Node.h"
#include "Drawable.h"

class SCENEGRAPH_API DrawableNode : public Node
{
public:
	DrawableNode(Drawable& drawable);
	
	virtual ~DrawableNode();

	virtual NodeType GetType() const;

	const Drawable& GetDrawable() const;
	void SetDrawable(Drawable& drawable);

	virtual void Render(RenderSystem& renderSystem) const;

private:
	Drawable* _drawable;
};