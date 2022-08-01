#pragma once

#include <string>
#include "SceneGraphExports.h"
#include "Model.h"

class SCENEGRAPH_API ModelLoader
{
public:
	typedef ModelLoader* (*tRegister)();
	typedef void (*tUnregister)();

	static const std::string RegisterFunction;
	static const std::string UnregisterFunction;

	virtual Model* Load(const std::string& fileName) = 0;
	virtual Model* Load(const std::string& fileName, RenderSystem& renderSystem, float start, float end) = 0;
};