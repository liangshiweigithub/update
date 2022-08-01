#pragma once

#include <string>
#include <vector>

#include "GraphicsEngineExports.h"

#include "D3dRenderSystem/D3dRenderSystem.h"
#include "Common/EventHandler.h"
#include "SceneGraph/Pass.h"
#include "Scene.h"

class GRAPHICSENGINE_API GraphicsEngine
{
public:
	typedef std::vector<Model*> tModelArray;
	typedef std::vector<Pass*> tPassArray;

	GraphicsEngine();
	~GraphicsEngine();

	Scene& GetScene();
	D3dRenderSystem* GetRenderSystem();

	bool Initialize();

	Model* LoadModel(const std::string& fileName);
	Model* LoadModel(const std::string& fileName, float start, float end);
	void UnLoadModel(Model* model);

	void AddPass(Pass* pass);

	void RenderScene();

private:
	void DeleteModel(Model* model);
	void DeletePass(Pass* pass);

private:
	D3dRenderSystem* _renderSystem;
	Scene _scene;
	
#pragma warning(push)
#pragma warning(disable : 4251)
	tModelArray _models;
	tPassArray _passes;
#pragma warning(pop)
};

