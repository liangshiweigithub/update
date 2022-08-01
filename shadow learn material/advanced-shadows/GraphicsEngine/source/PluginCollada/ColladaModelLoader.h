#pragma once

#include "FCDocument/FCDGeometryInstance.h"

#include "ColladaSceneNode.h"
#include "ColladaLibraryManager.h"

class ColladaModelLoader : public ModelLoader
{
public:
	Model* Load(const std::string& fileName);
	Model* Load(const std::string& fileName, RenderSystem& renderSystem, float start, float end);

private:
	GroupNode* LoadSceneNode(ColladaSceneNode sceneNode, ColladaLibraryManager& manager);

	Node* LoadGeometryInstance(FCDGeometryInstance* geometryInstance, ColladaLibraryManager& manager);

private:
	static Logger logger;
};