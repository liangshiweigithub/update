#pragma once

#include <vector>
#include <string>

#include "GraphicsEngineExports.h"
#include "SceneGraph/RenderSystem.h"
#include "SceneGraph/RootNode.h"
#include "SceneGraph/Camera.h"
#include "SceneGraph/Light.h"

class GraphicsEngine;

class GRAPHICSENGINE_API Scene
{
public:
	Scene();
	~Scene();

	const RootNode& GetRootNode() const;
	RootNode& GetRootNode();
		
	Camera& GetCamera();
	const Camera& GetCamera() const;

	void AddNode(Node* node);
	void RemoveNode(Node* node);

	void Cull(RenderSystem& renderSystem);
	void Render(RenderSystem& renderSystem) const;

private:
	typedef std::vector<Light*> tLightArray;

private:
	RootNode _rootNode;
	Camera _camera;

	unsigned int _maxLights;

#pragma warning(push)
#pragma warning(disable : 4251)
	tLightArray _lights;
#pragma warning(pop)
};

