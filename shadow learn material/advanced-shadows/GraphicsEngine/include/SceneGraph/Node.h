#pragma once

#include <vector>

#include "SceneGraphExports.h"
#include "Common/Object.h"
#include "RenderSystem.h"
#include "Culler.h"

class SCENEGRAPH_API Node : public Object
{
public:
	enum NodeType
	{
		eRoot = 0,
		eDrawable,
		eState,
		eGroup,
		eMesh,
		eTransform,
		eModel,
		COUNT
	};

	typedef std::vector<Node*> tNodeArray;

	Node();
	Node(Node* parent);

	virtual ~Node();

	static Node* Search(Node& node, const std::string& name);

	Node* GetParent() const;
	void SetParent(Node* parent);

	unsigned int GetChildCount() const;
	Node* GetChild(unsigned int index) const;

	void AddChild(Node* child);
	void RemoveChild(Node* child);
	void RemoveAllChilds();

	virtual NodeType GetType() const = 0;

	virtual void Cull(RenderSystem& renderSystem);
	virtual void Cull(RenderSystem& renderSystem, unsigned int split);
	virtual void Render(RenderSystem& renderSystem) const;
	virtual void Reset();

private:
	/* methods */
	void DeleteAllChilds();

	/* fields */
	Node* _parent;

#pragma warning(push)
#pragma warning(disable : 4251)
	tNodeArray _childs;
#pragma warning(pop)
};