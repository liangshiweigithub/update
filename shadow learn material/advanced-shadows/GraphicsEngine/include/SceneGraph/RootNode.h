#pragma once

#include "SceneGraphExports.h"
#include "Node.h"

class SCENEGRAPH_API RootNode : public Node
{
public:
	RootNode();

	NodeType GetType() const;
};