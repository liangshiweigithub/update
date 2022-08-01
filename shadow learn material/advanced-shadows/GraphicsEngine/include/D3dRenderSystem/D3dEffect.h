#pragma once

#include "D3dRenderSystemExports.h"

#include "SceneGraph/Shader.h"

#include "Direct3d/d3dx11.h"

#include "Effects11/d3dx11effect.h"

class D3DRENDERSYSTEM_API D3dEffect : public Shader
{
friend class D3dEffectManager;

public:
	enum MacroType
	{
		Integer = 0,
		Float,
		Character,
		String
	};

	D3dEffect();
	virtual ~D3dEffect();

	void SetMacro(const std::string& name, void* definition, MacroType type);

	virtual void UniformCallBack(D3dEffectManager& effectManager, const Material& material);
	virtual void UniformCallBack(D3dEffectManager& effectManager, const Texture& texture, ID3D11ShaderResourceView& resource, float* texMtx);
	virtual void UniformCallBack(D3dEffectManager& effectManager, const Transform& transform);

protected:
	typedef std::vector<D3D10_SHADER_MACRO> tMacroArray;
	typedef std::vector<std::string> tStringArray;

	void AddMacro(const std::string& name);
	void DeleteInputLayoutDesc();

private:
	virtual bool Initialize(ID3DX11Effect& effect);
	const D3D10_SHADER_MACRO* GetMacroArray();

protected:
	D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc;
	unsigned int inputLayoutSize;

private:
	static const D3D11_INPUT_ELEMENT_DESC DefaultInputLayoutDesc[];
	static const unsigned int DefaultInputLayoutSize;


#pragma warning(push)
#pragma warning(disable : 4251)
	tMacroArray _macros;
	tStringArray _macroDefs;
#pragma warning(pop)

};