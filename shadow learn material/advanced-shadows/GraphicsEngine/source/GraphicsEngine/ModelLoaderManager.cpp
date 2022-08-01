#include "stdafx.h"
#include "ModelLoaderManager.h"


/* ModelLoaderInfo */
ModelLoaderManager::ModelLoaderInfo::ModelLoaderInfo(HMODULE nhModule, ModelLoader* nModelLoader)
 :	hModule(nhModule),
	modelLoader(nModelLoader)
{
}


/* ModelLoaderManager */
ModelLoaderManager::ModelLoaderManager()
{
// todo: load plugins from directory
#ifdef _DEBUG
	Register("PluginColladaD.dll");
#else
	Register("PluginCollada.dll");
#endif
}

ModelLoaderManager::~ModelLoaderManager()
{
	UnRegisterAll();
}

Model* ModelLoaderManager::Load(const std::string& fileName) const
{
	Model* model = 0;

	for (unsigned int i = 0; i < _modelLoaders.size() && !model; ++i)
		model = _modelLoaders[i].modelLoader->Load(fileName);

	return model;
}

Model* ModelLoaderManager::Load(const std::string& fileName, RenderSystem& renderSystem, float start, float end) const
{
	Model* model = 0;

	for (unsigned int i = 0; i < _modelLoaders.size() && !model; ++i)
		model = _modelLoaders[i].modelLoader->Load(fileName, renderSystem, start, end);

	return model;
}

void ModelLoaderManager::UnLoad(Model* model) const
{
	if (model)
	{
		delete model;
		model = 0;
	}
}

void ModelLoaderManager::Register(const std::string& fileName)
{
	HMODULE hModule = ::LoadLibrary(fileName.c_str());

	if (hModule)
	{
		ModelLoader::tRegister reg = 
			reinterpret_cast<ModelLoader::tRegister>(::GetProcAddress(hModule, ModelLoader::RegisterFunction.c_str()));

		ModelLoader* modelLoader = 0;

		if (reg)
			modelLoader = reg();

		if (modelLoader)
			_modelLoaders.push_back(ModelLoaderInfo(hModule, modelLoader));
		else
			::FreeLibrary(hModule);
	}
}

void ModelLoaderManager::UnRegisterAll()
{
	for (unsigned int i = 0; i < _modelLoaders.size(); ++i)
	{
		ModelLoader::tUnregister unreg = 
			reinterpret_cast<ModelLoader::tUnregister>(::GetProcAddress(_modelLoaders[i].hModule, ModelLoader::UnregisterFunction.c_str()));

		if (unreg)
			unreg();

		::FreeLibrary(_modelLoaders[i].hModule);
	}

	_modelLoaders.clear();
}