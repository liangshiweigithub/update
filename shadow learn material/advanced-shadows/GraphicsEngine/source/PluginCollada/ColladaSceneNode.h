#pragma once

#include "FCDocument/FCDSceneNode.h"

class ColladaSceneNode
{
public:
	ColladaSceneNode(FCDSceneNode& sceneNode);
	~ColladaSceneNode();

	std::string GetName() const;

	unsigned int GetChildCount() const;
	ColladaSceneNode GetChild(unsigned int index) const;

	tMat4f GetPosition() const;

	unsigned int GetInstanceCount() const;
	FCDEntityInstance* GetInstance(unsigned int index) const;
	
private:
	FCDSceneNode* _sceneNode;

};