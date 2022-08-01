#include "stdafx.h"
#include "StateNode.h"

StateNode::StateNode(State& state) :
	_state(&state)
{
}

StateNode::~StateNode()
{
}

Node::NodeType StateNode::GetType() const
{
	return Node::eState;
}

const State& StateNode::GetState() const
{
	return *_state;
}

void StateNode::SetState(State& state)
{
	_state = &state;
}

void StateNode::Render(RenderSystem& renderSystem) const
{
	_state->Push(renderSystem);

	Node::Render(renderSystem);

	_state->Pop(renderSystem);
}