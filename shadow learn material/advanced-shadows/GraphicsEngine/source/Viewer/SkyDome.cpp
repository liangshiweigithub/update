#include "stdafx.h"

#include "SkyDome.h"

/* SkyDome::SkyDomeShader **************************************************************/
const std::string SkyDome::SkyDomeShader::ShaderSource("../shaders/RenderSky.fx");

SkyDome::SkyDomeShader::SkyDomeShader()
{
	SetName("SkyDome::SkyDomeShader");
	SetFilename(ShaderSource, Shader::CompleteSource);
}

bool SkyDome::SkyDomeShader::Initialize(ID3DX11Effect& effect)
{
	if (!effect.GetTechniqueByName("RenderSky")->IsValid())
		return false;

	diffuseTex = effect.GetVariableByName("g_txDiffuse")->AsShaderResource();
	world = effect.GetVariableByName("g_mWorld")->AsMatrix();
	view = effect.GetVariableByName("g_mView")->AsMatrix();
	projection = effect.GetVariableByName("g_mProj")->AsMatrix();
	tex = effect.GetVariableByName("g_mTex")->AsMatrix();

	if (!diffuseTex->IsValid() || !world->IsValid() || !view->IsValid() || !projection->IsValid())
		return false;

	return true;
}

void SkyDome::SkyDomeShader::UniformCallBack(D3dEffectManager&, const Texture& texture, ID3D11ShaderResourceView& resource,  float* texMtx)
{
	switch (texture.GetTextureType())
	{
		case Texture::Diffuse:
			diffuseTex->SetResource(&resource);
			tex->SetMatrix(texMtx);
			break;
		default:
			break;		
	}
}

void SkyDome::SkyDomeShader::UniformCallBack(D3dEffectManager&, const Transform& transform)
{
	world->SetMatrix(const_cast<float*>(&transform.GetMatrix().elem[0][0]));
}

/* SkyDome *****************************************************************************/
SkyDome::SkyDome() :
	_renderSystem(0)	
{
}

SkyDome::~SkyDome()
{
	if (_renderSystem)
		_renderSystem->GetEffectManager()->Delete(_skyDomeShader);
}

bool SkyDome::Initialize(RenderSystem& renderSystem)
{
	bool success = true;

	success &= (_renderSystem = dynamic_cast<D3dRenderSystem*>(&renderSystem)) != 0;

	if (!success)
		return false;

	success &= _renderSystem->GetEffectManager()->Generate(_skyDomeShader, *_renderSystem);

	return success;
}

void SkyDome::Render()
{
	_renderSystem->GetEffectManager()->Use(_skyDomeShader, "RenderSky", 0);

	_skyDomeShader.view->SetMatrix(_renderSystem->GetViewMatrix());
	_skyDomeShader.projection->SetMatrix(_renderSystem->GetProjMatrix());

	for (unsigned int i = 0; i < _nodes.size(); ++i)
		_nodes[i]->Render(*_renderSystem);
}
