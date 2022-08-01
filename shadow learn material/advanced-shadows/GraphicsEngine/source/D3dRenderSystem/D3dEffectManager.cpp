#include "stdafx.h"
#include <strsafe.h>

#include "D3dRenderSystem.h"
#include "D3Dcompiler.h"

Logger D3dEffectManager::logger("D3dEffectManager");

D3dEffectManager::D3dEffectManager(ID3D11Device& d3dDevice) :
	_d3dDevice(&d3dDevice),
	_effects(1, 0),
	_inputLayouts(1, 0),
	_activeEffect(0),
	_activeD3dEffect(0),
	_activeD3dInputLayout(0),
	_activeD3dTechnique(0),
	_activeD3dPass(0),
	_effectStack(0),
	_layoutStack(0),
	_techniqueStack(0),
	_passStack(0)
{
}

D3dEffectManager::~D3dEffectManager()
{
	// just to be sure that all ressources will be released
	for (tEffectArray::size_type i = 0; i < _effects.size(); ++i)
		if (_effects[i])
			_effects[i]->Release();

	for (tLayoutArray::size_type i = 0; i < _inputLayouts.size(); ++i)
		if (_inputLayouts[i])
			_inputLayouts[i]->Release();
}

bool D3dEffectManager::Generate(D3dEffect& effect, const RenderSystem& renderSystem)
{
	HRESULT hr = S_OK;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3D11_SHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    shaderFlags |= D3DCOMPILE_DEBUG ;
#endif

	ID3D10Blob*	errors = 0;
	ID3D10Blob* effectblob = 0;

	ID3DX11Effect *d3dEffect = 0;
	
	hr = D3DX11CompileFromFile(effect.GetPath(Shader::CompleteSource).c_str(), effect.GetMacroArray(), 0, 0, "fx_5_0", shaderFlags, 0, 0, &effectblob, &errors, NULL);
	
	if(effectblob != NULL && errors == NULL)
	{
		hr = D3DX11CreateEffectFromMemory(effectblob->GetBufferPointer(), effectblob->GetBufferSize(), 0, _d3dDevice, &d3dEffect);
		effectblob->Release();
	}

	if (errors)
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << static_cast<const char*>(errors->GetBufferPointer());

		errors->Release();

		return false;
	}

	if (FAILED(hr))
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << "Cannot locate " << effect.GetPath(Shader::CompleteSource).c_str() <<
				" for " << effect.GetName() << " (" << renderSystem.GetErrorString(hr) << ")";
		return false;
	}

	ID3DX11EffectPass* pass = d3dEffect->GetTechniqueByIndex(0)->GetPassByIndex(0);
	
//	D3DX11_PASS_SHADER_DESC effectVsDesc;
//g_EffectPass->GetVertexShaderDesc(&effectVsDesc);
//D3DX11_EFFECT_SHADER_DESC effectVsDesc2;
//effectVsDesc.pShaderVariable->GetShaderDesc(effectVsDesc.ShaderIndex, &effectVsDesc2);
//const void *vsCodePtr = effectVsDesc2.pBytecode;
//unsigned vsCodeLen = effectVsDesc2.BytecodeLength;
//
//ID3D11InputLayout *g_InputLayout;
//D3D11_INPUT_ELEMENT_DESC inputDesc[] = { /* ... */ };
//hr = g_Dev->CreateInputLayout(
//  inputDesc, _countof(inputDesc), vsCodePtr, vsCodeLen, &g_InputLayout);


	D3DX11_PASS_DESC passDesc;
	hr = pass->GetDesc(&passDesc);
	
	if (FAILED(hr))
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << "Cannot retrieve pass-desc for " << effect.GetName() <<
			" (" << renderSystem.GetErrorString(hr) << ")";
		return false;
	}

	ID3D11InputLayout* inputLayout = 0;
	hr = _d3dDevice->CreateInputLayout(effect.inputLayoutDesc, effect.inputLayoutSize, passDesc.pIAInputSignature,
									   passDesc.IAInputSignatureSize, &inputLayout);

	if (FAILED(hr))
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << "Cannot create inputlayout for " << effect.GetName() <<
			" (" << renderSystem.GetErrorString(hr) << ")";
		return false;
	}

	if (!effect.Initialize(*d3dEffect))
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << "Failed to initialize D3dEffect " << effect.GetName();
		return false;
	}

	_effects.push_back(d3dEffect);
	_inputLayouts.push_back(inputLayout);
	effect.SetProgramBindId(_effects.size() - 1);

	if (logger.IsEnabled(Logger::Info))
		Logger::Stream(logger, Logger::Info) << "Generate program for " << effect.GetName();

	return true;
}

void D3dEffectManager::Delete(D3dEffect& effect)
{
	unsigned int index = effect.GetProgramBindId();

	if (index < _effects.size() && _effects[index])
	{
		_effects[index]->Release();
		_effects[index] = 0;
	}

	if (index < _inputLayouts.size() && _inputLayouts[index])
	{
		_inputLayouts[index]->Release();
		_inputLayouts[index] = 0;
	}
}

void D3dEffectManager::Use(const D3dEffect& effect, const std::string& technique, const std::string& pass)
{
	_activeEffect = const_cast<D3dEffect*>(&effect);

	unsigned int index = _activeEffect->GetProgramBindId();
	_activeD3dEffect = _effects[index];
	_activeD3dTechnique = _activeD3dEffect->GetTechniqueByName(technique.c_str());
	_activeD3dPass = _activeD3dTechnique->GetPassByName(pass.c_str());
	_activeD3dInputLayout = _inputLayouts[index];
}

void D3dEffectManager::Use(const D3dEffect& effect, const std::string& technique, unsigned int pass)
{
	_activeEffect = const_cast<D3dEffect*>(&effect);

	unsigned int index = _activeEffect->GetProgramBindId();
	_activeD3dEffect = _effects[index];
	_activeD3dTechnique = _activeD3dEffect->GetTechniqueByName(technique.c_str());
	_activeD3dPass = _activeD3dTechnique->GetPassByIndex(pass);
	_activeD3dInputLayout = _inputLayouts[index];
}

void D3dEffectManager::Use(const D3dEffect& effect, unsigned int technique, const std::string& pass)
{
	_activeEffect = const_cast<D3dEffect*>(&effect);

	unsigned int index = _activeEffect->GetProgramBindId();
	_activeD3dEffect = _effects[index];
	_activeD3dTechnique = _activeD3dEffect->GetTechniqueByIndex(technique);
	_activeD3dPass = _activeD3dTechnique->GetPassByName(pass.c_str());
	_activeD3dInputLayout = _inputLayouts[index];
}

void D3dEffectManager::Use(const D3dEffect& effect, unsigned int technique, unsigned int pass)
{
	_activeEffect = const_cast<D3dEffect*>(&effect);

	unsigned int index = _activeEffect->GetProgramBindId();
	_activeD3dEffect = _effects[index];
	_activeD3dTechnique = _activeD3dEffect->GetTechniqueByIndex(technique);
	_activeD3dPass = _activeD3dTechnique->GetPassByIndex(pass);
	_activeD3dInputLayout = _inputLayouts[index];
}

void D3dEffectManager::Use(const std::string& technique, const std::string& pass)
{
	if (!_activeD3dEffect)
		return;

	_activeD3dTechnique = _activeD3dEffect->GetTechniqueByName(technique.c_str());
	_activeD3dPass = _activeD3dTechnique->GetPassByName(pass.c_str());
}

void D3dEffectManager::Use(const std::string& technique, unsigned int pass)
{
	if (!_activeD3dEffect)
		return;

	_activeD3dTechnique = _activeD3dEffect->GetTechniqueByName(technique.c_str());
	_activeD3dPass = _activeD3dTechnique->GetPassByIndex(pass);
}

void D3dEffectManager::Use(unsigned int technique, const std::string& pass)
{
	if (!_activeD3dEffect)
		return;

	_activeD3dTechnique = _activeD3dEffect->GetTechniqueByIndex(technique);
	_activeD3dPass = _activeD3dTechnique->GetPassByName(pass.c_str());
}

void D3dEffectManager::Use(unsigned int technique, unsigned int pass)
{
	if (!_activeD3dEffect)
		return;

	_activeD3dTechnique = _activeD3dEffect->GetTechniqueByIndex(technique);
	_activeD3dPass = _activeD3dTechnique->GetPassByIndex(pass);
}

void D3dEffectManager::SendUniform2iv(const std::string& name, const tVec2i& v0)
{
	if (!_activeD3dEffect)
		return;

	ID3DX11EffectVectorVariable* v = _activeD3dEffect->GetVariableByName(name.c_str())->AsVector();

	if (v->IsValid())
	{
		int vec[2] = {v0.x, v0.y};
		v->SetIntVector(vec);
	}
}

void D3dEffectManager::SendUniform4fv(const std::string& name, const tVec4f& v0)
{
	if (!_activeD3dEffect)
		return;

	ID3DX11EffectVectorVariable* v = _activeD3dEffect->GetVariableByName(name.c_str())->AsVector();

	if (v->IsValid())
	{
		D3DXVECTOR4 vec(v0.x, v0.y, v0.z, v0.w);
		v->SetFloatVector(vec);
	}
}

void D3dEffectManager::SendUniformMat4f(const std::string& name, float* m0)
{
	if (!_activeD3dEffect)
		return;

	ID3DX11EffectMatrixVariable* v = _activeD3dEffect->GetVariableByName(name.c_str())->AsMatrix();

	if (v->IsValid())
	{
		v->SetMatrix(m0);
	}
}

void D3dEffectManager::Push()
{
	_effectStack.push_back(_activeD3dEffect);
	_layoutStack.push_back(_activeD3dInputLayout);
	_techniqueStack.push_back(_activeD3dTechnique);
	_passStack.push_back(_activeD3dPass);
}

void D3dEffectManager::Pop()
{
	_activeD3dEffect = _effectStack.back();
	_activeD3dInputLayout = _layoutStack.back();
	_activeD3dTechnique = _techniqueStack.back();
	_activeD3dPass = _passStack.back();

	_effectStack.pop_back();
	_layoutStack.pop_back();
	_techniqueStack.pop_back();
	_passStack.pop_back();
}

void D3dEffectManager::UniformCallBack(const Material& material)
{
	if (_activeEffect)
		_activeEffect->UniformCallBack(*this, material);
}

void D3dEffectManager::UniformCallBack(const Texture& texture, ID3D11ShaderResourceView& resource, float* texMtx)
{
	if (_activeEffect)
		_activeEffect->UniformCallBack(*this, texture, resource, texMtx);
}

void D3dEffectManager::UniformCallBack(const Transform& transform)
{
	if (_activeEffect)
		_activeEffect->UniformCallBack(*this, transform);
}

D3dEffect* D3dEffectManager::GetActiveEffect() const
{
	return _activeEffect;
}

ID3DX11Effect* D3dEffectManager::GetActiveD3dEffect() const
{
	return _activeD3dEffect;
}

ID3DX11EffectTechnique* D3dEffectManager::GetActiveD3dTechnique() const
{
	return _activeD3dTechnique;
}

ID3DX11EffectPass* D3dEffectManager::GetActiveD3dPass() const
{
	return _activeD3dPass;
}

ID3D11InputLayout* D3dEffectManager::GetActiveD3dInputLayout() const
{
	return _activeD3dInputLayout;
}