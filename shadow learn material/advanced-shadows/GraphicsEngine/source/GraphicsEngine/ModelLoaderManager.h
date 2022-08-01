#pragma once

#include <vector>
#include <string>


class ModelLoaderManager : public Singleton<ModelLoaderManager>
{
public:
	friend class Singleton<ModelLoaderManager>;

	~ModelLoaderManager();

	Model* Load(const std::string& fileName) const;
	Model* Load(const std::string& fileName, RenderSystem& renderSystem, float start, float end) const;
	void UnLoad(Model* model) const;

protected:
	struct ModelLoaderInfo
	{
		ModelLoaderInfo(HMODULE nhModule, ModelLoader* nModelLoader);

		HMODULE hModule;
		ModelLoader* modelLoader;
	};

	ModelLoaderManager();

	void Register(const std::string& fileName);
	void UnRegisterAll();

private:

	/* fields */
	std::vector<ModelLoaderInfo> _modelLoaders;
};

