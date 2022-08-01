#pragma once

#include <string>
#include "SceneGraphExports.h"
#include "Common/Object.h"
#include "RenderSystem.h"

class SCENEGRAPH_API State : public Object
{
public:
	virtual void Push(RenderSystem& renderSystem) const = 0;
	virtual void Pop(RenderSystem& renderSystem) const = 0;	
};