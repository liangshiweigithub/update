#pragma once

#include "SceneGraphExports.h"
#include "Common/Object.h"
#include "RenderSystem.h"

class SCENEGRAPH_API Drawable : public Object
{
public:
	virtual ~Drawable() {}
	virtual void Render(RenderSystem& renderSystem) const = 0;
};