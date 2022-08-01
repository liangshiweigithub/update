#include "stdafx.h"
#include "Scene.h"

Scene::Scene() :
	_maxLights(4)
{
}

Scene::~Scene()
{
	for (tLightArray::size_type i = 0; i < _lights.size(); ++i)
		delete _lights[i];
}

const RootNode& Scene::GetRootNode() const
{
	return _rootNode;
}

RootNode& Scene::GetRootNode()
{
	return _rootNode;
}

Camera& Scene::GetCamera()
{
	return _camera;
}

const Camera& Scene::GetCamera() const
{
	return _camera;
}

void Scene::AddNode(Node* node)
{
	if (node)
		_rootNode.AddChild(node);
}

void Scene::RemoveNode(Node* node)
{
	if (node)
		_rootNode.RemoveChild(node);
}

void Scene::Cull(RenderSystem& renderSystem)
{
	_rootNode.Cull(renderSystem);
}

void Scene::Render(RenderSystem& renderSystem) const
{
	_rootNode.Render(renderSystem);
}