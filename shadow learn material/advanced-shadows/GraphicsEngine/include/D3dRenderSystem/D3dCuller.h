#pragma once

#include "D3dRenderSystemExports.h"

#include "SceneGraph/Culler.h"
#include "SceneGraph/RenderSystem.h"

class D3DRENDERSYSTEM_API D3dCuller : public Culler
{
public:
	void Cull(GroupNode& groupNode, RenderSystem& renderSystem);
};