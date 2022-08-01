#pragma once

#include "D3dRenderSystemExports.h"

#include "Direct3d/d3dx11.h"
#include "Effects11/d3dx11effect.h"

class D3dEffect;
class D3dRenderSystem;

class D3DRENDERSYSTEM_API D3dEffectManager
{
public:
	typedef std::vector<ID3DX11Effect*> tEffectArray;
	typedef std::vector<ID3D11InputLayout*> tLayoutArray;
	typedef std::vector<ID3DX11EffectTechnique*> tTechniqueArray;
	typedef std::vector<ID3DX11EffectPass*> tPassArray;

	D3dEffectManager(ID3D11Device& d3dDevice);
	~D3dEffectManager();

	bool Generate(D3dEffect& effect, const RenderSystem& renderSystem);
	void Delete(D3dEffect& effect);

	void Use(const D3dEffect& effect, const std::string& technique, const std::string& pass);
	void Use(const D3dEffect& effect, const std::string& technique, unsigned int pass);
	void Use(const D3dEffect& effect, unsigned int technique, const std::string& pass);
	void Use(const D3dEffect& effect, unsigned int technique, unsigned int pass);
	void Use(const std::string& technique, const std::string& pass);
	void Use(const std::string& technique, unsigned int pass);
	void Use(unsigned int technique, const std::string& pass);
	void Use(unsigned int technique, unsigned int pass);

	void SendUniform2iv(const std::string& name, const tVec2i& v0);
	void SendUniform4fv(const std::string& name, const tVec4f& v0);
	void SendUniformMat4f(const std::string& name, float* m0);

	void Push();
	void Pop();

	void UniformCallBack(const Material& material);
	void UniformCallBack(const Texture& texture, ID3D11ShaderResourceView& resource, float* texMtx);
	void UniformCallBack(const Transform& transform);

	D3dEffect* GetActiveEffect() const;
	ID3DX11Effect* GetActiveD3dEffect() const;
	ID3DX11EffectTechnique* GetActiveD3dTechnique() const;
	ID3DX11EffectPass* GetActiveD3dPass() const;
	ID3D11InputLayout* GetActiveD3dInputLayout() const;

private:
	static Logger logger;

	ID3D11Device* _d3dDevice;

	D3dEffect* _activeEffect;
	ID3DX11Effect* _activeD3dEffect;
	ID3DX11EffectTechnique* _activeD3dTechnique;
	ID3DX11EffectPass* _activeD3dPass;
	ID3D11InputLayout* _activeD3dInputLayout;

#pragma warning(push)
#pragma warning(disable : 4251)
	tEffectArray _effects;
	tLayoutArray _inputLayouts;

	tEffectArray _effectStack;
	tLayoutArray _layoutStack;
	tTechniqueArray _techniqueStack;
	tPassArray _passStack;
#pragma warning(pop)
	
};