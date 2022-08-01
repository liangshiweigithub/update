#pragma once

#include "SceneGraphExports.h"
#include "TransformNode.h"
#include "BoundingBox.h"

class SCENEGRAPH_API GroupNode : public TransformNode
{
public:
	GroupNode();
	GroupNode(BoundingBox& boundingVolume);

	virtual ~GroupNode();

	virtual NodeType GetType() const;

	bool IsCulled() const;
	void SetCulled(bool isCulled);

	BoundingBox* GetBoundingVolume();
	void SetBoundingVolume(BoundingBox& boundingVolume);

	void Cull(RenderSystem& renderSystem);
	void Render(RenderSystem& renderSystem) const;
	virtual void Cull(RenderSystem& renderSystem, unsigned int split);

private:
	bool _isCulled;

	BoundingBox* _boundingVolume;
};
