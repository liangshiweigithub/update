#include "stdafx.h"
#include "GraphicsEngine.h"
#include "ModelLoaderManager.h"

GraphicsEngine::GraphicsEngine() :
	_renderSystem(0)
{
}

GraphicsEngine::~GraphicsEngine()
{
	for (tModelArray::size_type i = 0; i < _models.size(); ++i)
		DeleteModel(_models[i]);

	for (tPassArray::size_type i = 0; i < _passes.size(); ++i)
		DeletePass(_passes[i]);

	if (_renderSystem)
		delete _renderSystem;
}

Scene& GraphicsEngine::GetScene()
{
	return _scene;
}

D3dRenderSystem* GraphicsEngine::GetRenderSystem()
{
	return _renderSystem;
}

bool GraphicsEngine::Initialize()
{
	_renderSystem = new D3dRenderSystem();

	return true;
}

Model* GraphicsEngine::LoadModel(const std::string& fileName, float start, float end)
{
	Model* model = ModelLoaderManager::Instance().Load(fileName, *_renderSystem, start, 0.98f * end);

	if (model)
	{
		_renderSystem->GenerateRessources(*model);
		_scene.AddNode(model->GetModelNode());
		_models.push_back(model);

		_renderSystem->DrawStatus(static_cast<unsigned int>(end));
	}

	return model;
}

Model* GraphicsEngine::LoadModel(const std::string& fileName)
{
	Model* model = ModelLoaderManager::Instance().Load(fileName);

	if (model)
	{
		_renderSystem->GenerateRessources(*model);
		_scene.AddNode(model->GetModelNode());
		_models.push_back(model);
	}

	return model;
}

void GraphicsEngine::UnLoadModel(Model* model)
{
	if (!model)
		return;

	tModelArray::iterator i;
	for (i = _models.begin(); i != _models.end() && (*i) != model; i++);

	if ((*i) == model)
		_models.erase(i);

	DeleteModel(model);
}

void GraphicsEngine::DeleteModel(Model* model)
{
	if (!model)
		return;

	for (tPassArray::size_type i = 0; i < _passes.size(); ++i)
		_passes[i]->RemoveModel(model);

	_scene.RemoveNode(model->GetModelNode());
	_renderSystem->DeleteRessources(*model);

	ModelLoaderManager::Instance().UnLoad(model);
}

void GraphicsEngine::RenderScene()
{
	for (tPassArray::size_type i = 0; i < _passes.size(); ++i)
	{
		_renderSystem->SetActivePass(_passes[i]);
		_passes[i]->Render();
	}

	_renderSystem->CheckError();
}

void GraphicsEngine::AddPass(Pass* pass)
{
	if (pass)
		_passes.push_back(pass);
}

void GraphicsEngine::DeletePass(Pass* pass)
{
	if (pass)
	{
		delete pass;
		pass = 0;
	}
}

