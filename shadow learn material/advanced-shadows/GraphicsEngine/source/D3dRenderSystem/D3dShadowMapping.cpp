#include "stdafx.h"

#include "D3dShadowMapping.h"
#include "D3dRenderSystem.h"
#include <math.h>

/* D3dShadowMapping::GenDepthMapEffect *************************************************/
const std::string D3dShadowMapping::GenDepthMapEffect::GenDepthMapSourceFile("../shaders/GenDepthMap.fx");
const std::string D3dShadowMapping::GenDepthMapEffect::NbrOfSplitsMacro("NBROFSPLITS");

D3dShadowMapping::GenDepthMapEffect::GenDepthMapEffect(const std::string& name)
{
	SetName(name);
	AddMacro(NbrOfSplitsMacro);
}

bool D3dShadowMapping::GenDepthMapEffect::Initialize(ID3DX11Effect& effect)
{
	if (!effect.GetTechniqueByName("RenderHWSMDepthMap")->IsValid() ||
		!effect.GetTechniqueByName("RenderVSMDepthMap")->IsValid() ||
		!effect.GetTechniqueByName("RenderESMDepthMap")->IsValid() ||
		!effect.GetTechniqueByName("RenderEVSMDepthMap")->IsValid() ||
		!effect.GetTechniqueByName("RenderDistrSAVSMDepthMap")->IsValid() ||
		!effect.GetTechniqueByName("RenderUintSAVSMDepthMap")->IsValid())
		return false;

	diffuseTex = effect.GetVariableByName("texDiffuse")->AsShaderResource();
	world = effect.GetVariableByName("g_mWorld")->AsMatrix();
	view = effect.GetVariableByName("g_mView")->AsMatrix();
	projection = effect.GetVariableByName("g_mProj")->AsMatrix();
	tex = effect.GetVariableByName("g_mDiffuseTex")->AsMatrix();
	textured = effect.GetVariableByName("textured")->AsScalar();
	matDiffuse = effect.GetVariableByName("matDiffuse")->AsVector();
	viewProj = effect.GetVariableByName("g_mLightViewProj")->AsMatrix();
	crop = effect.GetVariableByName("g_mCrop")->AsMatrix();
	liSP = effect.GetVariableByName("g_mLiSP")->AsMatrix();
	firstSplit = effect.GetVariableByName("g_firstSplit")->AsScalar();
	lastSplit = effect.GetVariableByName("g_lastSplit")->AsScalar();
	evsmconst = effect.GetVariableByName("g_EVSMConst")->AsVector();
	
	if (!diffuseTex->IsValid() || !world->IsValid() || !view->IsValid() || !projection->IsValid() ||
		!textured->IsValid() || !matDiffuse->IsValid() || !viewProj->IsValid() || !crop->IsValid() ||
		!liSP->IsValid() || !firstSplit->IsValid() || !lastSplit->IsValid() || !evsmconst->IsValid())
		return false;

	return true;
}

void D3dShadowMapping::GenDepthMapEffect::UniformCallBack(D3dEffectManager&, const Material& material)
{
	const tVec4f& diffuse = material.GetDiffuse();

	matDiffuse->SetFloatVector( (float*)D3DXVECTOR4( diffuse.x, diffuse.y, diffuse.z, diffuse.w ) );

	if (!material.GetTextureCount())
		textured->SetBool(false);
}

void D3dShadowMapping::GenDepthMapEffect::UniformCallBack(D3dEffectManager&, const Texture& texture, ID3D11ShaderResourceView& resource, float* texMtx)
{
	switch (texture.GetTextureType())
	{
		case Texture::Diffuse:
			diffuseTex->SetResource(&resource);
			tex->SetMatrix(texMtx);
			textured->SetBool(true);
			break;
		default:
			textured->SetBool(false);
			break;		
	}
}

void D3dShadowMapping::GenDepthMapEffect::UniformCallBack(D3dEffectManager&, const Transform& transform)
{
	world->SetMatrix(const_cast<float*>(&transform.GetMatrix().elem[0][0]));
}

/* D3dShadowMapping::DispDepthMapEffect ************************************************/
const std::string D3dShadowMapping::DispDepthMapEffect::DispDepthMapSourceFile("../shaders/DisplayMap.fx");

D3dShadowMapping::DispDepthMapEffect::DispDepthMapEffect(const std::string& name)
{
	SetName(name);
}

bool D3dShadowMapping::DispDepthMapEffect::Initialize(ID3DX11Effect& effect)
{
	if (!effect.GetTechniqueByName("DisplayDepthMap")->IsValid() ||
		!effect.GetTechniqueByName("DisplayDepthMapArray")->IsValid() ||
		!effect.GetTechniqueByName("DisplayTexMap")->IsValid() ||
		!effect.GetTechniqueByName("DisplayTexMapArray")->IsValid() ||
		!effect.GetTechniqueByName("DisplayESMDepthMap")->IsValid() ||
		!effect.GetTechniqueByName("GenMipmap")->IsValid() ||
		!effect.GetTechniqueByName("DisplaySAVSMDepthMap")->IsValid() ||
		!effect.GetTechniqueByName("DisplayDistrSAVSMDepthMap")->IsValid() ||
		!effect.GetTechniqueByName("DisplayUintSAVSMDepthMap")->IsValid())
		return false;

	depthMap = effect.GetVariableByName("depthMap")->AsShaderResource();
	idepthMap = effect.GetVariableByName("idepthMap")->AsShaderResource();
	depthMapArray = effect.GetVariableByName("depthMapArray")->AsShaderResource();
	texMap = effect.GetVariableByName("texMap")->AsShaderResource();
	texMapArray = effect.GetVariableByName("texMapArray")->AsShaderResource();
	arrayIndex = effect.GetVariableByName("g_arrayIndex")->AsScalar();
	mapSize = effect.GetVariableByName("g_mapSize")->AsVector();
	tex = effect.GetVariableByName("g_mTex")->AsMatrix();
	texBounds = effect.GetVariableByName("g_texBounds")->AsVector();
	evsmconst = effect.GetVariableByName("g_EVSMConst")->AsVector();
	filterWidth = effect.GetVariableByName("g_filterWidth")->AsScalar();

	if (!depthMap->IsValid() || !depthMapArray->IsValid() || !texMap->IsValid() ||
		!texMapArray->IsValid() || !arrayIndex->IsValid() || !mapSize->IsValid() || 
		!tex->IsValid() || !texBounds->IsValid() || !evsmconst->IsValid() || !filterWidth->IsValid())
		return false;

	return true;
}

/* D3dShadowMapping::BlurEffect ********************************************************/
const std::string D3dShadowMapping::BlurEffect::BlurSourceFile("../shaders/PSPostProcess.fx");

D3dShadowMapping::BlurEffect::BlurEffect(const std::string& name)
{
	SetName(name);
}

bool D3dShadowMapping::BlurEffect::Initialize(ID3DX11Effect& effect)
{
	if (!effect.GetTechniqueByName("BoxBlur")->IsValid())
		return false;

	map = effect.GetVariableByName("map")->AsShaderResource();
	size = effect.GetVariableByName("g_SourceSize")->AsVector();
	dim = effect.GetVariableByName("g_BlurDim")->AsVector();
	samples = effect.GetVariableByName("g_BlurSamples")->AsScalar();
	tex = effect.GetVariableByName("g_mTex")->AsMatrix();
	texBounds = effect.GetVariableByName("g_texBounds")->AsVector();
	gaussianWeights = effect.GetVariableByName("g_Weights")->AsScalar();

	if (!map->IsValid() || !size->IsValid() || !dim->IsValid() || !gaussianWeights->IsValid() ||
		!samples->IsValid() || !tex->IsValid() || !texBounds->IsValid())
		return false;

	return true;
}

/* D3dShadowMapping::GenSATEffect ********************************************************/
const std::string D3dShadowMapping::GenSATEffect::SATSourceFile("../shaders/GenSAT.fx");

D3dShadowMapping::GenSATEffect::GenSATEffect(const std::string& name)
{
	SetName(name);
}

bool D3dShadowMapping::GenSATEffect::Initialize(ID3DX11Effect& effect)
{
	if (!effect.GetTechniqueByName("GenerateSAT")->IsValid())
		return false;

	map = effect.GetVariableByName("map")->AsShaderResource();
	imap = effect.GetVariableByName("imap")->AsShaderResource();
	texwidth = effect.GetVariableByName("g_texWidth")->AsScalar();
	texheight = effect.GetVariableByName("g_texHeight")->AsScalar();
	offset = effect.GetVariableByName("g_offset")->AsScalar();
	samplesPerPass = effect.GetVariableByName("g_samples")->AsScalar();
	tex = effect.GetVariableByName("g_mTex")->AsMatrix();
	texBounds = effect.GetVariableByName("g_texBounds")->AsVector();

	if (!map->IsValid() || !texwidth->IsValid() || !texheight->IsValid() ||
		!offset->IsValid() || !samplesPerPass->IsValid() || !tex->IsValid() || !texBounds->IsValid())
		return false;

	return true;
}

/* D3dShadowMapping ********************************************************************/
const unsigned int D3dShadowMapping::MaxNbrOfSplits = 16;
const unsigned int D3dShadowMapping::NbrOfSamples = 200;

const tVec4f D3dShadowMapping::UV4[4] = 
{
	tVec4f(.0f, .0f,  .5f,  .5f),
	tVec4f(.5f, .0f, 1.0f,  .5f),
	tVec4f(.0f, .5f,  .5f, 1.0f),
	tVec4f(.5f, .5f, 1.0f, 1.0f)
};

const tVec4f D3dShadowMapping::UV9[9] = 
{
	tVec4f(      .0f,       .0f, 1.0f/3.0f, 1.0f/3.0f),
	tVec4f(1.0f/3.0f,       .0f, 2.0f/3.0f, 1.0f/3.0f),
	tVec4f(2.0f/3.0f,       .0f,      1.0f, 1.0f/3.0f),
	tVec4f(      .0f, 1.0f/3.0f, 1.0f/3.0f, 2.0f/3.0f),
	tVec4f(1.0f/3.0f, 1.0f/3.0f, 2.0f/3.0f, 2.0f/3.0f),
	tVec4f(2.0f/3.0f, 1.0f/3.0f,      1.0f, 2.0f/3.0f),
	tVec4f(      .0f, 2.0f/3.0f, 1.0f/3.0f,      1.0f),
	tVec4f(1.0f/3.0f, 2.0f/3.0f, 2.0f/3.0f,      1.0f),
	tVec4f(2.0f/3.0f, 2.0f/3.0f,      1.0f,      1.0f)
};

const tVec4f D3dShadowMapping::UV16[16] = 
{
	tVec4f( .0f,  .0f, .25f, .25f),
	tVec4f(.25f,  .0f,  .5f, .25f),
	tVec4f( .5f,  .0f, .75f, .25f),
	tVec4f(.75f,  .0f, 1.0f, .25f),
	tVec4f( .0f, .25f, .25f,  .5f),
	tVec4f(.25f, .25f,  .5f,  .5f),
	tVec4f( .5f, .25f, .75f,  .5f),
	tVec4f(.75f, .25f, 1.0f,  .5f),
	tVec4f( .0f,  .5f, .25f, .75f),
	tVec4f(.25f,  .5f,  .5f, .75f),
	tVec4f( .5f,  .5f, .75f, .75f),
	tVec4f(.75f,  .5f, 1.0f, .75f),
	tVec4f( .0f, .75f, .25f, 1.0f),
	tVec4f(.25f, .75f,  .5f, 1.0f),
	tVec4f( .5f, .75f, .75f, 1.0f),
	tVec4f(.75f, .75f, 1.0f, 1.0f)
};

D3dShadowMapping::D3dShadowMapping() :
	_renderSystem(0),
	_lView(tVec3f(.0f, 100.0f, .0f), tVec3f(.0f, .0f, .0f), tVec3f(1.0f, .0f, .0f)),
	_lProj(0),
	_sceneId(0),
	_useLiSP(false),
	_updateLightUp(false),
	_reduceSwimming(false),
	_lambda(.5f),
	_genSMEffect("D3dShadowMapping::GenDepthMapEffect"),
	_dispSMEffect("D3dShadowMapping::DispDepthMapEffect"),
	_blurSMEffect("D3dShadowMapping::BlurEffect"),
	_genSATEffect("D3dShadowMapping::GenSATEffect"),
	_filterMode(None),
	_nbrOfSplits(1),
	_shadowMapSz(512),
	_liSP(new LiSP(1)),
	_useGS(false),
	_debug(false),
	_blurRadius(.0f),
	_blurSplitCor(1.0f),
	_lightRadius(0.1f),
	_lightRadiusSplitCor(1.0f),
	_focusing(false),
	_useMinDepth(false),
	_pseudoNear(.0f),
	_pseudoFar(.0f),
	_errVals(ErrDirCOUNT),
	_errRanges(ErrDirCOUNT),
	_UV(1),
	_useBBSel(false),
	_calcError(false),
	_splitErrorSmples(1),
	_errPseudoNF(2),
	_samplesPerPassSat(2),
	_logblur(false),
	_evsmConst(tVec2f(40.0,20.0)),
	_mipmapped(false)
{
	for (unsigned int i = 0; i < ErrDirCOUNT; ++i)
		_errVals[i].resize(NbrOfSamples);
}

D3dShadowMapping::~D3dShadowMapping()
{
	if (_lProj)
		delete _lProj;

	if (_liSP)
		delete _liSP;

	DeleteArrays();

	if (_renderSystem)
	{
		_renderSystem->GetEffectManager()->Delete(_genSMEffect);
		_renderSystem->GetEffectManager()->Delete(_dispSMEffect);
		_renderSystem->GetEffectManager()->Delete(_blurSMEffect);
		_renderSystem->GetEffectManager()->Delete(_genSATEffect);
		
		DeleteRenderTargets();
		DeleteMinMaxMipmap();
	}
}

bool D3dShadowMapping::Initialize(D3dRenderSystem& renderSystem)
{
	_renderSystem = &renderSystem;

	GenerateArrays();

	if (!GenerateRenderTargets())
		return false;

	if (_renderSystem->GetViewport().height && _renderSystem->GetViewport().width)
		if (!GenerateMinMaxMipmap())
			return false;

	return Compile();
}

void D3dShadowMapping::Render()
{
	if (!_lProj || !_camera || !_nodes.size())
		return;

	_renderSystem->SetActivePass(this);

	if (_useGS)
		_nodes[_sceneId]->Reset();

	UpdateEyeViewProj();

	tMat4f eViewMat, eProjMat, lProjMat, lViewMat;
	D3dUtils::ConvertToLib(_eViewMat, eViewMat);
	D3dUtils::ConvertToLib(_eProjMat, eProjMat);

	BoundingBox sceneAABB;
	sceneAABB.Expand(*_nodes[_sceneId]);
	
	const View& eyeView = _camera->GetView();
	const Perspective& eyeProj = _camera->GetPerspective();

	UpdateLDirVDirAngle(eyeView, _lView);

	float nearDist = eyeProj.GetZNear();
	float farDist = eyeProj.GetZFar();

	if (_focusing || _useLiSP || _nbrOfSplits > 1)
	{
		const tMat4f invEyeViewProj(vmath::inverse(eProjMat * eViewMat));
		const Frustum camFrust(invEyeViewProj, BoundingBox(tVec3f(-1.0f, -1.0f, .0f), tVec3f(1.0f)));
		AdjustCameraPlanes(sceneAABB, camFrust, nearDist, farDist);
		//AdjustCameraPlanes(sceneAABB, nearDist, farDist);
	}

	nearDist += _zNear * (farDist - nearDist);

	if (_useMinDepth)
		nearDist = GetMinDepth();

	_depthRange.x = nearDist;
	_depthRange.y = farDist;

	if (_splitFrustums[0])
	{
		delete _splitFrustums[0];
		_splitFrustums[0] = 0;
	}

	// construct orange split frustum for visualization if the adjusted near plane distance
	// is greater than the actual near plane distance
	if (eyeProj.GetZNear() + 0.01f < nearDist)
	{
		Perspective persp(eyeProj.GetFOV(), eyeProj.GetAspect(), eyeProj.GetZNear(), nearDist);

		tMat4f eProjMtx;
		D3DXMATRIX eDxProjMtx = *(reinterpret_cast<D3DXMATRIX*>(_renderSystem->GetProjMatrix(persp)));
		D3dUtils::ConvertToLib(eDxProjMtx, eProjMtx);

		const tMat4f invViewProj(vmath::inverse(eProjMtx * eViewMat));
		_splitFrustums[0] = new Frustum(invViewProj, BoundingBox(tVec3f(-1.0f, -1.0f, .0f), tVec3f(1.0f)));
	}

	CalcSplitPositions(nearDist, farDist, _nbrOfSplits);

	Frustum* lightFrustum = 0;
	if (_lProj->GetType() == Projection::Perspective)
	{
		UpdatePointLightViewProj(eyeView);
		
		D3dUtils::ConvertToLib(_lProjMat, lProjMat);
		D3dUtils::ConvertToLib(_lViewMat, lViewMat);
		
		lightFrustum = new Frustum(vmath::inverse(lProjMat * lViewMat), BoundingBox(tVec3f(-1.0f, -1.0f, .0f), tVec3f(1.0f)));
	}

	RenderSystem::Viewport vp = _renderSystem->GetViewport();
	
	GetActiveRenderTarget().Bind(*_renderSystem);
	GetActiveRenderTarget().Clear(*_renderSystem);
	
	for (unsigned int split = 0; split < _nbrOfSplits; ++split)
	{
		const float zNear = _splitPositions[split];
		const float zFar  = _splitPositions[split+1];

		D3DXMATRIX projMtx = *(reinterpret_cast<D3DXMATRIX*>(
			_renderSystem->GetProjMatrix(Perspective(eyeProj.GetFOV(), eyeProj.GetAspect(), zNear, zFar))));

		tMat4f eProjMat;
		D3dUtils::ConvertToLib(projMtx, eProjMat);
		
		tMat4f invCViewProj(vmath::inverse(eProjMat * eViewMat));

		Frustum cameraFrustum(invCViewProj, BoundingBox(tVec3f(-1.0f, -1.0f, .0f), tVec3f(1.0f)));

		_intersectionBodys[split]->Clear();
		CalcPolygonBodyB(*_intersectionBodys[split], *_lProj, _lView, cameraFrustum, lightFrustum, sceneAABB);

		if (_intersectionBodys[split]->GetVertexCount())
		{
			if (split == 0 && _lProj->GetType() != Projection::Perspective)
			{
				UpdateDirLightViewProj(eyeView, *_intersectionBodys[split]);
	
				D3dUtils::ConvertToLib(_lProjMat, lProjMat);
				D3dUtils::ConvertToLib(_lViewMat, lViewMat);
			}

			tMat4f liSPMtx;

			if (_useLiSP)
			{
				if (split == 0)
				{
					_liSP->SetLightProjMat(lProjMat, _lProj->GetType());
					_liSP->SetLightView(lViewMat, _lView);
					_liSP->SetCamView(eViewMat, eyeView);
					_liSP->SetCamProjMat(eProjMat);
					_liSP->SetFieldOfView(eyeProj.GetFOV());
					_liSP->SetGamma(_gamma);
					_liSP->SetSplitParam(_lambda);
					_liSP->SetPseudoNear(_pseudoNear);
					_liSP->SetPseudoFar(_pseudoFar);
				}

				liSPMtx = _liSP->GetLiSPMtx(split, *_renderSystem, *_intersectionBodys[split],
					cameraFrustum, lightFrustum, sceneAABB, _depthRange);
				CalcLightViewProjMat(split, *_intersectionBodys[split], liSPMtx * lProjMat, lViewMat);
			}
			else
			{
				liSPMtx = vmath::identity4<float>();
				CalcLightViewProjMat(split, *_intersectionBodys[split], lProjMat, lViewMat);
			}

			D3dUtils::ConvertToD3d(liSPMtx, _liSPMatrices[split]);
			
			if (_useGS)
				FindCasters(split);
			else
				GenerateDepthMap(split);
		}

		//if (_debug)
			GenerateDebugInfos(invCViewProj, *_intersectionBodys[split], split);
	}
	
	if (_useGS)
		GenerateDepthMap(0);

	GetActiveRenderTarget().UnBind(*_renderSystem);
	
	if (_filterMode == SAVSM || _filterMode == UINTSAVSM)
	{
		D3dRenderTarget* src = NULL;
		D3dRenderTarget* dest = NULL;
		std::string technique;

		if(_filterMode == UINTSAVSM)
		{
			src = &_vsmUintMap;
			dest = &_vsmUintSat;
			technique = "GenerateUintSAT";
		}
		else
		{
			if(_distributePrecision)
			{
				src = &_vsmDistrMap;
				dest = &_vsmDistrSat;
				technique = "GenerateDistrSAT";
			}
			else
			{
				src = &_vsmMap;
				dest = &_vsmSat;
				technique = "GenerateSAT";
			}
		}

		for (unsigned int split = 0; split < _nbrOfSplits; ++split)
		{
			CreateSAT(src,dest,technique,split);
		}
	}

	if (_filterMode == VSM || _filterMode == ESM || _filterMode == EVSM)
	{
		if(GetActiveRenderTarget().GetMipMapped())
			_renderSystem->GetD3dContext()->GenerateMips(GetActiveRenderTarget().GetShaderResourceView());

		RenderSystem::CullState cull = _renderSystem->GetCullState();
		_renderSystem->SetCullState(RenderSystem::CullBackFace);
		RenderSystem::Viewport vp = _renderSystem->GetViewport();
		D3dRenderTarget* srcrt = NULL;

		if(_filterMode == VSM)
			srcrt = &_vsmMap;
		else if(_filterMode == ESM)
			srcrt = &_esmMap;
		else if(_filterMode == EVSM)
			srcrt = &_evsmMap;

		for (unsigned int split = 0; split < _nbrOfSplits; ++split)
		{
			Blur(*srcrt, _blur, tVec2f(1.0f, .0f), split);
			Blur(_blur, *srcrt, tVec2f(.0f, 1.0f), split);
		}

		_renderSystem->SetViewport(vp);
		_renderSystem->SetCullState(cull);
	}

	_renderSystem->SetViewport(vp);

	if (lightFrustum)
		delete lightFrustum;

	if (_calcError && (_useLiSP || _updateLightUp))
		CalcErrorValues();

	_renderSystem->SetActivePass(0);
}

void D3dShadowMapping::CreateSAT(D3dRenderTarget* src, D3dRenderTarget* dest, const std::string technique, const unsigned int split)
{
	ID3DX11EffectShaderResourceVariable* depthMap = _filterMode == UINTSAVSM ? _genSATEffect.imap : _genSATEffect.map;
	float size = _filterMode == UINTSAVSM ? src->GetSize().x : static_cast<float>(_shadowMapSz);

	int n = log(size)/log((float)_samplesPerPassSat);
	int m = log(size)/log((float)_samplesPerPassSat);
	int pass = _nbrOfSplits > 1 ? 2 : 0;

	_renderSystem->GetEffectManager()->Use(_genSATEffect,technique, pass);
	_genSATEffect.texwidth->SetInt(size);
	_genSATEffect.texheight->SetInt(size);
	_genSATEffect.samplesPerPass->SetInt(_samplesPerPassSat);
	_genSATEffect.tex->SetMatrix(_normMatrices[split]);
	_genSATEffect.texBounds->SetFloatVector(_UV[split]);

    //HorizontalPass
    for(int i = 0; i < n; i++)
    {
		dest->Bind(*_renderSystem);

		_genSATEffect.offset->SetInt(pow((float)_samplesPerPassSat,i));

		src->Bind(*_renderSystem, *depthMap);

        _renderSystem->Draw(*_viewports[split]);

		src->UnBind(*_renderSystem, *depthMap);
		dest->UnBind(*_renderSystem);

        // Swap pointers (ping pong)
        std::swap(dest, src);
    }

    //vertical Pass
	_renderSystem->GetEffectManager()->Use(_genSATEffect,technique, pass+1);
	_genSATEffect.texwidth->SetInt(size);
	_genSATEffect.texheight->SetInt(size);
	_genSATEffect.samplesPerPass->SetInt(_samplesPerPassSat);
	_genSATEffect.tex->SetMatrix(_normMatrices[split]);
	_genSATEffect.texBounds->SetFloatVector(_UV[split]);

    for(int i = 0; i < m; i++)
    {
		dest->Bind(*_renderSystem);

		_genSATEffect.offset->SetInt(pow((float)_samplesPerPassSat,i));

		src->Bind(*_renderSystem, *depthMap);

		//_renderSystem->Draw(RenderSystem::Viewport(0,0,size,size));
        _renderSystem->Draw(*_viewports[split]);

		src->UnBind(*_renderSystem, *depthMap);
		dest->UnBind(*_renderSystem);

        // Swap pointers (ping pong)
        std::swap(dest, src);
    }
}

void D3dShadowMapping::FindCasters(unsigned int split)
{
	_renderSystem->SetProjMatrix(&_lSplitProjMat.m[0][0]);
	_renderSystem->SetViewMatrix(&_lSplitViewMat.m[0][0]);

	_nodes[_sceneId]->Cull(*_renderSystem, split);
}

void D3dShadowMapping::GenerateDepthMap(unsigned int split)
{
	D3DXMATRIX viewProj;
	
	if (_useGS)
	{
		_renderSystem->SetViewports(_viewports);

		viewProj = _lViewMat * _lProjMat;

		_renderSystem->SetProjMatrix(&_lProjMat.m[0][0]);
		_renderSystem->SetViewMatrix(&_lViewMat.m[0][0]);

		_genSMEffect.liSP->SetMatrixArray(&_liSPMatrices[0].m[0][0], 0, _nbrOfSplits);
		_genSMEffect.crop->SetMatrixArray(&_cropMatrices[0].m[0][0], 0, _nbrOfSplits);
	}
	else
	{
		_renderSystem->SetViewport(*_viewports[split]);

		viewProj = _lSplitViewMat * _lSplitProjMat;
		
		_renderSystem->SetProjMatrix(&_lSplitProjMat.m[0][0]);
		_renderSystem->SetViewMatrix(&_lSplitViewMat.m[0][0]);
	}	

	_nodes[_sceneId]->Cull(*_renderSystem);
	
	switch (_filterMode)
	{
		case None:
		case HWPCF:
		case Box4x4PCF:
		case Poisson25PCF:
		case Box6x6PCF:
		case Poisson32PCF:
		case Box8x8PCF:
		case Poisson64PCF:
		case PCFPCSS:
		case VarBoxPCF:
			_renderSystem->GetEffectManager()->Use(_genSMEffect, "RenderHWSMDepthMap", _useGS ? 1 : 0);	
			break;
		case ESM:
			{
				if(_logblur)
					_renderSystem->GetEffectManager()->Use(_genSMEffect, "RenderLogESMDepthMap", _useGS ? 1 : 0);
				else
					_renderSystem->GetEffectManager()->Use(_genSMEffect, "RenderESMDepthMap", _useGS ? 1 : 0);
			}
			break;
		case VSM:
			_renderSystem->GetEffectManager()->Use(_genSMEffect, "RenderVSMDepthMap", _useGS ? 1 : 0);
			break;
		case EVSM:
			_renderSystem->GetEffectManager()->Use(_genSMEffect, "RenderEVSMDepthMap", _useGS ? 1 : 0);
			break;
		case SAVSM:
			if(_distributePrecision)
				_renderSystem->GetEffectManager()->Use(_genSMEffect, "RenderDistrSAVSMDepthMap", _useGS ? 1 : 0);
			else
				_renderSystem->GetEffectManager()->Use(_genSMEffect, "RenderSAVSMDepthMap", _useGS ? 1 : 0);
			break;
		case UINTSAVSM:
			_renderSystem->GetEffectManager()->Use(_genSMEffect, "RenderUintSAVSMDepthMap", _useGS ? 1 : 0);
			break;
	}

	_genSMEffect.viewProj->SetMatrix(viewProj);
	_genSMEffect.view->SetMatrix(_lViewMat);
	_genSMEffect.evsmconst->SetFloatVector(_evsmConst);

	_nodes[_sceneId]->Render(*_renderSystem);
}

void D3dShadowMapping::Blur(D3dRenderTarget& src, D3dRenderTarget& dest, const tVec2f& dim, unsigned int split)
{
	int filterWidth = _filterWidths[split];
	float sz = static_cast<float>(_shadowMapSz);

	if (filterWidth <= 1)
		return;	

	dest.Bind(*_renderSystem);

	if(_filterMode == ESM && _logblur)
		_renderSystem->GetEffectManager()->Use(_blurSMEffect, _useGaussFilter ? "LogGaussianBlur" : "LogBoxBlur", _nbrOfSplits > 1 ? 1 : 0);
	else
		_renderSystem->GetEffectManager()->Use(_blurSMEffect, _useGaussFilter ? "GaussianBlur" : "BoxBlur", _nbrOfSplits > 1 ? 1 : 0);

	_blurSMEffect.size->SetFloatVector(D3DXVECTOR2(sz, sz));
	_blurSMEffect.dim->SetFloatVector(D3DXVECTOR2(dim.x, dim.y));
	_blurSMEffect.samples->SetInt(filterWidth);
	_blurSMEffect.texBounds->SetFloatVector(_UV[split]);
	_blurSMEffect.gaussianWeights->SetFloatArray((float*)&_gaussianWeights[0],0,_gaussianWeights.size());

	if (_nbrOfSplits > 1)
		_blurSMEffect.tex->SetMatrix(_normMatrices[split]);

	src.Bind(*_renderSystem, *_blurSMEffect.map);

	_renderSystem->Draw(*_viewports[split]);

	src.UnBind(*_renderSystem, *_blurSMEffect.map);

	dest.UnBind(*_renderSystem);
}

void D3dShadowMapping::UpdateEyeViewProj()
{
	_eViewMat = *(reinterpret_cast<D3DXMATRIX*>(_renderSystem->GetViewMatrix(_camera->GetView())));

	_eProjMat = *(reinterpret_cast<D3DXMATRIX*>(_renderSystem->GetProjMatrix(_camera->GetPerspective())));
}

void D3dShadowMapping::UpdateLDirVDirAngle(const View& eView, const View& lView)
{
	const tVec3f cViewDir(vmath::normalize(eView.GetCenter() - eView.GetEye()));
	const tVec3f lViewDir(vmath::normalize(lView.GetCenter() - lView.GetEye()));

	_gamma = acos(abs(vmath::dot(cViewDir, lViewDir))) * static_cast<float>(180.0 / M_PI);

	_angleCamDirLightDir = acos(vmath::dot(cViewDir, lViewDir));
}

void D3dShadowMapping::UpdateDirLightViewProj(const View& eView, const PolygonBody& /*body*/)
{
	_lProjMat = *(reinterpret_cast<D3DXMATRIX*>(_renderSystem->GetProjMatrix(*_lProj)));

	if (_useLiSP || _updateLightUp)
	{
		//const unsigned int size = body.GetVertexCount();
		//const Polygon3d::tVertexArray& vertices = body.GetVertexArray();
		//const tVec3f eye(eView.GetEye());
		
		//tVec3f bodyVec(.0f);
		//for (unsigned int i = 0; i < size; ++i)
		//	bodyVec += (vertices[i] - eye);

		//const View lView(eye, eye + (_lView.GetCenter() - _lView.GetEye()), vmath::normalize(bodyVec));
		
		const tVec3f eye(eView.GetEye());
		const tVec3f leftVec(vmath::normalize(vmath::cross(eView.GetUp(), eView.GetCenter() - eye)));
		const tVec3f up(vmath::normalize(vmath::cross(_lView.GetCenter() - _lView.GetEye(), leftVec)));

		if (_useGS || !_focusing)
			_lViewMat = *(reinterpret_cast<D3DXMATRIX*>(_renderSystem->GetViewMatrix(View(_lView.GetEye(), _lView.GetCenter(), up))));
		else
			_lViewMat = *(reinterpret_cast<D3DXMATRIX*>(_renderSystem->GetViewMatrix(View(eye, eye + (_lView.GetCenter() - _lView.GetEye()), up))));
	}
	else
	{
		_lViewMat = *(reinterpret_cast<D3DXMATRIX*>(_renderSystem->GetViewMatrix(_lView)));
	}
}

void D3dShadowMapping::UpdatePointLightViewProj(const View& eView)
{
	_lProjMat = *(reinterpret_cast<D3DXMATRIX*>(_renderSystem->GetProjMatrix(*_lProj)));
	
	if (_useLiSP || _updateLightUp)
	{
		const tVec3f eye(eView.GetEye());
		const tVec3f leftVec(vmath::normalize(vmath::cross(eView.GetUp(), eView.GetCenter() - eye)));
		const tVec3f up(vmath::normalize(vmath::cross(_lView.GetCenter() - _lView.GetEye(), leftVec)));

		_lViewMat = *(reinterpret_cast<D3DXMATRIX*>(_renderSystem->GetViewMatrix(View(_lView.GetEye(), _lView.GetCenter(), up))));
	}
	else
	{
		_lViewMat = *(reinterpret_cast<D3DXMATRIX*>(_renderSystem->GetViewMatrix(_lView)));
	}
}

void D3dShadowMapping::ComputeGaussianWeights()
{
	float w = 2*_blurRadius + 1;
    float sigma = (w+1)/6;

    float twoSigmaSquare = 2.0f * sigma * sigma;
    float sigmaRoot = (float)sqrt(twoSigmaSquare * M_PI);
    float total = 0.0f;
    float distance = 0.0f;
    int index = 0;

	_gaussianWeights.resize(w);
    for (int i = -_blurRadius; i <= _blurRadius; ++i)
    {
        distance = i * i;
        index = i + _blurRadius;
        _gaussianWeights[index] = (float)exp(-distance / twoSigmaSquare) / sigmaRoot;
        total += _gaussianWeights[index];
    }

	for (int i = 0; i < _gaussianWeights.size(); i++)
        _gaussianWeights[i] /= total;
}

void D3dShadowMapping::AdjustCameraPlanes(const BoundingBox& aabb, const Frustum& frust, float& zNear, float& zFar)
{
	// find the nearest and farthest points of given
	// scene objects in camera's view space

	PolygonBody body;
	body.Add(frust);
	body.Clip(aabb);
	body.CreateUniqueVertexArray();

	const Polygon3d::tVertexArray& vertices = body.GetVertexArray();
	unsigned int size = body.GetVertexCount();

	float maxZ = .0f, minZ = FLT_MAX;

	const View& view = _camera->GetView();
	const tVec3f eye(view.GetEye());
	const tVec3f viewDir(vmath::normalize(view.GetCenter() - eye));

	// for each point in body
	for (unsigned int i = 0; i < size; ++i)
	{
		const tVec3f vPointToCam = vertices[i] - eye;
		const float dist = vmath::dot(vPointToCam, viewDir);

		// find boundary values
		maxZ = max(dist, maxZ);
		minZ = min(dist, minZ);
	}

	// use smallest distance as new near plane
	// and make sure it is not too small
	zNear = max(minZ, _camera->GetPerspective().GetZNear() + _zNear);

	// use largest distance as new far plane
	// and make sure it is larger than zNear
	zFar = max(maxZ, zNear + 1.0f);
}

//void D3dShadowMapping::AdjustCameraPlanes(const BoundingBox& aabb, float& zNear, float& zFar)
//{
//	// find the nearest and farthest points of given
//	// scene objects in camera's view space
//
//	float maxZ = .0f, minZ = FLT_MAX;
//
//	const View& view = _camera->GetView();
//	const tVec3f eye(view.GetEye());
//	const tVec3f viewDir(vmath::normalize(view.GetCenter() - eye));
//
//	// for each point in AABB
//	for (unsigned int i = 0; i < BoundingBox::CornerCOUNT; ++i)
//	{
//		const tVec3f vPointToCam = aabb.GetCorner(static_cast<BoundingBox::Corner>(i)) - eye;
//		const float dist = vmath::dot(vPointToCam, viewDir);
//
//		// find boundary values
//		maxZ = max(dist, maxZ);
//		minZ = min(dist, minZ);
//	}
//
//	// use smallest distance as new near plane
//	// and make sure it is not too small
//	zNear = max(minZ, _camera->GetPerspective().GetZNear() + _zNear);
//
//	// use largest distance as new far plane
//	// and make sure it is larger than zNear
//	zFar = max(maxZ, zNear + 1.0f);
//}

void D3dShadowMapping::CalcLightViewProjMat(unsigned int split, const PolygonBody& body, const tMat4f& lProjMat, const tMat4f& lViewMat)
{
	tMat4f lViewProjMat = lProjMat * lViewMat;

	unsigned int size = body.GetVertexCount();
	const Polygon3d::tVertexArray& vertices = body.GetVertexArray();

	// map to unit cube
	BoundingBox bodyAABB;
	for (unsigned int i = 0; i < size; ++i)
		bodyAABB.Expand(vertices[i], lViewProjMat);

	tVec3f maximum = bodyAABB.GetMaximum();
	tVec3f minimum = bodyAABB.GetMinimum();

	// maximize z for more robust shadows to avoid unwanted far plane clipping
	maximum.z = 1.0f;

	const float diffX = maximum.x - minimum.x;
	const float diffY = maximum.y - minimum.y;
	const float diffZ = maximum.z - minimum.z;

	const float sumX = maximum.x + minimum.x;
	const float sumY = maximum.y + minimum.y;
	//const float sumZ = maximum.z + minimum.z;

	tVec3f scale(2.0f / diffX, 2.0f / diffY, 1.0f / diffZ);
	tVec3f offset(-sumX / diffX, -sumY / diffY, -minimum.z / diffZ);

	if (_reduceSwimming && !(_useLiSP || _updateLightUp))
	{
		float scaleQuantizer = 64.0f;
		scale.x = 1.0f / ceil(1.0f / scale.x * scaleQuantizer) * scaleQuantizer;
		scale.y = 1.0f / ceil(1.0f / scale.y * scaleQuantizer) * scaleQuantizer;

		offset.x = -0.5f * (sumX) * scale.x;
		offset.y = -0.5f * (sumY) * scale.y;

		float halfTexSz = 0.5f * _shadowMapSz;
		offset.x = ceil(offset.x * halfTexSz) / halfTexSz;
		offset.y = ceil(offset.y * halfTexSz) / halfTexSz;
	}
	
	_filterScales[split] = tVec2f(scale);
	_splitScales[split] = 5.0e-4f * _lightRadius * D3DXVECTOR2(scale.x, _useLiSP ? scale.x : scale.y) * _lightRadiusSplitCor;
	_filterWidths[split] = (_blurRadius * 2 + 1);// * _filterScales[split].x * _blurSplitCor;

	tMat4f scaleTranslate
	(
		scale.x, .0, .0, offset.x,
		.0, scale.y, .0, offset.y,
		.0, .0, scale.z, offset.z,
		.0, .0, .0, 1.0
	);
	
	tMat4f lProjMtx = lProjMat;
	
	if (_useLiSP || _nbrOfSplits > 1 || _focusing)
		lProjMtx = scaleTranslate * lProjMat;

	D3dUtils::ConvertToD3d(lProjMtx, _lSplitProjMat);
	D3dUtils::ConvertToD3d(lViewMat, _lSplitViewMat);
	D3dUtils::ConvertToD3d(scaleTranslate, _cropMatrices[split]);

	_lightView[split] = _lSplitViewMat;
	_lightProj[split] = _lSplitProjMat;
	_normLightViewProj[split] = _lSplitViewMat * _lSplitProjMat * _normMatrices[split];
}

void D3dShadowMapping::CalcSplitPositions(float zNear, float zFar, unsigned int nbrOfSplits)
{
	_splitPositions[0] = zNear;
	_splitPositions[nbrOfSplits] = zFar;
	
	float n = zNear;
	float f = zFar;

	if (nbrOfSplits > 1)
	{
		n += _pseudoNear * (zFar - zNear);
		f -= _pseudoFar * (zFar - zNear);
	}

	for (unsigned int i = 1; i < nbrOfSplits; ++i)
	{
		float s =  i / static_cast<float>(nbrOfSplits);
		float cLog = n * powf(f / n, s);
		float cUni = n + (f - n) * s;

		_splitPositions[i] = (1.0f - _lambda) * cUni + _lambda * cLog;
	}
}

void D3dShadowMapping::CalcPolygonBodyB(PolygonBody& bodyB, const Projection& lProj, const View& lView, const Frustum& cFrust, const Frustum* lFrust, BoundingBox& s)
{
	bodyB.Add(cFrust);

	if (lProj.GetType() == Projection::Perspective && lFrust)
	{
		// point light
		if (_focusing)
		{
			bodyB.Clip(s);
			bodyB.CreateConvexHull(lView.GetEye());
			bodyB.Clip(*lFrust);
			bodyB.Clip(s);
		}
		else
			bodyB.CreateConvexHull(lView.GetEye());
		
		bodyB.CreateUniqueVertexArray();
	}
	else
	{
		// directional light
		if (_focusing)
			bodyB.Clip(s);
		
		bodyB.Extrude(tVec3f(lView.GetEye() - lView.GetCenter()), s);
	}
}

float D3dShadowMapping::GetMinDepth()
{
	RenderSystem::Viewport vp = _renderSystem->GetViewport();
	
	_renderSystem->SetProjMatrix(&_eProjMat.m[0][0]);
	_renderSystem->SetViewMatrix(&_eViewMat.m[0][0]);

	_nodes[_sceneId]->Cull(*_renderSystem);
	
	_camDepthMipmap[0]->Bind(*_renderSystem);
	_camDepthMipmap[0]->Clear(*_renderSystem);

	_renderSystem->GetEffectManager()->Use(_genSMEffect, "RenderPreDepth", 0);	

	_genSMEffect.view->SetMatrix(&_eViewMat.m[0][0]);
	_genSMEffect.projection->SetMatrix(&_eProjMat.m[0][0]);

	RenderSystem::Viewport viewport(0, 0, _camDepthMipmap[0]->GetSize().x, _camDepthMipmap[0]->GetSize().y);
	_renderSystem->SetViewport(viewport);

	_nodes[_sceneId]->Render(*_renderSystem);	

	_camDepthMipmap[0]->UnBind(*_renderSystem);

	_renderSystem->GetEffectManager()->Use(_dispSMEffect, "GenMipmap", 0);

	for (unsigned int i = 1; i < _camDepthMipmap.size(); ++i)
	{
		_camDepthMipmap[i]->Bind(*_renderSystem);
		_camDepthMipmap[i]->Clear(*_renderSystem);
		
		int size[2] = {
			static_cast<int>(_camDepthMipmap[i-1]->GetSize().x),
			static_cast<int>(_camDepthMipmap[i-1]->GetSize().y)
		};
		_dispSMEffect.mapSize->SetIntVector(size);

		ID3DX11EffectShaderResourceVariable* depthMap = _filterMode == FilterMode::UINTSAVSM ? _dispSMEffect.idepthMap : _dispSMEffect.depthMap;
		_camDepthMipmap[i-1]->Bind(*_renderSystem, *depthMap);
		
		_renderSystem->Draw(RenderSystem::Viewport(0, 0, _camDepthMipmap[i]->GetSize().x, _camDepthMipmap[i]->GetSize().y));
		
		_camDepthMipmap[i-1]->UnBind(*_renderSystem, *depthMap);
		_camDepthMipmap[i]->UnBind(*_renderSystem);
	}

	_renderSystem->SetViewport(vp);
	
	_renderSystem->GetD3dContext()->CopyResource(_minmaxMIP.GetTexture2D(), _camDepthMipmap[_camDepthMipmap.size()-1]->GetTexture2D());

	D3D11_MAPPED_SUBRESOURCE mappedTex;

	float zNear = FLT_MAX;
	//if (_minmaxMIP.GetTexture2D()->Map(D3D11CalcSubresource(0, 0, 0), D3D11_MAP_READ, 0, &mappedTex) == S_OK)
	if(_renderSystem->GetD3dContext()->Map(_minmaxMIP.GetTexture2D(),D3D11CalcSubresource(0,0,0),D3D11_MAP_READ,0,&mappedTex) == S_OK)
	{
		for (unsigned int i = 0; i < _minmaxMIP.GetSize().y; ++i)
		{
			BYTE* pLine = &(reinterpret_cast<BYTE*>(mappedTex.pData))[i * mappedTex.RowPitch];
			
			for (unsigned int j = 0; j < 2 * _minmaxMIP.GetSize().x; j += 2)
				zNear = min(*(reinterpret_cast<float*>(&pLine[j*sizeof(float)])), zNear);
		}

		_renderSystem->GetD3dContext()->Unmap(_minmaxMIP.GetTexture2D(), 0);
	}
	
	return zNear == FLT_MAX ? 1.0f : zNear;
}

void D3dShadowMapping::DeleteMinMaxMipmap()
{
	_renderSystem->GetBufferManager()->Delete(_minmaxMIP);

	for (unsigned int i = 0; i < _camDepthMipmap.size(); ++i)
	{
		if (_camDepthMipmap[i])
		{
			_renderSystem->GetBufferManager()->Delete(*_camDepthMipmap[i]);
			delete _camDepthMipmap[i];
			_camDepthMipmap[i] = 0;
		}
	}
}

bool D3dShadowMapping::GenerateMinMaxMipmap()
{
	DeleteMinMaxMipmap();
	
	_minmaxMIP.SetArraySize(1);
	_minmaxMIP.SetSize(tVec2u(2));
	if (!_renderSystem->GetBufferManager()->Generate(_minmaxMIP, *_renderSystem))
		return false;

	_camDepthMipmap.resize(10);
	tVec2u size(_renderSystem->GetViewport().width, _renderSystem->GetViewport().height);

	for (unsigned int i = 0; i < _camDepthMipmap.size(); ++i)
	{
		_camDepthMipmap[i] = new D3dRGFloatRT();
		_camDepthMipmap[i]->SetArraySize(1);
		_camDepthMipmap[i]->SetClearColor(tVec4f(FLT_MAX, FLT_MAX, FLT_MAX, 1.0f));
		_camDepthMipmap[i]->SetSize(size);
		if (!_renderSystem->GetBufferManager()->Generate(*_camDepthMipmap[i], *_renderSystem))
			return false;
		//size /= 2;
		size = tVec2u(512) / static_cast<unsigned int>(pow(2.0, static_cast<int>(i)));
	}

	return true;
}

void D3dShadowMapping::GenerateDebugInfos(const tMat4f& invEyeViewProj, const PolygonBody& body, unsigned int split)
{
	if (_splitFrustums[split+1])
		delete _splitFrustums[split+1];

	_splitFrustums[split+1] = new Frustum(invEyeViewProj, BoundingBox(tVec3f(-1.0f, -1.0f, .0f), tVec3f(1.0f)));

	// for visualisation purposes
	if (_splitBBs[split])
		delete _splitBBs[split];

	unsigned int size = body.GetVertexCount();
	const Polygon3d::tVertexArray& vertices = body.GetVertexArray();

	_splitBBs[split] = new BoundingBox();

	for (unsigned int i = 0; i < size; ++i)
		_splitBBs[split]->Expand(vertices[i]);
}

void D3dShadowMapping::CalcErrorValues()
{
	for (unsigned int i = 0; i < ErrDirCOUNT; ++i)
	{
		_errVals[i].assign(_errVals[i].size(), tVec3f(.0f, .0f, -1.0f));
		_errRanges[i].x = FLT_MAX;
		_errRanges[i].y = -1.0f;
	}

	RenderSystem::Viewport vp = _renderSystem->GetViewport();
	
	const tVec3f eye(_camera->GetView().GetEye());
	const tVec3f center(_camera->GetView().GetCenter());
	const tVec3f viewDir(vmath::normalize(center - eye));

	tMat2f imageResMtx(static_cast<float>(vp.width), .0f, .0f, static_cast<float>(vp.height));
	tMat2f shadowResMtx(1/static_cast<float>(_shadowMapSz), .0f, .0f, 1/static_cast<float>(_shadowMapSz));

	const float step = (_splitPositions[_nbrOfSplits] - _splitPositions[0]) / static_cast<float>(NbrOfSamples-1);

	for (unsigned int sample = 0; sample < NbrOfSamples; ++sample)
	{
		float depth = _splitPositions[0] + sample * step;

		for (unsigned int i = 0; i <= _nbrOfSplits; ++i)
		{
			if (depth == _splitPositions[i] || abs(depth - _splitPositions[i]) < step)
			{
				depth = _splitPositions[i];
				_splitErrorSmples[i] = sample;
				break;
			}
		}

		if (_pseudoNear > .0f)
		{
			float zPseudoNear = _depthRange.x + _pseudoNear * (_depthRange.y - _depthRange.x);
			if (depth == zPseudoNear || abs(depth - zPseudoNear) < step)
			{
				depth = zPseudoNear;
				_errPseudoNF[0] = sample;
			}
		}

		if (_pseudoFar > .0f)
		{
			float zPseudoFar = _depthRange.y - _pseudoFar * (_depthRange.y - _depthRange.x);
			if (depth == zPseudoFar || abs(depth - zPseudoFar) < step)
			{
				depth = zPseudoFar;
				_errPseudoNF[1] = sample;
			}
		}

		const tVec4f wPos = tVec4f(eye + depth * viewDir, 1.0f); 

		bool found = false;
		for (unsigned int split = 0; split < _nbrOfSplits && !found; ++split)
		{
			if (_useBBSel)
			{
				tMat4f texMtx;
				D3dUtils::ConvertToLib(_normLightViewProj[split], texMtx);
				
				const tVec4f lPos(texMtx * wPos);
				const tVec2f uv(lPos.x / lPos.w, lPos.y / lPos.w);
				const float zReceiver = lPos.z / lPos.w;

				if ((uv.x >= _UV[split].x && uv.y >= _UV[split].y && zReceiver >= 0.0f) &&
					(uv.x <= _UV[split].z && uv.y <= _UV[split].w && zReceiver <= 1.0f))
					found = true;
			}
			else
			{
				if (depth <= _splitPositions[split+1])
					found = true;	
			}

			if (found)
			{
				const float n = _splitPositions[split];
				const float f = _splitPositions[split+1];
				const float dist = f - n;
				//const float res = (vp.width * vp.height) / powf(static_cast<float>(_shadowMapSz), 2.0f);
				const float resX = vp.width / static_cast<float>(_shadowMapSz);
				const float resY = vp.height / static_cast<float>(_shadowMapSz);
				//const float sin_gamma = sin(_angleCamDirLightDir);
				const float theta = static_cast<float>(DEG2RAD(0.5 * _camera->GetPerspective().GetFOV()));
				//const float aspect = _camera->GetPerspective().GetAspect();

				if (_useLiSP)
				{
					float n_LiSP = _liSP->GetDistances()[_liSP->GetMode()][split].x;

					// error in z-direction
					//_errVals[ZDir][sample].x = res * (powf((depth - n) / sin_gamma + n_LiSP, 2.0f) / depth) *
					//						   (dist / (n_LiSP * sin_gamma * (n_LiSP + dist)));
					
					// error in x-direction
					_errVals[XDir][sample].x = resX * f * ((n_LiSP + depth - n) / (depth * (n_LiSP + dist)));

					// error in y-direction
					_errVals[YDir][sample].x = resY * dist / (2 * tanf(theta)) *
										       (powf(n_LiSP + depth - n, 2.0f) / (depth * n_LiSP * (n_LiSP + dist)));
				}
				else
				{
					//float n_LiSP = 1.0e10f;

					// error in z-direction
					//_errVals[ZDir][sample].x = res * (powf((depth - n) / sin_gamma + n_LiSP, 2.0f) / depth) *
					//						   (dist / (n_LiSP * sin_gamma * (n_LiSP + dist)));

					// error in x-direction
					_errVals[XDir][sample].x = resX * f / depth;

					// error in y-direction
					_errVals[YDir][sample].x = resY * dist / (2 * tanf(theta)) * 1.0f / depth;
				}

				for (unsigned int i = 0; i < ErrDirCOUNT; ++i)
				{
					_errVals[i][sample].y = depth;
					_errVals[i][sample].z = static_cast<float>(split);

					_errRanges[i].x = min(_errRanges[i].x, _errVals[i][sample].x);
					_errRanges[i].y = max(_errRanges[i].y, _errVals[i][sample].x);
				}
			}
		}
	}
}

void D3dShadowMapping::DisplayDepthMaps(const RenderSystem::Viewport& viewport)
{
	D3dRenderTarget* rt = NULL;
	ID3DX11EffectShaderResourceVariable* depthMap = NULL;
	int size[2];

	switch (_filterMode)
	{
		case None:
		case HWPCF:
		case Box4x4PCF:
		case Poisson25PCF:
		case Box6x6PCF:
		case Poisson32PCF:
		case Box8x8PCF:
		case Poisson64PCF:
		case PCFPCSS:
		case VarBoxPCF:
			_renderSystem->GetEffectManager()->Use(_dispSMEffect, "DisplayDepthMap", 0);
			depthMap = _dispSMEffect.depthMap; //float depthmap
			rt = &_depthMap;
			break;
		case ESM:
			{
				if(_logblur)
					_renderSystem->GetEffectManager()->Use(_dispSMEffect, "DisplayLogESMDepthMap", 0);
				else
				{
					_renderSystem->GetEffectManager()->Use(_dispSMEffect, "DisplayESMDepthMap", 0);
					_dispSMEffect.evsmconst->SetFloatVector(_evsmConst);
				}
				depthMap = _dispSMEffect.depthMap; //float depthmap
				rt = &_esmMap;
			}
			break;
		case VSM:
			_renderSystem->GetEffectManager()->Use(_dispSMEffect, "DisplayDepthMap", 0);
			depthMap = _dispSMEffect.depthMap; //float depthmap
			rt = &_vsmMap;
			break;
		case EVSM:
			_renderSystem->GetEffectManager()->Use(_dispSMEffect, "DisplayESMDepthMap", 0);
			depthMap = _dispSMEffect.depthMap; //float depthmap
			_dispSMEffect.evsmconst->SetFloatVector(_evsmConst);
			rt = &_evsmMap;
			break;
		case SAVSM:
			if(_distributePrecision)
			{
				_renderSystem->GetEffectManager()->Use(_dispSMEffect, "DisplayDistrSAVSMDepthMap", _nbrOfSplits > 1 ? 1 : 0);
				rt = &_vsmDistrMap;
			}
			else
			{
				_renderSystem->GetEffectManager()->Use(_dispSMEffect, "DisplaySAVSMDepthMap", _nbrOfSplits > 1 ? 1 : 0);
				rt = &_vsmMap;
			}

			depthMap = _dispSMEffect.depthMap; //float depthmap

			size[0] = _shadowMapSz;//rt->GetSize().x;
			size[1] = _shadowMapSz;//rt->GetSize().y;
			_dispSMEffect.mapSize->SetIntVector(size);

			break;
		case UINTSAVSM:
			_renderSystem->GetEffectManager()->Use(_dispSMEffect, "DisplayUintSAVSMDepthMap", _nbrOfSplits > 1 ? 1 : 0);
			depthMap = _dispSMEffect.idepthMap; //integer depthmap

			rt = &_vsmUintMap;

			size[0] = rt->GetSize().x;
			size[1] = rt->GetSize().y;
			_dispSMEffect.mapSize->SetIntVector(size);
			break;
	}

	if(_filterMode >= SAVSM)
	{
		unsigned int divisor = rt->GetSize().x / _shadowMapSz;
		unsigned int factor = divisor * 2;

		for (unsigned int split = 0; split < _nbrOfSplits; ++split)
		{
			int x = viewport.x + viewport.width / divisor * ( split % (factor / 2) );
			int y = viewport.y + viewport.width / divisor * ( 2 * split / factor );
			int width = viewport.width / divisor;
			int height = viewport.height / divisor;
			RenderSystem::Viewport v = RenderSystem::Viewport(x,y,width,height);

			_dispSMEffect.tex->SetMatrix(_normMatrices[split]);
			_dispSMEffect.texBounds->SetFloatVector(_UV[split]);
			_dispSMEffect.filterWidth->SetInt(_filterWidths[split]);

			rt->Bind(*_renderSystem, *depthMap);
			_renderSystem->Draw(v);
			rt->UnBind(*_renderSystem, *depthMap);
		}
	}
	else
	{
		rt->Bind(*_renderSystem, *depthMap);
		_renderSystem->Draw(viewport);
		rt->UnBind(*_renderSystem, *depthMap);
	}
}

void D3dShadowMapping::DisplayMap(RenderSystem::Viewport& viewport, const D3dRenderTarget& rt)
{
	_renderSystem->GetEffectManager()->Use(_dispSMEffect, "DisplayTexMap", 0);	

	rt.Bind(*_renderSystem, *_dispSMEffect.texMap, 0);

	_renderSystem->Draw(viewport);
	
	rt.UnBind(*_renderSystem, *_dispSMEffect.texMap);

	rt.Bind(*_renderSystem, *_dispSMEffect.texMap, 1);

	viewport.x = viewport.x + 255;
	_renderSystem->Draw(viewport);
	
	rt.UnBind(*_renderSystem, *_dispSMEffect.texMap);
}

void D3dShadowMapping::GenerateArrays()
{
	DeleteArrays();

	_splitPositions.resize(_nbrOfSplits + 1);
	_intersectionBodys.resize(_nbrOfSplits);
	_normLightViewProj.resize(_nbrOfSplits);
	_lightView.resize(_nbrOfSplits);
	_lightProj.resize(_nbrOfSplits);
	_splitFrustums.resize(_nbrOfSplits + 1);
	_splitBBs.resize(_nbrOfSplits);
	_planesNearFar.resize(_nbrOfSplits);
	_normLightViewProj.resize(_nbrOfSplits);
	_normMatrices.resize(_nbrOfSplits);
	_viewports.resize(_nbrOfSplits);
	_cropMatrices.resize(_nbrOfSplits);
	_liSPMatrices.resize(_nbrOfSplits);
	_filterScales.resize(_nbrOfSplits);
	_splitScales.resize(_nbrOfSplits);
	_filterWidths.resize(_nbrOfSplits);
	_UV.resize(_nbrOfSplits);
	_splitErrorSmples.resize(_nbrOfSplits + 1);

	for (unsigned int i = 0; i < _nbrOfSplits; ++i)
	{
		_viewports[i] = new RenderSystem::Viewport(0, 0, _shadowMapSz, _shadowMapSz);
		_intersectionBodys[i] = new PolygonBody();
	}
}

void D3dShadowMapping::DeleteArrays()
{
	for (unsigned int i = 0; i < _intersectionBodys.size(); ++i)
	{
		if (_intersectionBodys[i])
		{
			delete _intersectionBodys[i];
			_intersectionBodys[i] = 0;
		}
	}

	for (unsigned int i = 0; i < _splitFrustums.size(); ++i)
	{
		if (_splitFrustums[i])
		{
			delete _splitFrustums[i];
			_splitFrustums[i] = 0;
		}
	}

	for (unsigned int i = 0; i < _splitBBs.size(); ++i)
	{
		if (_splitBBs[i])
		{
			delete _splitBBs[i];
			_splitBBs[i] = 0;
		}
	}

	for (unsigned int i = 0; i < _viewports.size(); ++i)
	{
		if (_viewports[i])
		{
			delete _viewports[i];
			_viewports[i] = 0;
		}
	}
}

D3dRenderTarget& D3dShadowMapping::GetActiveRenderTarget()
{
	if (_filterMode <= PCFPCSS || _filterMode == VarBoxPCF)
		return _depthMap;
	else if (_filterMode == ESM)
		return _esmMap;
	else if (_filterMode == VSM)
		return _vsmMap;
	else if (_filterMode == EVSM)
		return _evsmMap;
	else if(_filterMode == SAVSM)
	{
		if(_distributePrecision)
			return _vsmDistrMap;
		else
			return _vsmMap;
	}
	else if(_filterMode == UINTSAVSM)
		return _vsmUintMap;
}

void D3dShadowMapping::GenerateTexAtlasConstants(float unit, unsigned int factor)
{
	for (unsigned int i = 0; i < _nbrOfSplits; ++i)
	{
		_viewports[i]->x = _shadowMapSz * ( i % (factor / 2) );
		_viewports[i]->y = _shadowMapSz * ( 2 * i / factor );
		_viewports[i]->width = _shadowMapSz;
		_viewports[i]->height = _shadowMapSz;

		const float x = unit * ( (2 * i + 1) % factor );
		const float y = unit * ( 2 * (2 * i / factor) + 1 );		
		_normMatrices[i] = D3DXMATRIX
		(
			unit,   .0f,  .0f,  .0f,
			 .0f, -unit,  .0f,  .0f,
			 .0f,   .0f, 1.0f,  .0f,
			   x,     y,  .0f, 1.0f
		);
	}
}

bool D3dShadowMapping::GenerateRenderTargets()
{
	DeleteRenderTargets();

	D3dRenderTarget& rt = GetActiveRenderTarget();
	rt.SetArraySize(1);

	if (_nbrOfSplits == 1)
	{
		rt.SetSize(tVec2u(_shadowMapSz));
		_lightRadiusSplitCor = _blurSplitCor = 1.0f;
		GenerateTexAtlasConstants(0.5f, 2);
		_UV[0] = tVec4f(.0f, .0f, 1.0f, 1.0f);
	}
	else if (static_cast<int>(_nbrOfSplits) - 5 < 0)
	{
		// 2 - 4 splits
		rt.SetSize(tVec2u(_shadowMapSz * 2));
		_lightRadiusSplitCor = _blurSplitCor = 0.5f;
		GenerateTexAtlasConstants(0.5f/2.0f, 4);
		for (unsigned int i = 0; i < _nbrOfSplits; ++i)
			_UV[i] = UV4[i];
	}
	else if (static_cast<int>(_nbrOfSplits) - 10 < 0)
	{
		// 5 - 9 splits
		rt.SetSize(tVec2u(_shadowMapSz * 3));
		_lightRadiusSplitCor = _blurSplitCor = 1 / 3.0f;
		GenerateTexAtlasConstants(0.5f/3.0f, 6);
		for (unsigned int i = 0; i < _nbrOfSplits; ++i)
			_UV[i] = UV9[i];
	}
	else
	{
		// 10 - 16 splits
		rt.SetSize(tVec2u(_shadowMapSz * 4));	
		_lightRadiusSplitCor = _blurSplitCor = 0.25f;
		GenerateTexAtlasConstants(0.5f/4.0f, 8);
		for (unsigned int i = 0; i < _nbrOfSplits; ++i)
			_UV[i] = UV16[i];
	}

	if (_filterMode == VSM || _filterMode == ESM || _filterMode == EVSM)
		rt.SetMipMapped(_mipmapped);

	if (!_renderSystem->GetBufferManager()->Generate(rt, *_renderSystem))
		return false;

	if (_filterMode == VSM || _filterMode == ESM || _filterMode == EVSM)
	{
		_blur.SetArraySize(1);
		_blur.SetSize(rt.GetSize());

		if (!_renderSystem->GetBufferManager()->Generate(_blur, *_renderSystem))
			return false;
	}

	if (_filterMode < SAVSM)
		return true;
	
	if (_filterMode == UINTSAVSM)
	{
		_vsmUintSat.SetArraySize(1);
		_vsmUintSat.SetSize(rt.GetSize());
		if (!_renderSystem->GetBufferManager()->Generate(_vsmUintSat, *_renderSystem))
			return false;
	}
	else
	{
		if (_distributePrecision)
		{
			_vsmDistrSat.SetArraySize(1);
			_vsmDistrSat.SetSize(rt.GetSize());
			if (!_renderSystem->GetBufferManager()->Generate(_vsmDistrSat, *_renderSystem))
				return false;
		}
		else
		{
			_vsmSat.SetArraySize(1);
			_vsmSat.SetSize(rt.GetSize());
			if (!_renderSystem->GetBufferManager()->Generate(_vsmSat, *_renderSystem))
				return false;
		}
	}

	return true;
}

void D3dShadowMapping::DeleteRenderTargets()
{
	_renderSystem->GetBufferManager()->Delete(_depthMap);
	_renderSystem->GetBufferManager()->Delete(_vsmMap);
	_renderSystem->GetBufferManager()->Delete(_esmMap);
	_renderSystem->GetBufferManager()->Delete(_evsmMap);
	_renderSystem->GetBufferManager()->Delete(_blur);
	_renderSystem->GetBufferManager()->Delete(_vsmSat);
	_renderSystem->GetBufferManager()->Delete(_vsmDistrSat);
	_renderSystem->GetBufferManager()->Delete(_vsmDistrMap);
	_renderSystem->GetBufferManager()->Delete(_vsmUintSat);
	_renderSystem->GetBufferManager()->Delete(_vsmUintMap);
}

unsigned int D3dShadowMapping::GetShadowMapSize() const
{
	return _shadowMapSz;
}

unsigned int D3dShadowMapping::GetRenderTargetSize()
{
	return GetActiveRenderTarget().GetSize().x;
}

void D3dShadowMapping::SetShadowMapSize(unsigned int size)
{
	_shadowMapSz = size;

	if (!_renderSystem)
		return;
	
	GenerateRenderTargets();
}

const D3dRenderTarget& D3dShadowMapping::GetShadowMaps() const
{
	if (_filterMode <= PCFPCSS || _filterMode == VarBoxPCF)
		return _depthMap;
	else if (_filterMode == ESM)
		return _esmMap;
	else if (_filterMode == VSM)
		return _vsmMap;
	else if (_filterMode == EVSM)
		return _evsmMap;
	else if(_filterMode == SAVSM)
	{
		if(_distributePrecision)
			return _vsmDistrMap;
		else
			return _vsmMap;
	}
	else if(_filterMode == UINTSAVSM)
		return _vsmUintMap;
}

const Projection& D3dShadowMapping::GetLightProjection() const
{
	return *_lProj;
}

void D3dShadowMapping::SetLightProjection(const Projection& proj)
{
	if (_lProj)
		delete _lProj;

	switch (proj.GetType())
	{
		case Projection::Perspective:
		{
			const Perspective& p = dynamic_cast<const Perspective&>(proj);
			_lProj = new Perspective(p.GetFOV(), p.GetAspect(), p.GetZNear(), p.GetZFar());
			
		}
		break;
		
		case Projection::Ortho3d:
		{
			const Ortho3d& o = dynamic_cast<const Ortho3d&>(proj);
			_lProj = new Ortho3d(o.GetLeft(), o.GetRight(), o.GetBottom(), o.GetTop(), o.GetZNear(), o.GetZFar());
		}
		break;
	}
}

bool D3dShadowMapping::CheckRecompile(FilterMode mode) const
{
	bool result = true;

	if (_filterMode == mode)
		result = false;

	// check if recompile necessary
	if (mode == HWPCF || mode == None || mode > PCFPCSS)
		result = false;

	if ((_filterMode == Box4x4PCF && mode == Poisson25PCF) ||
		(mode == Box4x4PCF && _filterMode == Poisson25PCF))
		result = false;

	if ((_filterMode == Box6x6PCF && mode == Poisson32PCF) ||
		(mode == Box6x6PCF && _filterMode == Poisson32PCF))
		result = false;

	if ((_filterMode == Box8x8PCF && (mode == Poisson64PCF || mode == PCFPCSS)) ||
		(_filterMode == Poisson64PCF && (mode == Box8x8PCF || mode == PCFPCSS)) ||
		(_filterMode == PCFPCSS && (mode == Box8x8PCF || mode == Poisson64PCF)))
		result = false;

	return result;
}

bool D3dShadowMapping::Compile()
{
	if (_renderSystem)
	{
		_renderSystem->GetEffectManager()->Delete(_genSMEffect);
		_genSMEffect.SetFilename(GenDepthMapEffect::GenDepthMapSourceFile, Shader::CompleteSource);
		_genSMEffect.SetMacro(GenDepthMapEffect::NbrOfSplitsMacro, &_nbrOfSplits, D3dEffect::Integer);
		if (!_renderSystem->GetEffectManager()->Generate(_genSMEffect, *_renderSystem))
			return false;

		_renderSystem->GetEffectManager()->Delete(_dispSMEffect);
		_dispSMEffect.SetFilename(DispDepthMapEffect::DispDepthMapSourceFile, Shader::CompleteSource);
		if (!_renderSystem->GetEffectManager()->Generate(_dispSMEffect, *_renderSystem))
			return false;

		_renderSystem->GetEffectManager()->Delete(_blurSMEffect);
		_blurSMEffect.SetFilename(BlurEffect::BlurSourceFile, Shader::CompleteSource);
		if (!_renderSystem->GetEffectManager()->Generate(_blurSMEffect, *_renderSystem))
			return false;

		_renderSystem->GetEffectManager()->Delete(_genSATEffect);
		_genSATEffect.SetFilename(GenSATEffect::SATSourceFile, Shader::CompleteSource);
		if (!_renderSystem->GetEffectManager()->Generate(_genSATEffect, *_renderSystem))
			return false;

		return true;
	}

	return false;
}

unsigned int D3dShadowMapping::GetFilterMacroVal(FilterMode mode) const
{
	unsigned int result = 0;

	switch (mode)
	{
		case Box4x4PCF:
		case Poisson25PCF:
			result = 0;
			break;
		case Box6x6PCF:
		case Poisson32PCF:
			result = 1;
			break;
		case Box8x8PCF:
		case Poisson64PCF:
		case PCFPCSS:
			result = 2;
			break;
		default:
			result = 0;
	};

	return result;
}

const D3dShadowMapping::DispDepthMapEffect& D3dShadowMapping::Get2dEffect() const
{
	return _dispSMEffect;
}

const View& D3dShadowMapping::GetLightView() const
{
	return _lView;
}

void D3dShadowMapping::SetLightView(const View& view)
{
	_lView = view;
}

Camera& D3dShadowMapping::GetCamera()
{
	return *_camera;
}

void D3dShadowMapping::SetCamera(Camera& camera)
{
	_camera = &camera;
}

float D3dShadowMapping::GetSplitParam() const
{
	return _lambda;
}

void D3dShadowMapping::SetSplitParam(float lambda)
{
	_lambda = lambda;
	
	if (_liSP)
		_liSP->SetSplitParam(_lambda);
}

float D3dShadowMapping::GetBlurRadius() const
{
	return _blurRadius;
}

void D3dShadowMapping::SetBlurRadius(float radius)
{
	_blurRadius = floor(radius * 16.0f + 0.5f);

	ComputeGaussianWeights();
}

bool D3dShadowMapping::GetAlignmentLSVS() const
{
	return _updateLightUp;
}

void D3dShadowMapping::SetAlignmentLSVS(bool enable)
{
	_updateLightUp = enable;
}

float* D3dShadowMapping::GetSplitScales()
{
	return _splitScales[0];
}

int* D3dShadowMapping::GetFilterWidths()
{
	return (int*)(&_filterWidths[0]);
}

float D3dShadowMapping::GetLightRadius() const
{
	return _lightRadius;
}

void D3dShadowMapping::SetLightRadius(float radius)
{
	_lightRadius = radius;
}

const tVec2f& D3dShadowMapping::GetEVSMConst() const
{
	return _evsmConst;
}

void D3dShadowMapping::SetEVSMConst(float pos, float neg)
{
	_evsmConst = tVec2f(pos,neg);
}

bool D3dShadowMapping::GetUseLogBlur() const
{
	return _logblur;
}

void D3dShadowMapping::SetUseLogBlur(bool enable)
{
	_logblur = enable;
}

bool D3dShadowMapping::GetMipMapped() const
{
	return _mipmapped;
}

void D3dShadowMapping::SetMipMapped(bool enable)
{
	bool renewRTs = _mipmapped != enable;

	_mipmapped = enable;

	if (!_renderSystem || !renewRTs)
		return;

	GenerateRenderTargets();
}

bool D3dShadowMapping::GetUseGaussFilter() const
{
	return _useGaussFilter;
}

void D3dShadowMapping::SetUseGaussFilter(bool enable)
{
	_useGaussFilter = enable;
}

bool D3dShadowMapping::GetUseFocusing() const
{
	return _focusing;
}

void D3dShadowMapping::SetUseFocusing(bool enable)
{
	_focusing = enable;
}

float D3dShadowMapping::GetZNear() const
{
	return _zNear;
}

void D3dShadowMapping::SetZNear(float value)
{
	_zNear = value;
}

const tVec2f& D3dShadowMapping::GetDepthRange() const
{
	return _depthRange;
}

const D3dShadowMapping::tSplitSmpleArray& D3dShadowMapping::GetSplitPosSamples() const
{
	return _splitErrorSmples;
}

const D3dShadowMapping::tSplitSmpleArray& D3dShadowMapping::GetPseudoNFSamples() const
{
	return _errPseudoNF;
}

void D3dShadowMapping::UseScene(unsigned int id)
{
	_sceneId = id;
}

D3dShadowMapping::FilterMode D3dShadowMapping::GetFilterMode() const
{
	return _filterMode;
}

void D3dShadowMapping::SetFilterMode(FilterMode mode)
{
	D3dShadowMapping::FilterMode current = _filterMode;
	_filterMode = mode;

	if (!_renderSystem)
		return;

	if (current == mode ||
		(current <= PCFPCSS && mode <= PCFPCSS))
		//|| (current > PCFPCSS && mode > PCFPCSS))
		return;

	GenerateRenderTargets();
}

unsigned int D3dShadowMapping::GetNbrOfSplits() const
{
	return _nbrOfSplits;
}

bool D3dShadowMapping::GetSwimmingRed() const
{
	return _reduceSwimming;
}

void D3dShadowMapping::SetSwimmingRed(bool enable)
{
	_reduceSwimming = enable;
}

bool D3dShadowMapping::GetUseLiSP() const
{
	return _useLiSP;
}

void D3dShadowMapping::SetUseLiSP(bool enable)
{
	_useLiSP = enable;
}

LiSP& D3dShadowMapping::GetLiSP()
{
	return *_liSP;
}

bool D3dShadowMapping::GetUseGS() const
{
	return _useGS;
}

void D3dShadowMapping::SetUseGS(bool enable)
{
	if (_nbrOfSplits > 1)
		_useGS = enable;
}

bool D3dShadowMapping::GetUseMinDepth() const
{
	return _useMinDepth;
}

void D3dShadowMapping::SetUseMinDepth(bool enable)
{
	_useMinDepth = enable;
}

bool D3dShadowMapping::GetDistributePrecision() const
{
	return _distributePrecision;
}

void D3dShadowMapping::SetDistributePrecision(bool enable)
{
	_distributePrecision = enable;

	if (!_renderSystem)
		return;
	
	GenerateRenderTargets();
}

void D3dShadowMapping::SetDebug(bool enable)
{
	_debug = enable;
}

void D3dShadowMapping::SetNbrOfSplits(unsigned int nbrOfSplits)
{
	if (nbrOfSplits > MaxNbrOfSplits)
		return;

	unsigned int current = _nbrOfSplits;
	_nbrOfSplits = nbrOfSplits;

	if (current == nbrOfSplits)
		return;

	LiSP::Mode mode = LiSP::NOpt;
	std::vector<float> n(MaxNbrOfSplits, 1.0f);
	bool arbScaled = _liSP->GetArbScaled();
	if (_liSP)
	{
		mode = _liSP->GetMode();
		const LiSP::tPArray& params = _liSP->GetParamN();
		for (unsigned int i = 0; i < params.size(); ++i)
			n[i] = params[i];
		delete _liSP;
	}

	_liSP = new LiSP(_nbrOfSplits);
	_liSP->SetMode(mode);
	_liSP->SetArbScaled(arbScaled);
	for (unsigned int i = 0; i < _nbrOfSplits; ++i)
		_liSP->SetParamN(n[i], i);

	if (!_renderSystem)
		return;

	if (nbrOfSplits > 1)
	{
		_renderSystem->GetEffectManager()->Delete(_genSMEffect);
		_genSMEffect.SetMacro(GenDepthMapEffect::NbrOfSplitsMacro, &nbrOfSplits, D3dEffect::Integer);
		_renderSystem->GetEffectManager()->Generate(_genSMEffect, *_renderSystem);
	}

	GenerateArrays();

	GenerateRenderTargets();
}

float D3dShadowMapping::GetAngleCamDirLightDir() const
{
	return _angleCamDirLightDir * 180.0f / static_cast<float>(M_PI);
}

float* D3dShadowMapping::GetNormLightViewProjMtx()
{
	return &_normLightViewProj[0].m[0][0];
}

const D3dShadowMapping::tMatrixArray& D3dShadowMapping::GetTexMatrixArray() const
{
	return _normLightViewProj;
}

float* D3dShadowMapping::GetLightViewMtx()
{
	return &_lightView[0].m[0][0];
}

float* D3dShadowMapping::GetLightProjMtx(unsigned int split)
{
	return split < _nbrOfSplits ? &_lightProj[split].m[0][0] : 0;
}

float* D3dShadowMapping::GetSplitPositions()
{
	return &_splitPositions[1];
}

const D3dShadowMapping::tPosArray& D3dShadowMapping::GetSplitPos() const
{
	return _splitPositions;
}

const D3dShadowMapping::tFrustumArray& D3dShadowMapping::GetSplitFrustums() const
{
	return _splitFrustums;
}

D3dShadowMapping::tBBArray& D3dShadowMapping::GetSplitBBs()
{
	return _splitBBs;
}

const D3dShadowMapping::tDistArray& D3dShadowMapping::GetNearAndFar() const
{
	return _planesNearFar;
}

const D3dShadowMapping::tBodyArray& D3dShadowMapping::GetBodyArray() const
{
	return _intersectionBodys;
}

const D3dShadowMapping::tErrValsArray& D3dShadowMapping::GetErrorValues() const
{
	return _errVals;
}

const D3dShadowMapping::tDistArray& D3dShadowMapping::GetErrorRanges() const
{
	return _errRanges;
}

D3dShadowMapping::GenDepthMapEffect& D3dShadowMapping::GetGenDepthMapEffect()
{
	return _genSMEffect;
}

float D3dShadowMapping::GetPseudoNear() const
{
	return _pseudoNear;
}

void D3dShadowMapping::SetPseudoNear(float value)
{
	_pseudoNear = value;
}

float D3dShadowMapping::GetPseudoFar() const
{
	return _pseudoFar;
}

void D3dShadowMapping::SetPseudoFar(float value)
{
	_pseudoFar = value;
}

bool D3dShadowMapping::GetCalcError() const
{
	return _calcError;
}

void D3dShadowMapping::SetCalcError(bool enable)
{
	_calcError = enable;
}

bool D3dShadowMapping::GetUseBBSel() const
{
	return _useBBSel;
}

void D3dShadowMapping::SetUseBBSel(bool enable)
{
	_useBBSel = enable;
}

