#pragma once

#include "SceneGraphExports.h"

class Model;
class Node;
class GroupNode;
class BoundingBox;
class RenderSystem;

class SCENEGRAPH_API Culler
{
public:
	enum Planes
	{
		Left = 0,
		Right,
		Bottom,
		Top,
		Near,
		Far,
		PlaneCount
	};

	Culler();
	virtual ~Culler();

	bool IsEnabled() const;
	void SetIsEnabled(bool enable);

	const BoundingBox* GenerateBoundingVolumes(Model& model);
	const BoundingBox* GenerateBoundingVolumes(Node& node);

	virtual void Cull(GroupNode& groupNode, RenderSystem& renderSystem) = 0;
	
private:
	bool _isEnabled;
};