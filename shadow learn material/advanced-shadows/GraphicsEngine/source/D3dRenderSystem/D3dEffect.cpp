#include "stdafx.h"

#include <strsafe.h>
#include <math.h>

#include "D3dEffect.h"

const D3D11_INPUT_ELEMENT_DESC D3dEffect::DefaultInputLayoutDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

const unsigned int D3dEffect::DefaultInputLayoutSize = 3;

D3dEffect::D3dEffect() :
	inputLayoutSize(DefaultInputLayoutSize),
	inputLayoutDesc(0),
	_macros(1),
	_macroDefs(1)
{
	inputLayoutDesc = new D3D11_INPUT_ELEMENT_DESC[inputLayoutSize];

	for (unsigned int i = 0; i < inputLayoutSize; ++i)
		inputLayoutDesc[i] = DefaultInputLayoutDesc[i];

	_macros[0].Name = 0;
	_macros[0].Definition = 0;
}

D3dEffect::~D3dEffect()
{
	DeleteInputLayoutDesc();
}

void D3dEffect::SetMacro(const std::string& name, void* definition, MacroType type)
{
	bool found = false;
	for (tMacroArray::size_type i = 0; i < _macros.size() && !found; ++i)
	{
		if (!strcmp(_macros[i].Name, name.c_str()))
		{
			const unsigned int size = 16;
			char buf[size];
			HRESULT hr = S_OK;
			switch (type)
			{
				case Integer:
					hr = StringCchPrintfA(buf, size, "%d", *static_cast<int*>(definition));
				break;

				case Float:
					hr = StringCchPrintfA(buf, size, "%f", *static_cast<float*>(definition));
				break;

				case Character:
					hr = StringCchPrintfA(buf, size, "%c", *static_cast<char*>(definition));
				break;

				case String:
					hr = StringCchPrintfA(buf, size, "%s", static_cast<char*>(definition));
				break;
			}

			if (SUCCEEDED(hr))
			{
				_macroDefs[i].assign(std::string(buf));				
				_macros[i].Definition = _macroDefs[i].c_str();
			}

			found = true;
		}
	}
}

bool D3dEffect::Initialize(ID3DX11Effect&)
{
	return true;
}

const D3D10_SHADER_MACRO* D3dEffect::GetMacroArray()
{
	return &_macros[0];
}

void D3dEffect::UniformCallBack(D3dEffectManager&, const Material&)
{
}

void D3dEffect::UniformCallBack(D3dEffectManager&, const Texture&, ID3D11ShaderResourceView&, float*)
{
}

void D3dEffect::UniformCallBack(D3dEffectManager&, const Transform&)
{
}

void D3dEffect::AddMacro(const std::string& name)
{
	_macros[_macros.size()-1].Name = name.c_str();
	_macros[_macros.size()-1].Definition = 0;

	// last element has to be NULL
	D3D10_SHADER_MACRO macro = {0, 0};
	_macros.push_back(macro);
	_macroDefs.push_back(std::string(""));
}

void D3dEffect::DeleteInputLayoutDesc()
{
	if (inputLayoutDesc)
	{
		delete[] inputLayoutDesc;
		inputLayoutDesc = 0;
	}
}