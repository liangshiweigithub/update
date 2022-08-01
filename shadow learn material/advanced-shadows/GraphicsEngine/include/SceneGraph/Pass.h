#pragma once

#include <vector>

#include "SceneGraphExports.h"
#include "Node.h"
#include "Model.h"

class SCENEGRAPH_API Pass
{
public:
	typedef std::vector<Node*> tNodeArray;
	virtual ~Pass();

	void AddNode(Node* node);
	void RemoveNode(Node* node);
	void RemoveNodes(Node* node);
	void RemoveModel(Model* model);
	void RemoveAllNodes();

	unsigned int GetNodeCount() const;
	const tNodeArray& GetNodeArray() const;

	virtual void Render() = 0;

protected:
#pragma warning(push)
#pragma warning(disable : 4251)
	tNodeArray _nodes;
#pragma warning(pop)
};