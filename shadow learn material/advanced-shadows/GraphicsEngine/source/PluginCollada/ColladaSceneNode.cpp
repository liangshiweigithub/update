#include "stdafx.h"
#include "ColladaSceneNode.h"

#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDEntity.h"

ColladaSceneNode::ColladaSceneNode(FCDSceneNode& sceneNode)
: _sceneNode(&sceneNode)
{
}

ColladaSceneNode::~ColladaSceneNode()
{
}

std::string ColladaSceneNode::GetName() const
{
	return _sceneNode->GetName().c_str();
}

unsigned int ColladaSceneNode::GetChildCount() const
{
	return static_cast<unsigned int>(_sceneNode->GetChildrenCount());
}

ColladaSceneNode ColladaSceneNode::GetChild(unsigned int index) const
{
	return ColladaSceneNode(*_sceneNode->GetChild(index));
}

tMat4f ColladaSceneNode::GetPosition() const
{
	tMat4f matrix;

	for(int i = 0; i < 4; ++i)
		for(int j = 0; j < 4; ++j)
			matrix.elem[i][j] = _sceneNode->ToMatrix().m[i][j];

	return matrix;
}

unsigned int ColladaSceneNode::GetInstanceCount() const
{
	return static_cast<unsigned int>(_sceneNode->GetInstanceCount());
}

FCDEntityInstance* ColladaSceneNode::GetInstance(unsigned int index) const
{
	return _sceneNode->GetInstance(index);
}