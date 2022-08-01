#pragma once

#include "SceneGraphExports.h"
#include "Node.h"
#include "State.h"

class SCENEGRAPH_API StateNode : public Node
{
public:
	StateNode(State& state);
	virtual ~StateNode();

	virtual NodeType GetType() const;

	const State& GetState() const;
	void SetState(State& state);

	void Render(RenderSystem& renderSystem) const;	

private:
	State* _state;
};