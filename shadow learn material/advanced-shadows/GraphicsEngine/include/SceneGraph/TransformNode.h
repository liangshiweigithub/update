#pragma once

#include "SceneGraphExports.h"
#include "StateNode.h"
#include "Transform.h"

class SCENEGRAPH_API TransformNode : public StateNode
{
public:
	TransformNode();
	TransformNode(const Transform& transform);
	
	virtual ~TransformNode();

	virtual NodeType GetType() const;

	const Transform& GetTransform() const;
	Transform& GetTransform();

private:
	Transform _transform;
};