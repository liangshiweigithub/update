#include "stdafx.h"

#include "MainPass.h"
#include "DXUTApp.h"
#include <limits>

/* MainPass::SingleSMShader ************************************************************/
const std::string MainPass::SingleSMShader::SingleSMSource("../shaders/RenderSingleSM.fx");
const std::string MainPass::SingleSMShader::PCFPresetMacro("PRESET");

MainPass::SingleSMShader::SingleSMShader()
{
}

MainPass::SingleSMShader::SingleSMShader(const std::string& name)
{
	SetName(name);
	AddMacro(PCFPresetMacro);
}

MainPass::SingleSMShader::~SingleSMShader()
{
}

bool MainPass::SingleSMShader::InitializeVariables(ID3DX11Effect& effect)
{
	diffuseTex = effect.GetVariableByName("texDiffuse")->AsShaderResource();
	if (!diffuseTex->IsValid()) return false;
	world = effect.GetVariableByName("g_mWorld")->AsMatrix();
	if (!world->IsValid()) return false;
	view = effect.GetVariableByName("g_mView")->AsMatrix();
	if (!view->IsValid()) return false;
	projection = effect.GetVariableByName("g_mProj")->AsMatrix();
	if (!projection->IsValid()) return false;
	tex = effect.GetVariableByName("g_mDiffuseTex")->AsMatrix();
	if (!tex->IsValid()) return false;
	textured = effect.GetVariableByName("textured")->AsScalar();
	if (!textured->IsValid()) return false;
	gammaCorrect = effect.GetVariableByName("gammaCorrect")->AsScalar();
	if (!gammaCorrect->IsValid()) return false;
	effectLight = effect.GetVariableByName("g_light");
	if (!effectLight->IsValid()) return false;
	matAmbient = effect.GetVariableByName("matAmbient")->AsVector();
	if (!matAmbient->IsValid()) return false;
	matDiffuse = effect.GetVariableByName("matDiffuse")->AsVector();
	if (!matDiffuse->IsValid()) return false;
	matSpecular = effect.GetVariableByName("matSpecular")->AsVector();
	if (!matSpecular->IsValid()) return false;
	matShininess = effect.GetVariableByName("matShininess")->AsScalar();
	if (!matShininess->IsValid()) return false;
	lightViewProj = effect.GetVariableByName("g_mNormLightViewProj")->AsMatrix();
	if (!lightViewProj->IsValid()) return false;
	shadowTex = effect.GetVariableByName("texShadow")->AsShaderResource();
	if (!shadowTex->IsValid()) return false;
	ishadowTex = effect.GetVariableByName("itexShadow")->AsShaderResource();
	if (!ishadowTex->IsValid()) return false;
	visTexels = effect.GetVariableByName("g_visTexels")->AsScalar();
	if (!visTexels->IsValid()) return false;
	shadowMapSz = effect.GetVariableByName("g_shadowMapSz")->AsScalar();
	if (!shadowMapSz->IsValid()) return false;
	lightview = effect.GetVariableByName("g_mLightView")->AsMatrix();
	if (!lightview->IsValid()) return false;
	lightRadius = effect.GetVariableByName("g_LightRadius")->AsVector();
	if (!lightRadius->IsValid()) return false;
	visTexelSz = effect.GetVariableByName("g_texelSz")->AsScalar();
	if (!visTexelSz->IsValid()) return false;
	camPos = effect.GetVariableByName("g_CamPos")->AsVector();
	if (!camPos->IsValid()) return false;
	visSampling = effect.GetVariableByName("g_visSampling")->AsScalar();
	if (!visSampling->IsValid()) return false;
	renderShadows = effect.GetVariableByName("g_renderShadows")->AsScalar();
	if (!renderShadows->IsValid()) return false;
	visError = effect.GetVariableByName("g_errorDir")->AsScalar();
	if (!visError->IsValid()) return false;
	camView = effect.GetVariableByName("g_mCamView")->AsMatrix();
	if (!camView->IsValid()) return false;
	viewVec = effect.GetVariableByName("g_viewVec")->AsVector();
	if (!viewVec->IsValid()) return false;
	lightVec = effect.GetVariableByName("g_lightVec")->AsVector();
	if (!lightVec->IsValid()) return false;
	useHWPCF = effect.GetVariableByName("g_hwPCF")->AsScalar();
	if (!useHWPCF->IsValid()) return false;
	useBlockerSearch = effect.GetVariableByName("g_blockerSearch")->AsScalar();
	if (!useBlockerSearch->IsValid()) return false;
	shading = effect.GetVariableByName("g_shading")->AsScalar();
	if (!shading->IsValid()) return false;
	distributePrecision = effect.GetVariableByName("g_distributePrecision")->AsScalar();
	if (!distributePrecision->IsValid()) return false;
	evsmconst = effect.GetVariableByName("g_EVSMConst")->AsVector();
	if (!evsmconst->IsValid()) return false;
	lbramount = effect.GetVariableByName("g_LBRAmount")->AsScalar();
	if (!lbramount->IsValid()) return false;
	useLogBlur = effect.GetVariableByName("g_LogBlur")->AsScalar();
	if (!useLogBlur->IsValid()) return false;
	filterWidth = effect.GetVariableByName("g_FilterWidth")->AsScalar();
	if (!filterWidth->IsValid()) return false;
	visPCFRegions = effect.GetVariableByName("g_VisPCFRegions")->AsScalar();
	if (!visPCFRegions->IsValid()) return false;
	useMipMap = effect.GetVariableByName("g_MipMapping")->AsScalar();
	if (!useMipMap->IsValid()) return false;
	visMipMap = effect.GetVariableByName("g_VisMipMap")->AsScalar();
	if (!visMipMap->IsValid()) return false;
	maxsamples = effect.GetVariableByName("g_MaxSamples")->AsScalar();
	if (!maxsamples->IsValid()) return false;

	return true;
}

bool MainPass::SingleSMShader::Initialize(ID3DX11Effect& effect)
{
	if (!effect.GetTechniqueByName("RenderSingleSM")->IsValid())
		return false;

	return InitializeVariables(effect);
}

void MainPass::SingleSMShader::UniformCallBack(D3dEffectManager&, const Material& material)
{
	const tVec4f& ambient = material.GetAmbient();
	const tVec4f& diffuse = material.GetDiffuse();
	const tVec4f& specular = material.GetSpecular();

	matAmbient->SetFloatVector( (float*)D3DXVECTOR4( ambient.x, ambient.y, ambient.z, ambient.w ) );
	matDiffuse->SetFloatVector( (float*)D3DXVECTOR4( diffuse.x, diffuse.y, diffuse.z, diffuse.w ) );
	matSpecular->SetFloatVector( (float*)D3DXVECTOR4( specular.x, specular.y, specular.z, specular.w ) );
	matShininess->SetFloat( material.GetShininess() );

	if (!material.GetTextureCount())
		textured->SetBool(false);
}

void MainPass::SingleSMShader::UniformCallBack(D3dEffectManager&, const Texture& texture,
											   ID3D11ShaderResourceView& resource,
											   float* texMtx)
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

void MainPass::SingleSMShader::UniformCallBack(D3dEffectManager&, const Transform& transform)
{
	world->SetMatrix(const_cast<float*>(&transform.GetMatrix().elem[0][0]));
}

/* MainPass::PSSMShader ****************************************************************/
const std::string MainPass::PSSMShader::PSSMSource("../shaders/RenderPSSM.fx");
const std::string MainPass::PSSMShader::NbrOfSplitsMacro("NBROFSPLITS");
const std::string MainPass::PSSMShader::PCFPresetMacro("PRESET");

MainPass::PSSMShader::PSSMShader(const std::string& name)
{
	SetName(name);
	AddMacro(NbrOfSplitsMacro);
	AddMacro(PCFPresetMacro);
}

bool MainPass::PSSMShader::Initialize(ID3DX11Effect& effect)
{
	if (!effect.GetTechniqueByName("RenderPSSM")->IsValid() ||
		!effect.GetTechniqueByName("RenderPSSMBBSel")->IsValid())
		return false;

	if (!InitializeVariables(effect))
		return false;

	splits = effect.GetVariableByName("g_Splits")->AsScalar();
	visSplits = effect.GetVariableByName("g_visSplits")->AsScalar();
	visSplits = effect.GetVariableByName("g_visSplits")->AsScalar();
	shadowRTSz = effect.GetVariableByName("g_shadowRTSz")->AsScalar();
	
	if (!splits->IsValid() || !visSplits->IsValid() || !shadowRTSz->IsValid())
		return false;

	return true;
}

/* MainPass::KeyBindings ***************************************************************/
const unsigned int MainPass::KeyBindings::AABBDrawing = 'B';
const unsigned int MainPass::KeyBindings::ToggleLightProj = 'P';
const unsigned int MainPass::KeyBindings::DrawBodyAABB = VK_F7;
const unsigned int MainPass::KeyBindings::DrawBody = VK_F8;
const unsigned int MainPass::KeyBindings::UseHierarchy = 'H';

MainPass::KeyBindings::KeyBindings() :
	drawDepthValues(false),
	drawShadowMap(false),
	showSplits(false),
	directionalLight(true),
	drawMiniMap(false),
	drawErrGraph(false),
	drawBodyAABB(false),
	drawBodyExact(false),
	showTexels(false),
	useHierarchy(false),
	showAABBs(false),
	aabbBodySolid(true),
	exactBodySolid(true),
	camFrustSolid(true),
	showSampling(false),
	showShadows(true),
	visStats(false),
	visError(0),
	useHWPCF(true),
	useBlockerSearch(true),
	lbramount(0.0),
	useLogBlur(false),
	visPCFRegions(false),
	visMipMap(false),
	maxsamples(64),
	shading(true)
{
}

/* MainPass ****************************************************************************/
MainPass::MainPass(Scene& scene, CDXUTTextHelper& txtHelper) :
	_scene(&scene),
	_txtHelper(&txtHelper),
	_renderSystem(0),
	_singlesmShader("MainPass::SingleSMShader"),
	_pssmShader("MainPass::PSSMShader"),
	_sceneId(0),
	_useBestResSplitSel(false),
	_rotateLight(false),
	_spectatorActive(false),
	_enlargedMiniMap(false),
	_shadeMiniMap(false),
	_miniMap(290, 20, 333, 250),
	_enlargedDepthMap(false),
	_shadeDepthMap(false),
	_depthMap(20, 20, 250, 250),
	_enlargedErrGraph(false),
	_shadeErrGraph(false),
	_errGraph(10, 758, 300, 250)
{
	D3DXMatrixIdentity(&_viewMat);
	D3DXMatrixIdentity(&_projMat);
	RegisterEvents();
}

MainPass::~MainPass()
{
	UnregisterEvents();
	
	if (_renderSystem)
	{
		_renderSystem->GetEffectManager()->Delete(_singlesmShader);
		_renderSystem->GetEffectManager()->Delete(_pssmShader);
	}
}

Camera& MainPass::GetCamera()
{
	return _spectator;
}

SkyDome& MainPass::GetSkyDomePass()
{
	return _skyPass;
}

D3dShadowMapping& MainPass::GetShadowMappingPass()
{
	return _shadowMapping;
}

bool MainPass::Initialize(RenderSystem& renderSystem, const Perspective& p, const View& v)
{
	if ((_renderSystem = dynamic_cast<D3dRenderSystem*>(&renderSystem)) == 0)
		return false;

	unsigned int filter = _shadowMapping.GetFilterMacroVal(_shadowMapping.GetFilterMode());
	_singlesmShader.SetFilename(SingleSMShader::SingleSMSource, Shader::CompleteSource);
	_singlesmShader.SetMacro(SingleSMShader::PCFPresetMacro, &filter, D3dEffect::Integer);
	if (!_renderSystem->GetEffectManager()->Generate(_singlesmShader, *_renderSystem))
		return false;

	unsigned int nbrOfSplits = _shadowMapping.GetNbrOfSplits();
	_pssmShader.SetFilename(PSSMShader::PSSMSource, Shader::CompleteSource);
	_pssmShader.SetMacro(PSSMShader::NbrOfSplitsMacro, &nbrOfSplits, D3dEffect::Integer);
	_pssmShader.SetMacro(PSSMShader::PCFPresetMacro, &filter, D3dEffect::Integer);
	if (!_renderSystem->GetEffectManager()->Generate(_pssmShader, *_renderSystem))
		return false;

	if (!_skyPass.Initialize(*_renderSystem))
		return false;

	tVec4f lPos(50.0f, 600.0f, 60.0f, 1.0f);
	//tVec4f lPos(10.0f, 80.0f, 10.0f, 1.0f);
	_light.SetPosition(lPos);
	_light.SetSpotDirection(tVec3f(.0f, .0f, .0f));

	_shadowMapping.SetCamera(_scene->GetCamera());
	_shadowMapping.SetLightView(_light.GetView());
	UpdateLightProj();
	if (!_shadowMapping.Initialize(*_renderSystem))
		return false;

	_shadowMapping.GetCamera().SetView(v);
	_shadowMapping.GetCamera().SetPerspective(p);

	_spectator.SetView(v);
	_spectator.SetPerspective(p);

	_effectlight.Position = D3DXVECTOR3(_light.GetPosition().x, _light.GetPosition().y, _light.GetPosition().z);
	_effectlight.Ambient  = D3DXVECTOR4(.1f, .1f, .1f, .1f);
	_effectlight.Diffuse  = D3DXVECTOR4(.8f, .8f, .8f, .8f);
	_effectlight.Specular = D3DXVECTOR4(.9f, .9f, .9f, .9f);

	return true;
}

void MainPass::SetupView(bool spectate)
{
	_renderSystem->SetProjMatrix(_scene->GetCamera().GetPerspective());
	
	if (spectate)
		_renderSystem->SetViewMatrix(_spectator.GetView());
	else
		_renderSystem->SetViewMatrix(_shadowMapping.GetCamera().GetView());
}

void MainPass::Render()
{
	if (!_nodes.size())
		return;

	_renderSystem->SetDrawBoundingBoxes(false);

	_shadowMapping.Render();

	ID3D11RenderTargetView* rtv = DXUTGetD3D11RenderTargetView();
    ID3D11DepthStencilView* dsv = DXUTGetD3D11DepthStencilView();

	DXUTGetD3D11DeviceContext()->ClearRenderTargetView( rtv, D3DXVECTOR4(.0f, .0f, .0f, 1.0f) );
    DXUTGetD3D11DeviceContext()->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	DXUTGetD3D11DeviceContext()->OMSetRenderTargets(1, &rtv, dsv);
	
	SetupView(_spectatorActive);

	_renderSystem->SetDrawBoundingBoxes(_keyBindings.showAABBs);

	_skyPass.Render();

	_nodes[_sceneId]->Cull(*_renderSystem);	

	if (_shadowMapping.GetNbrOfSplits() > 1)
		RenderPSSM(*_renderSystem->GetViewMatrix(), *_renderSystem->GetProjMatrix());
	else
		RenderSingleSM(*_renderSystem->GetViewMatrix(), *_renderSystem->GetProjMatrix());

	if (_spectatorActive)
		RenderDebugInfos();

	if (_keyBindings.drawShadowMap && !_enlargedMiniMap && !_enlargedErrGraph)
		RenderDepthMap(_enlargedDepthMap);

	//RenderSystem::Viewport vp = _renderSystem->GetViewport();
	//unsigned int size = 250;
	//unsigned int x = GetMapCoords(DepthMap);
	//unsigned int y = vp.height - 260;

	//RenderSystem::Viewport viewp = RenderSystem::Viewport(x, y, size, size);
	//_shadowMapping.DisplayDepthMaps(viewp);

	_keyBindings.drawErrGraph = _shadowMapping.GetCalcError() && (_shadowMapping.GetUseLiSP() || _shadowMapping.GetAlignmentLSVS());

	if (_keyBindings.drawErrGraph && !_enlargedDepthMap && !_enlargedMiniMap)
		RenderErrGraph(_enlargedErrGraph);
}

void MainPass::RenderSingleSM(float& view, float& proj)
{
	_renderSystem->GetEffectManager()->Use( _singlesmShader, "RenderSingleSM", GetPass() );

	_singlesmShader.view->SetMatrix(&view);
	_singlesmShader.projection->SetMatrix(&proj);
	
	D3DXMATRIX camview;
	_renderSystem->GetViewMatrix(camview, _shadowMapping.GetCamera().GetView());
	_singlesmShader.camView->SetMatrix(&camview.m[0][0]);

	_singlesmShader.effectLight->SetRawValue(&_effectlight, 0, sizeof(EffectLight));
	_singlesmShader.lightViewProj->SetMatrix(_shadowMapping.GetNormLightViewProjMtx());
	_singlesmShader.lightview->SetMatrix(_shadowMapping.GetLightViewMtx());
	_singlesmShader.visTexels->SetBool(_keyBindings.showTexels);
	_singlesmShader.visTexelSz->SetInt(_texelSz);
	_singlesmShader.visSampling->SetBool(_spectatorActive ? false : _keyBindings.showSampling);
	_singlesmShader.shadowMapSz->SetInt(_shadowMapping.GetShadowMapSize());
	_singlesmShader.lightRadius->SetFloatVector(_shadowMapping.GetSplitScales());
	_singlesmShader.renderShadows->SetBool(_keyBindings.showShadows);
	_singlesmShader.visError->SetInt(_keyBindings.visError);
	_singlesmShader.useHWPCF->SetInt(_keyBindings.useHWPCF);
	_singlesmShader.useBlockerSearch->SetInt(_keyBindings.useBlockerSearch);
	_singlesmShader.shading->SetBool(_keyBindings.shading);
	_singlesmShader.distributePrecision->SetBool(_keyBindings.distributePrecision);
	_singlesmShader.gammaCorrect->SetBool(_renderSystem->IsInGammaCorrectMode());
	_singlesmShader.evsmconst->SetFloatVector(_shadowMapping.GetEVSMConst());
	_singlesmShader.lbramount->SetFloat(_keyBindings.lbramount);
	_singlesmShader.useLogBlur->SetBool(_keyBindings.useLogBlur);
	_singlesmShader.filterWidth->SetInt(*_shadowMapping.GetFilterWidths());
	_singlesmShader.visPCFRegions->SetBool(_keyBindings.visPCFRegions);
	_singlesmShader.useMipMap->SetBool(_keyBindings.useMipMap);
	_singlesmShader.visMipMap->SetBool(_keyBindings.visMipMap);
	_singlesmShader.maxsamples->SetInt(_keyBindings.maxsamples);

	const tVec3f& cam = _spectatorActive ? _spectator.GetView().GetEye() : _shadowMapping.GetCamera().GetView().GetEye();
	_singlesmShader.camPos->SetFloatVector(D3DXVECTOR3(cam.x, cam.y, cam.z));

	const tVec3f viewVec(vmath::normalize(_shadowMapping.GetCamera().GetView().GetEye() - _shadowMapping.GetCamera().GetView().GetCenter()));
	_singlesmShader.viewVec->SetFloatVector(D3DXVECTOR3(viewVec.x, viewVec.y, viewVec.z));

	const tVec3f lightVec(vmath::normalize(_shadowMapping.GetLightView().GetEye() - _shadowMapping.GetLightView().GetCenter()));
	_singlesmShader.lightVec->SetFloatVector(D3DXVECTOR3(lightVec.x, lightVec.y, lightVec.z));
	
	ID3DX11EffectShaderResourceVariable* sm = _shadowMapping.GetFilterMode() == D3dShadowMapping::FilterMode::UINTSAVSM ? _singlesmShader.ishadowTex : _singlesmShader.shadowTex;
	_shadowMapping.GetShadowMaps().Bind(*_renderSystem, *sm);
	
	_nodes[_sceneId]->Render(*_renderSystem);

	_shadowMapping.GetShadowMaps().UnBind(*_renderSystem, *sm);	
}

void MainPass::RenderPSSM(float& view, float& proj)
{
	_renderSystem->GetEffectManager()->Use(	_pssmShader, _useBestResSplitSel ? "RenderPSSMBBSel" : "RenderPSSM", GetPass() );

	_pssmShader.view->SetMatrix(&view);
	_pssmShader.projection->SetMatrix(&proj);

	D3DXMATRIX camview;
	_renderSystem->GetViewMatrix(camview, _shadowMapping.GetCamera().GetView());
	_pssmShader.camView->SetMatrix(&camview.m[0][0]);

	unsigned int nbrOfSplits = _shadowMapping.GetNbrOfSplits();
	_pssmShader.effectLight->SetRawValue(&_effectlight, 0, sizeof(EffectLight));
	_pssmShader.lightViewProj->SetMatrixArray(_shadowMapping.GetNormLightViewProjMtx(), 0, nbrOfSplits);
	_pssmShader.lightview->SetMatrixArray(_shadowMapping.GetLightViewMtx(), 0, nbrOfSplits);
	float* splitpos = _shadowMapping.GetSplitPositions();
	_pssmShader.splits->SetFloatArray(splitpos, 0, nbrOfSplits);
	_pssmShader.visTexels->SetBool(_keyBindings.showTexels);
	_pssmShader.visTexelSz->SetInt(_texelSz);
	_pssmShader.visSampling->SetBool(_spectatorActive ? false : _keyBindings.showSampling);
	_pssmShader.visSplits->SetBool(_keyBindings.showSplits);
	_pssmShader.shadowMapSz->SetInt(_shadowMapping.GetShadowMapSize());
	_pssmShader.shadowRTSz->SetInt(_shadowMapping.GetRenderTargetSize());
	_pssmShader.lightRadius->SetFloatVectorArray(_shadowMapping.GetSplitScales(), 0, nbrOfSplits);
	_pssmShader.renderShadows->SetBool(_keyBindings.showShadows);
	_pssmShader.visError->SetInt(_keyBindings.visError);
	_pssmShader.useHWPCF->SetInt(_keyBindings.useHWPCF);
	_pssmShader.useBlockerSearch->SetInt(_keyBindings.useBlockerSearch);
	_pssmShader.shading->SetBool(_keyBindings.shading);
	_pssmShader.distributePrecision->SetBool(_keyBindings.distributePrecision);
	_pssmShader.gammaCorrect->SetBool(_renderSystem->IsInGammaCorrectMode());
	_pssmShader.evsmconst->SetFloatVector(_shadowMapping.GetEVSMConst());
	_pssmShader.lbramount->SetFloat(_keyBindings.lbramount);
	_pssmShader.useLogBlur->SetBool(_keyBindings.useLogBlur);
	_pssmShader.filterWidth->SetIntArray(_shadowMapping.GetFilterWidths(), 0, nbrOfSplits);
	_pssmShader.visPCFRegions->SetBool(_keyBindings.visPCFRegions);
	_pssmShader.useMipMap->SetBool(_keyBindings.useMipMap);
	_pssmShader.visMipMap->SetBool(_keyBindings.visMipMap);
	_pssmShader.maxsamples->SetInt(_keyBindings.maxsamples);

	const tVec3f& cam = _spectatorActive ? _spectator.GetView().GetEye() : _shadowMapping.GetCamera().GetView().GetEye();
	_pssmShader.camPos->SetFloatVector(D3DXVECTOR3(cam.x, cam.y, cam.z));

	const tVec3f viewVec(vmath::normalize(_shadowMapping.GetCamera().GetView().GetEye() - _shadowMapping.GetCamera().GetView().GetCenter()));
	_pssmShader.viewVec->SetFloatVector(D3DXVECTOR3(viewVec.x, viewVec.y, viewVec.z));

	const tVec3f lightVec(vmath::normalize(_shadowMapping.GetLightView().GetEye() - _shadowMapping.GetLightView().GetCenter()));
	_pssmShader.lightVec->SetFloatVector(D3DXVECTOR3(lightVec.x, lightVec.y, lightVec.z));

	ID3DX11EffectShaderResourceVariable* sm = _shadowMapping.GetFilterMode() == D3dShadowMapping::FilterMode::UINTSAVSM ? _pssmShader.ishadowTex : _pssmShader.shadowTex;
	_shadowMapping.GetShadowMaps().Bind(*_renderSystem, *sm);

	_nodes[_sceneId]->Render(*_renderSystem);

	_shadowMapping.GetShadowMaps().UnBind(*_renderSystem, *sm);
}

void MainPass::RenderDebugInfos()
{
	const tVec4f color[4] =
	{
		tVec4f(1.0f, .0f, .0f, .5f),
		tVec4f(1.0f, 1.0f, .0f, .5f),
		tVec4f(.0f, 1.0f, .0f, .5f),
		tVec4f(.0f, .0f, 1.0f, .5f)
	};

	_renderSystem->Draw(
		Line(_shadowMapping.GetLightView().GetEye(), _shadowMapping.GetLightView().GetCenter() - _shadowMapping.GetLightView().GetEye()),
		tVec4f(.0f, .0f, .0f, 1.0f));

	RenderSystem::PolygonMode mode = _renderSystem->GetPolygonMode();

	if (_keyBindings.drawCamFrustum)
	{
		if (_keyBindings.camFrustSolid)
			_renderSystem->SetPolygonMode(RenderSystem::Fill);
		else
			_renderSystem->SetPolygonMode(RenderSystem::Wireframe);

		if (_shadowMapping.GetSplitFrustums()[0])
			_renderSystem->Draw(*_shadowMapping.GetSplitFrustums()[0], tVec4f(.9f, .5f, .0f, .5f));
		
		unsigned int colorIndex = 0;
		for (unsigned int i = 1; i < _shadowMapping.GetNbrOfSplits() + 1; ++i, ++colorIndex)
			if (_shadowMapping.GetSplitFrustums()[i])
				_renderSystem->Draw(*_shadowMapping.GetSplitFrustums()[i], color[colorIndex%4]);
	}

	if (_keyBindings.drawBodyExact)
	{
		if (_keyBindings.exactBodySolid)
			_renderSystem->SetPolygonMode(RenderSystem::Fill);
		else
			_renderSystem->SetPolygonMode(RenderSystem::Wireframe);

		for (unsigned int i = 0; i < _shadowMapping.GetNbrOfSplits(); ++i)
		{
			_shadowMapping.GetBodyArray()[i]->CreateConvexHull();
			_renderSystem->Draw(*_shadowMapping.GetBodyArray()[i], color[i%4]);
		}
	}

	if (_keyBindings.drawBodyAABB)
	{
		if (_keyBindings.aabbBodySolid)
			_renderSystem->SetPolygonMode(RenderSystem::Fill);
		else
			_renderSystem->SetPolygonMode(RenderSystem::Wireframe);

		for (unsigned int i = 0; i < _shadowMapping.GetNbrOfSplits(); ++i)
			_renderSystem->Draw(*_shadowMapping.GetSplitBBs()[i], color[i%4]);
	}

	_renderSystem->SetPolygonMode(mode);

	if (_keyBindings.drawMiniMap && !_enlargedDepthMap && !_enlargedErrGraph)
		RenderMiniMap(_enlargedMiniMap);
}

unsigned int MainPass::GetMapCoords(Map map) const
{
	float aspect = static_cast<float>(_renderSystem->GetViewport().width)/_renderSystem->GetViewport().height;
	unsigned int result = 10;

	switch (map)
	{
		case DepthMap:
			// do nothing
			break;
		case ErrGraph:
			if (_keyBindings.drawShadowMap)
				result = 270;
			break;
		case MiniMap:
			if (_keyBindings.drawShadowMap)
				result = 270;
			if (_keyBindings.drawErrGraph)
				result = 20 + static_cast<unsigned int>(250 * aspect);
			if (_keyBindings.drawShadowMap && _keyBindings.drawErrGraph)
				result = 280 + static_cast<unsigned int>(250 * aspect);			
			break;
	}
	return result;
}

RenderSystem::Viewport* MainPass::GetMapVP(Map map)
{
	RenderSystem::Viewport* vp = 0;
	switch (map)
	{
		case DepthMap:
			vp = &_depthMap;
			break;
		case ErrGraph:
			vp = &_errGraph;
			break;
		case MiniMap:
			vp = &_miniMap;
			break;
	}

	return vp;
}

bool MainPass::IsEnlarged(Map map) const
{
	bool result = false;

	switch (map)
	{
		case DepthMap:
			result = _enlargedDepthMap;
			break;
		case ErrGraph:
			result = _enlargedErrGraph;
			break;
		case MiniMap:
			result = _enlargedMiniMap;
			break;
	}

	return result;
}

void MainPass::RenderMiniMap(bool enlarged)
{
	RenderSystem::Viewport vp = _renderSystem->GetViewport();
	
	bool showTexels = _keyBindings.showTexels;
	bool showSplits = _keyBindings.showSplits;
	bool showSampling = _keyBindings.showSampling;

	_keyBindings.showTexels = false;
	_keyBindings.showSplits = false;
	_keyBindings.showSampling = false;

	ID3D11DepthStencilView* dsv = DXUTGetD3D11DepthStencilView();

    DXUTGetD3D11DeviceContext()->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH, 1.0f, 0 );

	unsigned int height = 250;
	unsigned int width = static_cast<unsigned int>(250.0f * static_cast<float>(vp.width) / vp.height); 
	unsigned int x = GetMapCoords(MiniMap);
	unsigned int y = vp.height - 260;
	
	if (enlarged)
	{
		height = vp.height - 40;
		width = vp.width - 40;
		x = 20;
		y = 20;
	}

	_miniMap = RenderSystem::Viewport(x, y, width, height);
	
	_renderSystem->SetViewport(_miniMap);

	SetupView(false);

	_skyPass.Render();

	_nodes[_sceneId]->Cull(*_renderSystem);	

	if (_shadowMapping.GetNbrOfSplits() > 1)
		RenderPSSM(*_renderSystem->GetViewMatrix(), *_renderSystem->GetProjMatrix());
	else
		RenderSingleSM(*_renderSystem->GetViewMatrix(), *_renderSystem->GetProjMatrix());

	_renderSystem->SetViewport(vp);

	RenderSystem::CullState cull = _renderSystem->GetCullState();
	_renderSystem->SetCullState(RenderSystem::CullFrontFace);

	// just to switch the inputlayout
	_renderSystem->GetEffectManager()->Use(_shadowMapping.Get2dEffect(), 0, 0);

	if (_shadeMiniMap)
		_renderSystem->Draw(_miniMap, tVec4f(1.0f, 1.0f, .0f, .4f));

	if (enlarged)
		_renderSystem->Draw(_miniMap, vp, tVec4f(.0f, .0f, .0f, .9f));

	if (D3dUtils::IsInside(_cursor.x, _cursor.y, _miniMap.x, _miniMap.y, _miniMap.width, _miniMap.height))
	{
		_txtHelper->Begin();
		_txtHelper->SetInsertionPos((_miniMap.x + _miniMap.width/2) - 95, (_miniMap.y + _miniMap.height/2) - 10);
		_txtHelper->SetForegroundColor(D3DXCOLOR(.1f, .1f, .1f, 1.0f));
		_txtHelper->DrawTextLine(enlarged ? L"Double Click to minimize" : L"Double Click to enlarge");
		_txtHelper->End();
	}

	_renderSystem->SetCullState(cull);

	_keyBindings.showTexels = showTexels;
	_keyBindings.showSplits = showSplits;
	_keyBindings.showSampling = showSampling;
}

void MainPass::RenderDepthMap(bool enlarged)
{
	RenderSystem::Viewport vp = _renderSystem->GetViewport();
	
	bool showTexels = _keyBindings.showTexels;
	bool showSplits = _keyBindings.showSplits;
	bool showSampling = _keyBindings.showSampling;
	bool showShadows = _keyBindings.showShadows;

	_keyBindings.showTexels = false;
	_keyBindings.showSplits = false;
	_keyBindings.showSampling = false;
	_keyBindings.showShadows = false;

	unsigned int size = 250;
	unsigned int x = GetMapCoords(DepthMap);
	unsigned int y = vp.height - 260;
	
	if (enlarged)
	{
		size = vp.height - 40;
		x = (vp.width - size) / 2;
		y = 20;
	}

	_depthMap = RenderSystem::Viewport(x, y, size, size);
	
	_renderSystem->SetViewport(_depthMap);

	const int nbrOfSplits = static_cast<int>(_shadowMapping.GetNbrOfSplits());
	unsigned int sz = size;
	unsigned int factor = 2;

	if (nbrOfSplits == 1)
	{
		// do nothing 
	}
	else if (nbrOfSplits - 5 < 0)
	{
		// 2 - 4 splits
		sz = size / 2;
		factor = 4;
	}
	else if (nbrOfSplits - 10 < 0)
	{
		// 5 - 9 splits
		sz = size / 3;
		factor = 6;
	}
	else
	{
		// 10 - 16 splits
		sz = size / 4;	
		factor = 8;
	}

	std::vector<tVec2i> fontPos(nbrOfSplits);
	std::vector<tVec2i> vpPos(nbrOfSplits);
	tVec2i fontoffset(x + 6, y + 6);

	for (int i = 0; i < nbrOfSplits; ++i)
	{
		vpPos[i].x = x + sz * ( i % (factor / 2) );
		vpPos[i].y = y + sz * ( 2 * i / factor );

		fontPos[i].x = fontoffset.x + sz * ( i % (factor / 2) );
		fontPos[i].y = fontoffset.y + sz * ( 2 * i / factor );
	}

	RenderSystem::CullState cull = _renderSystem->GetCullState();
	_renderSystem->SetCullState(RenderSystem::CullFrontFace);

	// switch the inputlayout and draw a quad to clear the backbuffer
	_renderSystem->GetEffectManager()->Use(_shadowMapping.Get2dEffect(), 0, 0);
	_renderSystem->Draw(_depthMap, tVec4f(.9f, .9f, .9f, 1.0f));

	_renderSystem->SetCullState(RenderSystem::CullBackFace);

	unsigned int frustumIndex = 1;

	if(!_keyBindings.drawDepthValues)
	{
		for (unsigned int i = 0; i < _shadowMapping.GetNbrOfSplits(); ++i, ++frustumIndex)
		{
			RenderSystem::Viewport lightView(vpPos[i].x, vpPos[i].y, sz, sz);	
			_renderSystem->SetViewport(lightView);

			_renderSystem->SetProjMatrix(_shadowMapping.GetLightProjMtx(i));
			_renderSystem->SetViewMatrix(_shadowMapping.GetLightViewMtx());

			ID3D11DepthStencilView* dsv = DXUTGetD3D11DepthStencilView();
			DXUTGetD3D11DeviceContext()->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		
			_nodes[_sceneId]->Cull(*_renderSystem);

			RenderSingleSM(*_shadowMapping.GetLightViewMtx(), *_shadowMapping.GetLightProjMtx(i));

			_renderSystem->SetPolygonMode(RenderSystem::Wireframe);
			_renderSystem->Draw(*_shadowMapping.GetSplitFrustums()[frustumIndex], tVec4f(.0f, .0f, .0f, 1.0f));
			_renderSystem->SetPolygonMode(RenderSystem::Fill);

			if (i == 0)
			{
				const View& view = _shadowMapping.GetCamera().GetView();
				const float dist = _shadowMapping.GetSplitPos()[1] - _shadowMapping.GetSplitPos()[0];
				Line line(view.GetEye(), 0.75f * dist * (view.GetCenter() - view.GetEye()));
				_renderSystem->Draw(line, tVec4f(.0f, .0f, 1.0f, 1.0f));
			}
		}
	}
	else
	{
		_shadowMapping.DisplayDepthMaps(_depthMap);
	}

	_renderSystem->SetViewport(vp);

	_renderSystem->SetCullState(RenderSystem::CullFrontFace);

	// just to switch the inputlayout
	_renderSystem->GetEffectManager()->Use(_shadowMapping.Get2dEffect(), 0, 0);

	_txtHelper->Begin();
	_txtHelper->SetForegroundColor(D3DXCOLOR(.1f, .1f, .1f, 1.0f));

	for (int i = 0; i < nbrOfSplits; ++i)
	{
		_txtHelper->SetInsertionPos(fontPos[i].x, fontPos[i].y);
		_txtHelper->DrawFormattedTextLine(L"%d", i+1);
	}
	_txtHelper->End();

	if (_shadeDepthMap)
		_renderSystem->Draw(_depthMap, tVec4f(1.0f, 1.0f, .0f, .4f));

	if (enlarged)
		_renderSystem->Draw(_depthMap, vp, tVec4f(.0f, .0f, .0f, .9f));

	if (D3dUtils::IsInside(_cursor.x, _cursor.y, _depthMap.x, _depthMap.y, _depthMap.width, _depthMap.height))
	{
		_txtHelper->Begin();
		_txtHelper->SetInsertionPos((_depthMap.x + _depthMap.width/2) - 95, (_depthMap.y + _depthMap.height/2) - 10);
		_txtHelper->SetForegroundColor(D3DXCOLOR(.1f, .1f, .1f, 1.0f));
		_txtHelper->DrawTextLine(enlarged ? L"Double Click to minimize" : L"Double Click to enlarge");
		_txtHelper->End();
	}

	//RenderSystem::Viewport viewp(20, vp.height - 590, 250, 250);
	//_shadowMapping.DisplayDepthMaps(viewp);
	//_shadowMapping.DisplayMap(viewp, _samplingMap);

	_renderSystem->SetCullState(cull);

	_keyBindings.showTexels = showTexels;
	_keyBindings.showSplits = showSplits;
	_keyBindings.showSampling = showSampling;
	_keyBindings.showShadows = showShadows;
}

void MainPass::RenderErrGraph(bool enlarged)
{	
	//const tVec4f curveClr[4] =
	//{
	//	tVec4f(.0f, .7f, .0f, 1.0f),
	//	tVec4f(1.0f, .3f, .0f, 1.0f),
	//	tVec4f(.2f, .5f, 1.0f, 1.0f),
	//	tVec4f(.6f, .6f, .6f, 1.0f)
	//};
	//tVec4f bgClr(.0f, .0f, .0f, .9f);
	//tVec4f axisClr(1.0f, 1.0f, 1.0f, 1.0f);
	//tVec4f shdwClr(.0f, .0f, .0f, 1.0f);
	//tVec4f bgBarClr(.0f, .0f, .0f, .4f);
	//tVec4f pnClr(1.0f, .0f, .0f, 1.0f);
	//const tVec4f splitClr[4] =
	//{
	//	tVec4f(1.0f, .0f, .0f, .15f),
	//	tVec4f(1.0f, 1.0f, .0f, .15f),
	//	tVec4f(.0f, 1.0f, .0f, .15f),
	//	tVec4f(.0f, .0f, 1.0f, .15f)
	//};

	const tVec4f curveClr[4] =
	{
		tVec4f(.0f, .7f, .0f, 1.0f),
		tVec4f(0.7f, .2f, .0f, 1.0f),
		tVec4f(.0f, .2f, 1.0f, 1.0f),
		tVec4f(.6f, .6f, .6f, 1.0f)
	};
	//tVec4f bgClr(1.0f, 1.0f, 1.0f, 0.75f);
	tVec4f bgClr(1.0f, 1.0f, 1.0f, 1.0f);
	tVec4f axisClr(.1f, .1f, .1f, 1.0f);
	tVec4f shdwClr(.0f, .0f, .0f, .0f);
	tVec4f bgBarClr(.0f, .0f, .0f, .5f);
	tVec4f pnClr(1.0f, .0f, .0f, 1.0f);
	const tVec4f splitClr[4] =
	{
		tVec4f(1.0f, .0f, .0f, .3f),
		tVec4f(1.0f, 1.0f, .0f, .3f),
		tVec4f(.0f, 1.0f, .0f, .3f),
		tVec4f(.0f, .0f, 1.0f, .3f)
	};


	RenderSystem::Viewport vp = _renderSystem->GetViewport();
	RenderSystem::CullState cull = _renderSystem->GetCullState();

	tVec2u size = tVec2u(static_cast<unsigned int>(250 * static_cast<float>(vp.width) / vp.height), 250);
	unsigned int x = GetMapCoords(ErrGraph);
	unsigned int y = vp.height - 260;
	
	if (enlarged)
	{
		size.y = vp.height - 40;
		size.x = static_cast<unsigned int>(size.y * static_cast<float>(vp.width) / vp.height);
		x = (vp.width - size.x) / 2;
		y = 20;
	}

	unsigned int nbrOfSplits = _shadowMapping.GetNbrOfSplits();
	
	_renderSystem->SetCullState(RenderSystem::CullFrontFace);

	_renderSystem->GetEffectManager()->Use(_shadowMapping.Get2dEffect(), 0, 0);

	_errGraph = RenderSystem::Viewport(x, y, size.x, size.y);
	_renderSystem->Draw(_errGraph, bgClr);

	RenderSystem::Viewport bgBar(x + 5, y + _errGraph.height - 33, size.x - 10, 28);
	_renderSystem->Draw(bgBar, bgBarClr);

	tVec2i graphOffset(x + 35, y + 30);
	unsigned int grX = graphOffset.x;
	unsigned int grY = graphOffset.y;
	unsigned int width = size.x - 115;
	unsigned int height = size.y - 130;

	// legend
	for (unsigned int i = 1; i < D3dShadowMapping::ErrDirCOUNT; ++i)
		//_renderSystem->Draw(RenderSystem::Viewport(grX + 30 + 135 * i, y + size.y - 20, 30, 2), curveClr[i]);
		_renderSystem->Draw(RenderSystem::Viewport(grX + 25 + 135 * (i-1), y + size.y - 21, 30, 4), curveClr[i]);

	// draw y-axis
	RenderSystem::Viewport yAxis(grX + 30, grY, 2, height);
	_renderSystem->Draw(yAxis, axisClr);
	
	// draw x-axis
	RenderSystem::Viewport xAxis(yAxis.x, grY + height, width, 2);
	_renderSystem->Draw(xAxis, axisClr);

	RenderSystem::Viewport area(yAxis.x + 2, yAxis.y, xAxis.width - 2, yAxis.height);
	//_renderSystem->Draw(area, tVec4f(1.0f, .0f, .0f, 1.0f));

	const D3dShadowMapping::tErrValsArray& values = _shadowMapping.GetErrorValues();
	const D3dShadowMapping::tDistArray& ranges = _shadowMapping.GetErrorRanges();

	float scaleX = width / _shadowMapping.GetDepthRange().y;
	float scaleY = FLT_MAX;

	for (unsigned int i = 0; i < ranges.size(); ++i)
		if ((_keyBindings.visPerspError & static_cast<unsigned int>(powf(2.0f, static_cast<float>(i)))) == powf(2.0f, static_cast<float>(i)))
			scaleY = min(scaleY, height / ranges[i].y);

	const D3dShadowMapping::tPosArray& splits = _shadowMapping.GetSplitPos();
	const D3dShadowMapping::tSplitSmpleArray& splitPosSamples = _shadowMapping.GetSplitPosSamples();

	for (unsigned int i = 0; i < splits.size(); ++i)
	{
		unsigned int step = static_cast<unsigned int>(scaleX * splits[i]);
		
		// draw labels of split distances on x-axis
		_txtHelper->Begin();
		_txtHelper->SetForegroundColor(D3DXCOLOR(axisClr.x,axisClr.y,axisClr.z,axisClr.w));
		_txtHelper->SetInsertionPos(yAxis.x + step - 5, xAxis.y + 8);
		_txtHelper->DrawFormattedTextLine(L"%.0f", splits[i]);
		
		unsigned int line = 0;
		for (unsigned int dir = 1; dir < D3dShadowMapping::ErrDirCOUNT; ++dir)
		{
			if ((_keyBindings.visPerspError & static_cast<unsigned int>(powf(2.0f, static_cast<float>(dir)))) != powf(2.0f, static_cast<float>(dir)))
				continue;
			
			line++;
			_txtHelper->SetForegroundColor(D3DXCOLOR(curveClr[dir].x,curveClr[dir].y,curveClr[dir].z,curveClr[dir].w));

			if (i != 0 && i < splits.size() - 1)
			{
				_txtHelper->SetInsertionPos(yAxis.x + step - 40, xAxis.y + 8 + 18 * line);
				_txtHelper->DrawFormattedTextLine(L"(%.2f/%.2f)", values[dir][splitPosSamples[i]].x, values[dir][splitPosSamples[i]+1].x);
			}
			else
			{
				_txtHelper->SetInsertionPos(yAxis.x + step - 15, xAxis.y + 8 + 18 * line);
				_txtHelper->DrawFormattedTextLine(L"(%.2f)", values[dir][splitPosSamples[i]].x);
			}
			
		}
		
		_txtHelper->End();

		RenderSystem::Viewport splitVP(yAxis.x + step, xAxis.y - 5, 2, 11);
		_renderSystem->Draw(splitVP, axisClr);
	}

	// draw x-axis lables of pseudo near and pseudo far
	const bool pseudoFAct = nbrOfSplits > 1 || (_shadowMapping.GetUseLiSP() && _shadowMapping.GetLiSP().GetMode() == LiSP::NOpt);
	const bool pseudoNAct = nbrOfSplits > 1 || (_shadowMapping.GetUseLiSP() && _shadowMapping.GetLiSP().GetMode() == LiSP::PseudoNear);

	if ((pseudoFAct || pseudoNAct) && (_shadowMapping.GetPseudoNear() > .0f || _shadowMapping.GetPseudoFar() > .0f))
	{
		bool pn = _shadowMapping.GetPseudoNear() > .0f && (pseudoNAct || pseudoFAct);
		bool pf = _shadowMapping.GetPseudoFar() > .0f && pseudoFAct;

		const D3dShadowMapping::tSplitSmpleArray& pseudoNFSamples = _shadowMapping.GetPseudoNFSamples();

		for (unsigned int i = 0; i < pseudoNFSamples.size(); ++i)
		{
			if (i == 0 && !pn)
				continue;
			if (i == pseudoNFSamples.size() - 1 && !pf)
				continue;

			float z = values[1][pseudoNFSamples[i]].y;
			unsigned int step = static_cast<unsigned int>(scaleX * z);
		
			// draw labels of split distances on x-axis
			
			_txtHelper->Begin();
			_txtHelper->SetForegroundColor(D3DXCOLOR(pnClr.x,pnClr.y,pnClr.z,pnClr.w));
			_txtHelper->SetInsertionPos(yAxis.x + step - 20, xAxis.y + 8);
			_txtHelper->DrawFormattedTextLine(L"%.0f (%s)", z, i == 0 ? L"PN" : L"PF");
				
			unsigned int line = 0;
			for (unsigned int dir = 1; dir < D3dShadowMapping::ErrDirCOUNT; ++dir)
			{
				if ((_keyBindings.visPerspError & static_cast<unsigned int>(powf(2.0f, static_cast<float>(dir)))) != powf(2.0f, static_cast<float>(dir)))
					continue;

				line++;
				_txtHelper->SetForegroundColor(D3DXCOLOR(curveClr[dir].x,curveClr[dir].y,curveClr[dir].z,curveClr[dir].w));
				_txtHelper->SetInsertionPos(yAxis.x + step - 15, xAxis.y + 8 + 18 * line);
				_txtHelper->DrawFormattedTextLine(L"(%.2f)", values[dir][pseudoNFSamples[i]].x);
			}

			_txtHelper->End();

			RenderSystem::Viewport splitVP(yAxis.x + step, xAxis.y - 5, 2, 11);
			_renderSystem->Draw(splitVP, pnClr);
		}
	}

	// draw labels of max and min error on y-axis
	_txtHelper->Begin();
	for (unsigned int i = 1; i < D3dShadowMapping::ErrDirCOUNT; ++i)
	{
		if ((_keyBindings.visPerspError & static_cast<unsigned int>(powf(2.0f, static_cast<float>(i)))) != powf(2.0f, static_cast<float>(i)))
			continue;

		_txtHelper->SetForegroundColor(D3DXCOLOR(curveClr[i].x,curveClr[i].y,curveClr[i].z,curveClr[i].w));
		_txtHelper->SetInsertionPos(yAxis.x - max(1, 1 + static_cast<int>(logf(ranges[i].y)/logf(10.0f))) * 10 - 32,
			static_cast<int>(yAxis.y + yAxis.height - scaleY * ranges[i].y - 9));
		_txtHelper->DrawFormattedTextLine(L"%.2f", ranges[i].y);
		_txtHelper->SetInsertionPos(yAxis.x - max(1, 1 + static_cast<int>(logf(ranges[i].x)/logf(10.0f))) * 10 - 32,
			static_cast<int>(yAxis.y + yAxis.height - scaleY * ranges[i].x - 9));
		_txtHelper->DrawFormattedTextLine(L"%.2f", ranges[i].x);
	}
	_txtHelper->End();

	for (unsigned int i = 1; i < D3dShadowMapping::ErrDirCOUNT; ++i)
	{
		if ((_keyBindings.visPerspError & static_cast<unsigned int>(powf(2.0f, static_cast<float>(i)))) != powf(2.0f, static_cast<float>(i)))
			continue;

		// maximum
		RenderSystem::Viewport errVP(yAxis.x - 5,  static_cast<unsigned int>(yAxis.y + yAxis.height - scaleY * ranges[i].y), 11, 2);
		_renderSystem->Draw(errVP, curveClr[i]);
		
		// minimum
		errVP = RenderSystem::Viewport(yAxis.x - 5, static_cast<unsigned int>(yAxis.y + yAxis.height - scaleY * ranges[i].x), 11, 2);
		_renderSystem->Draw(errVP, curveClr[i]);
	}

	RenderSystem::tLineList list[D3dShadowMapping::ErrDirCOUNT];
	std::vector<float> splitBorders(nbrOfSplits+1);

	// build list of screen space lines
	for (unsigned int i = 0; i < D3dShadowMapping::ErrDirCOUNT; ++i)
	{
		if ((_keyBindings.visPerspError & static_cast<unsigned int>(powf(2.0f, static_cast<float>(i)))) != powf(2.0f, static_cast<float>(i)))
			continue;

		for (unsigned int j = 0; j < values[i].size(); ++j)
		{
			if (j + 1 < values[i].size() && values[i][j].z != -1 && values[i][j+1].z != -1)
			{
				tVec3f start(values[i][j].y * scaleX, height - (values[i][j].x * scaleY), .0f);
				tVec3f end(values[i][j+1].y * scaleX, height - (values[i][j+1].x * scaleY), .0f);
				list[i].push_back(new Line(start, end - start));
			}
		}
	}

	for (unsigned int i = 0; i < values[0].size(); ++i)
	{
		if (nbrOfSplits > 1 && static_cast<unsigned int>(values[0][i].z) < nbrOfSplits)
		{
			unsigned int split = static_cast<unsigned int>(values[0][i].z);
			splitBorders[split+1] = max(values[0][i].y, splitBorders[split+1]);
		}
	}

	// color split zones in graph
	if (_keyBindings.showSplits)
	{
		splitBorders[0] = _shadowMapping.GetDepthRange().x;
		if (nbrOfSplits > 1)
		{
			for (unsigned int i = 0; i < nbrOfSplits; ++i)
			{
				unsigned int splitX = static_cast<unsigned int>(splitBorders[i] * scaleX);
				unsigned int splitWidth = static_cast<unsigned int>(splitBorders[i+1] * scaleX) - splitX;
				RenderSystem::Viewport splitVP(yAxis.x + splitX, grY, splitWidth, height);

				if (splitVP.x > 0 && splitVP.x <= yAxis.x + area.width &&
					splitVP.y > 0 && splitVP.y <= yAxis.y + area.height &&
					splitVP.width > 0 && splitVP.width <= area.width &&
					splitVP.height > 0 && splitVP.height <= area.height)
					_renderSystem->Draw(splitVP, splitClr[i%4]);
			}
		}
	}

	_renderSystem->GetEffectManager()->Use(_singlesmShader, 0, 0);
	for (unsigned int i = 0; i < D3dShadowMapping::ErrDirCOUNT; ++i)
	{
		if ((_keyBindings.visPerspError & static_cast<unsigned int>(powf(2.0f, static_cast<float>(i)))) != powf(2.0f, static_cast<float>(i)))
			continue;
		_renderSystem->Draw(list[i], curveClr[i], area);
	}

	for (unsigned int i = 0; i < D3dShadowMapping::ErrDirCOUNT; ++i)
	{
		if ((_keyBindings.visPerspError & static_cast<unsigned int>(powf(2.0f, static_cast<float>(i)))) != powf(2.0f, static_cast<float>(i)))
			continue;
		for (unsigned int j = 0; j < list[i].size(); ++j)
			delete list[i][j];
	}

	// just to switch the inputlayout
	_renderSystem->GetEffectManager()->Use(_shadowMapping.Get2dEffect(), 0, 0);

	if (_shadeErrGraph)
		_renderSystem->Draw(_errGraph, tVec4f(1.0f, 1.0f, .0f, .4f));

	if (enlarged)
		_renderSystem->Draw(_errGraph, vp, tVec4f(.0f, .0f, .0f, .9f));

	if (D3dUtils::IsInside(_cursor.x, _cursor.y, _errGraph.x, _errGraph.y, _errGraph.width, _errGraph.height))
	{
		_txtHelper->Begin();
		_txtHelper->SetInsertionPos((_errGraph.x + _errGraph.width/2) - 95, (_errGraph.y + _errGraph.height/2) - 10);
		_txtHelper->SetForegroundColor(D3DXCOLOR(.1f, .1f, .1f, 1.0f));
		_txtHelper->DrawTextLine(enlarged ? L"Double Click to minimize" : L"Double Click to enlarge");
		_txtHelper->End();
	}

	_renderSystem->SetCullState(cull);
	_renderSystem->SetViewport(vp);
}

void MainPass::UpdateLightProj()
{
	if (!_nodes.size() || !_nodes[_sceneId])
		return;

	BoundingBox sceneAABB;
	sceneAABB.Expand(*_nodes[_sceneId]);

	float maxZ = .0f, minZ = FLT_MAX, maxDim = .0f;

	const View& view = _light.GetView();
	const tVec3f eye(view.GetEye());
	const tVec3f viewDir(vmath::normalize(view.GetCenter() - eye));
	const tVec3f upVec(vmath::normalize(view.GetUp()));
	const tVec3f downVec(-upVec);
	const tVec3f leftVec(vmath::normalize(vmath::cross(viewDir, upVec)));
	const tVec3f rightVec(-leftVec);

	// for each point in AABB
	for (unsigned int i = 0; i < BoundingBox::CornerCOUNT; ++i)
	{
		const tVec3f corner = sceneAABB.GetCorner(static_cast<BoundingBox::Corner>(i));
		const tVec3f vPointToCam = corner - eye;

		const float dist = vmath::dot(vPointToCam, viewDir);
		maxZ = max(dist, maxZ);
		minZ = min(dist, minZ);
		
		maxDim = max(abs(vmath::dot(vPointToCam, leftVec)), maxDim);
		maxDim = max(abs(vmath::dot(vPointToCam, rightVec)), maxDim);
		maxDim = max(abs(vmath::dot(vPointToCam, upVec)), maxDim);
		maxDim = max(abs(vmath::dot(vPointToCam, downVec)), maxDim);
	}

	const float zNear = max(minZ, 1.0f);
	const float zFar  = max(maxZ, zNear + 1.0f);

	if (_keyBindings.directionalLight)
	{
		Ortho3d proj(-maxDim, maxDim, -maxDim, maxDim, zNear, zFar);
		_shadowMapping.SetLightProjection(proj);
	}
	else
	{
		const float fov = 2 * acosf(zNear / sqrtf(powf(zNear, 2) + powf(maxDim, 2)));
		Perspective proj(static_cast<float>(RAD2DEG(fov)), 1, zNear, zFar);
		_shadowMapping.SetLightProjection(proj);
	}
}

unsigned int MainPass::GetSceneId() const
{
	return _sceneId;
}

void MainPass::SetSceneId(unsigned int id)
{
	if (id >= _nodes.size())
		return;

	_sceneId = id;

	UpdateLightProj();	

	_shadowMapping.UseScene(_sceneId);
}

unsigned int MainPass::GetNbrOfSplits() const
{
	return _shadowMapping.GetNbrOfSplits();
}

void MainPass::SetNbrOfSplits(unsigned int nbrOfSplits)
{
	if (_shadowMapping.GetNbrOfSplits() == nbrOfSplits)
		return;

	_shadowMapping.SetNbrOfSplits(nbrOfSplits);
	
	if (!_renderSystem)
		return;

	if (nbrOfSplits > 1)
	{
		unsigned int filter = _shadowMapping.GetFilterMacroVal(_shadowMapping.GetFilterMode());

		_renderSystem->GetEffectManager()->Delete(_pssmShader);
		_pssmShader.SetMacro(PSSMShader::PCFPresetMacro, &filter, D3dEffect::Integer);
		_pssmShader.SetMacro(PSSMShader::NbrOfSplitsMacro, &nbrOfSplits, D3dEffect::Integer);
		_renderSystem->GetEffectManager()->Generate(_pssmShader, *_renderSystem);
	}	
}

D3dShadowMapping::FilterMode MainPass::GetFilterMode() const
{
	return _shadowMapping.GetFilterMode();
}

void MainPass::SetFilterMode(D3dShadowMapping::FilterMode mode)
{
	// check if recompile necessary
	bool recompile = _shadowMapping.CheckRecompile(mode);
	_shadowMapping.SetFilterMode(mode);

	if (!recompile)
		return;

	_shadowMapping.Compile();

	// recompile shader programs
	unsigned int filter = _shadowMapping.GetFilterMacroVal(mode);
	unsigned int nbrOfSplits = _shadowMapping.GetNbrOfSplits();

	_renderSystem->GetEffectManager()->Delete(_singlesmShader);
	_singlesmShader.SetMacro(SingleSMShader::PCFPresetMacro, &filter, D3dEffect::Integer);
	_renderSystem->GetEffectManager()->Generate(_singlesmShader, *_renderSystem);
	
	if (nbrOfSplits > 1)
	{
		_renderSystem->GetEffectManager()->Delete(_pssmShader);
		_pssmShader.SetMacro(PSSMShader::PCFPresetMacro, &filter, D3dEffect::Integer);
		_pssmShader.SetMacro(PSSMShader::NbrOfSplitsMacro, &nbrOfSplits, D3dEffect::Integer);
		_renderSystem->GetEffectManager()->Generate(_pssmShader, *_renderSystem);
	}
}

MainPass::KeyBindings& MainPass::GetKeyBindings()
{
	return _keyBindings;
}

int MainPass::GetVisTexelSz() const
{
	return _texelSz;
}

void MainPass::SetVisTexelSz(int size)
{
	_texelSz = size;
}

bool MainPass::GetBestResSplitSel() const
{
	return _useBestResSplitSel;
}

void MainPass::SetBestResSplitSel(bool enable)
{
	_useBestResSplitSel = enable;
}

bool MainPass::GetSpectatorActive() const
{
	return _spectatorActive;
}

void MainPass::SetSpectatorActive(bool enable)
{
	//if (enable)
	//	_spectator.SetView(_shadowMapping.GetCamera().GetView());
	
	_spectatorActive = enable;
	_shadowMapping.SetDebug(enable);
}

unsigned int MainPass::GetPass() const
{
	unsigned int result = 0;
	switch (_shadowMapping.GetFilterMode())
	{
		case D3dShadowMapping::None:
			result = 0;
			break;
		case D3dShadowMapping::HWPCF:
			result = 1;
			break;
		case D3dShadowMapping::Box4x4PCF:
		case D3dShadowMapping::Box6x6PCF:
		case D3dShadowMapping::Box8x8PCF:
			result = 2;
			break;
		case D3dShadowMapping::Poisson25PCF:
		case D3dShadowMapping::Poisson32PCF:
		case D3dShadowMapping::Poisson64PCF:
			result = 3;
			break;
		case D3dShadowMapping::PCFPCSS:
			result = 4;
			break;
		case D3dShadowMapping::ESM:
			result = 5;
			break;
		case D3dShadowMapping::VSM:
			result = 6;
			break;
		case D3dShadowMapping::EVSM:
			result = 7;
			break;
		case D3dShadowMapping::VarBoxPCF:
			result = 8;
			break;
		case D3dShadowMapping::SAVSM:
			result = 9;
			break;
		case D3dShadowMapping::UINTSAVSM:
			result = 10;
			break;
	};

	return result;
}

const std::string MainPass::GetStats()
{
	const unsigned int size = 256;
	char text[size];
	std::stringstream out;

	//if (_keyBindings.useHierarchy)
	//	out << "Hierarchical frustum culling\n";
	//else
	//	out << "Standard frustum culling\n";

	if (_shadowMapping.GetUseLiSP() && _shadowMapping.GetLiSP().GetMode() == LiSP::NOpt)
		out << "NOpt LiSP ";
	else if (_shadowMapping.GetUseLiSP() && _shadowMapping.GetLiSP().GetMode() == LiSP::Reparametrized)
		out << "Reparametrized LiSP ";
	else if (_shadowMapping.GetUseLiSP() && _shadowMapping.GetLiSP().GetMode() == LiSP::Arbitrary)
		out << "Arbitrarily warped ";
	else if (_shadowMapping.GetUseLiSP() && _shadowMapping.GetLiSP().GetMode() == LiSP::PseudoNear)
		out << "Pseudonear warped ";
	else
		out << "Uniform ";

	switch(_shadowMapping.GetFilterMode())
	{
		case D3dShadowMapping::None:
			out << "Shadowmapping ";
			break;
		case D3dShadowMapping::HWPCF:
			out << "Hardware PCF filtered Shadowmapping ";
			break;
		case D3dShadowMapping::Box4x4PCF:
			out << "Regular 4x4 PCF filtered Shadowmapping ";
			break;
		case D3dShadowMapping::Poisson25PCF:
			out << "Poisson 25x PCF filtered Shadowmapping ";
			break;
		case D3dShadowMapping::Box6x6PCF:
			out << "Regular 6x6 PCF filtered Shadowmapping ";
			break;
		case D3dShadowMapping::Poisson32PCF:
			out << "Poisson 32x PCF filtered Shadowmapping ";
			break;
		case D3dShadowMapping::Box8x8PCF:
			out << "Regular 8x8 PCF filtered Shadowmapping ";
			break;
		case D3dShadowMapping::Poisson64PCF:
			out << "Poisson 64x PCF filtered Shadowmapping ";
			break;
		case D3dShadowMapping::PCFPCSS:
			out << "64x Poisson PCF filtered Percentage Closer Soft Shadows ";
			break;
		case D3dShadowMapping::ESM:
			out << "Exponential Shadowmapping ";
			break;
		case D3dShadowMapping::VSM:
			out << "Variance Shadowmapping ";
			break;
		case D3dShadowMapping::EVSM:
			out << "Exponential Variance Shadowmapping ";
			break;
		case D3dShadowMapping::VarBoxPCF:
			{
				int kernelsz = 2 * _shadowMapping.GetBlurRadius() + 1;
				sprintf_s(text, 50, "Regular %ix%i PCF filtered Shadowmapping ", kernelsz, kernelsz);
				out << text;
				break;
			}
		case D3dShadowMapping::SAVSM:
			out << "Summed-Area Variance Shadowmapping fp32 ";
			break;
		case D3dShadowMapping::UINTSAVSM:
			out << "Summed-Area Variance Shadowmapping int32 ";
			break;
	}

	unsigned int nbrOfSplits = _shadowMapping.GetNbrOfSplits();

	if (nbrOfSplits > 1)
		out << "( " << _shadowMapping.GetNbrOfSplits() << " splits )";

	if (_keyBindings.directionalLight)
		out << "\nDirectional Light ";
	else
		out << "\nPoint Light ";

	sprintf_s(text, size, "( LightDir: %.f ", _shadowMapping.GetAngleCamDirLightDir());
	out << text;

	if (_shadowMapping.GetUseLiSP() && _shadowMapping.GetLiSP().GetMode() == LiSP::Reparametrized)
	{
		sprintf_s(text, size, "/ Parameter eta = %.2f ", _shadowMapping.GetLiSP().GetParam());
		out << text;
	}

	out << ")\n";

	const Perspective& p = _shadowMapping.GetCamera().GetPerspective();
	sprintf_s(text, size, "Field of view: %.f / Aspect ratio: %.2f\n", p.GetFOV(), p.GetAspect());
	out << text;

	const bool pseudoFAct = nbrOfSplits > 1 || (_shadowMapping.GetUseLiSP() && _shadowMapping.GetLiSP().GetMode() == LiSP::NOpt);
	const bool pseudoNAct = nbrOfSplits > 1 || (_shadowMapping.GetUseLiSP() && _shadowMapping.GetLiSP().GetMode() == LiSP::PseudoNear);

	float zNear = _shadowMapping.GetDepthRange().x;
	float zFar = _shadowMapping.GetDepthRange().y;
	float zNearPseudo = zNear + _shadowMapping.GetPseudoNear() * (zFar - zNear);
	float zFarPseudo = zFar - _shadowMapping.GetPseudoFar() * (zFar - zNear);

	if (pseudoFAct)
		sprintf_s(text, size, "zNear: %.2f ( Pseudo near: %.2f ) / zFar: %.2f ( Pseudo far: %.2f )", zNear, zNearPseudo, zFar, zFarPseudo);
	else if (pseudoNAct)
		sprintf_s(text, size, "zNear: %.2f ( Pseudo near: %.2f ) / zFar: %.2f", zNear, zNearPseudo, zFar);
	else
		sprintf_s(text, size, "zNear: %.2f / zFar: %.2f", zNear, zFar);
	
	out << text << "\n";
	
	if (_shadowMapping.GetUseLiSP())
	{
		LiSP::Mode mode = _shadowMapping.GetLiSP().GetMode();

		const LiSP::tParamArray& dist = _shadowMapping.GetLiSP().GetDistances();

		const char *lispTextSplits = 0, *lispText = 0;
		unsigned int first = 0, second = 0, last = 0;

		if (mode == LiSP::NOpt)
		{
			//lispTextSplits = "Split %d: n = %.2f ( NRepar = %.2f / N arbitrary = %.2f / N pseudo near = %.2f )\n";
			lispTextSplits = "Split %d: n = %.2f ( NRepar = %.2f / N arbitrary = %.2f )\n";
			//lispText = "n = %.2f ( NRepar = %.2f / N arbitrary = %.2f / N pseudo near = %.2f )\n";
			lispText = "n = %.2f ( NRepar = %.2f / N arbitrary = %.2f )\n";
			first = LiSP::Reparametrized;
			second = LiSP::Arbitrary;
			last = LiSP::PseudoNear;

		}
		else if (mode == LiSP::Reparametrized)
		{
			//lispTextSplits = "Split %d: n = %.2f ( NOpt = %.2f / N arbitrary = %.2f )\n";
			lispTextSplits = "Split %d: n = %.2f ( NOpt = %.2f / N arbitrary = %.2f / N pseudo near = %.2f )\n";
			//lispText = "n = %.2f ( NOpt = %.2f / N arbitrary = %.2f / N pseudo near = %.2f )\n";
			lispText = "n = %.2f ( NOpt = %.2f / N arbitrary = %.2f )\n";
			first = LiSP::NOpt;
			second = LiSP::Arbitrary;
			last = LiSP::PseudoNear;
		}
		else /* if (mode == LiSP::Arbitrary) */
		{
			//lispTextSplits = "Split %d: n = %.2f ( NOpt = %.2f / NRepar = %.2f / N pseudo near = %.2f )\n";
			lispTextSplits = "Split %d: n = %.2f ( NOpt = %.2f / NRepar = %.2f )\n";
			//lispText = "n = %.2f ( NOpt = %.2f / NRepar = %.2f / N pseudo near = %.2f )\n";
			lispText = "n = %.2f ( NOpt = %.2f / NRepar = %.2f )\n";
			first = LiSP::NOpt;
			second = LiSP::Reparametrized;
			last = LiSP::PseudoNear;
		}
		/*else
		{
			lispTextSplits = "Split %d: n = %.2f ( NOpt = %.2f / NRepar = %.2f / N arbitrary = %.2f )\n";
			lispText = "n = %.2f ( NOpt = %.2f / NRepar = %.2f / N arbitrary = %.2f )\n";
			first = LiSP::NOpt;
			second = LiSP::Reparametrized;
			last = LiSP::Arbitrary;
		}*/

		if (nbrOfSplits > 1)
			//sprintf_s(text, size, lispTextSplits, 1, dist[mode][0].x, dist[first][0].x, dist[second][0].x, dist[last][0].x);
			sprintf_s(text, size, lispTextSplits, 1, dist[mode][0].x, dist[first][0].x, dist[second][0].x);
		else
			//sprintf_s(text, size, lispText, dist[mode][0].x, dist[first][0].x, dist[second][0].x, dist[last][0].x);
			sprintf_s(text, size, lispText, dist[mode][0].x, dist[first][0].x, dist[second][0].x);

		out << text;

		for (unsigned int i = 1; i < nbrOfSplits; ++i)
		{
			sprintf_s(text, size, lispTextSplits, i+1, dist[mode][i].x, dist[first][i].x, dist[second][i].x, dist[last][i].x);
			out << text;
		}
	}

	return out.str();
}

void MainPass::KeyEvent(EventHandler&, unsigned int key, State state)
{
	switch (key)
	{
		case KeyBindings::ToggleLightProj:
			if (state == Down)
			{
				_keyBindings.directionalLight = !_keyBindings.directionalLight;
				UpdateLightProj();
			}
			break;
		//case KeyBindings::UseHierarchy:
		//	if (state == Down)
		//		_keyBindings.useHierarchy = !_keyBindings.useHierarchy;
		//	break;
		case KeyBindings::AABBDrawing:
			if (state == Down)
			{
				_keyBindings.showAABBs = !_keyBindings.showAABBs;
				_renderSystem->SetDrawBoundingBoxes(_keyBindings.showAABBs);
			}
			break;
	}

}

void MainPass::MouseEvent(EventHandler&, int x, int y, Button button, State state)
{
	bool enlarged = _enlargedMiniMap || _enlargedDepthMap || _enlargedErrGraph;

	bool insideMM = D3dUtils::IsInside(x, y, _miniMap.x, _miniMap.y, _miniMap.width, _miniMap.height);
	_shadeMiniMap = insideMM && !enlarged && _keyBindings.drawMiniMap;

	bool insideDM = D3dUtils::IsInside(x, y, _depthMap.x, _depthMap.y, _depthMap.width, _depthMap.height);
	_shadeDepthMap = insideDM && !enlarged && _keyBindings.drawShadowMap;

	bool insideEG = D3dUtils::IsInside(x, y, _errGraph.x, _errGraph.y, _errGraph.width, _errGraph.height);
	_shadeErrGraph = insideEG && !enlarged && _keyBindings.drawErrGraph;

	switch(button)
	{
		case Right:
			if (state == Down)
			{
				_lightRefPos = _light.GetPosition();
				
				if (_lightRefPos.x == .0f)
					_lightRefPos.x = .1f;
				else if (_lightRefPos.y == .0f)
					_lightRefPos.y = .1f;
				else if (_lightRefPos.z == .0f)
					_lightRefPos.z = .1f;				
					
				_lightRef = tVec2i(x, y);
			}

			_rotateLight = (state == Down);
			break;
		case Left:
			if (state == DblClk)
			{
				bool enlargedMM = _enlargedMiniMap;
				bool enlargedDM = _enlargedDepthMap;
				bool enlargedEG = _enlargedErrGraph;

				if (insideMM && !enlargedDM && !enlargedEG && _keyBindings.drawMiniMap)
					_enlargedMiniMap = !_enlargedMiniMap;

				if (insideDM && !enlargedMM && !enlargedEG && _keyBindings.drawShadowMap)
					_enlargedDepthMap = !_enlargedDepthMap;

				if (insideEG && !enlargedDM && !enlargedMM && _keyBindings.drawErrGraph)
					_enlargedErrGraph = !_enlargedErrGraph;
			}
			break;
	}

	if (_rotateLight)
		UpdateLightView(0.5f * (x - _lightRef.x), 0.2f * (y - _lightRef.y), _lightRefPos);

	_cursor = tVec2i(x, y);
}

void MainPass::UpdateLightView(float xAngle, float yAngle, const tVec4f& refPos)
{
	_light.SetPosition(vmath::rotation_matrix(xAngle, tVec3f(.0f, 1.0f, .0f)) * refPos);

	const tVec3f rotAxis(vmath::normalize(vmath::cross(tVec3f(.0f, 1.0f, .0f), tVec3f(_light.GetPosition()))));

	_light.SetPosition(vmath::rotation_matrix(yAngle, rotAxis) * _light.GetPosition());

	_effectlight.Position = D3DXVECTOR3(_light.GetPosition().x, _light.GetPosition().y, _light.GetPosition().z);

	_shadowMapping.SetLightView(_light.GetView());
	
	UpdateLightProj();
}

Light& MainPass::GetLight()
{
	return _light;
}

void MainPass::RegisterEvents()
{
	EventHandler::Instance().Register(EventHandler::KeyEvent, *this);
	EventHandler::Instance().Register(EventHandler::MouseEvent, *this);
}

void MainPass::UnregisterEvents()
{
	EventHandler::Instance().UnRegister(EventHandler::KeyEvent, *this);
	EventHandler::Instance().UnRegister(EventHandler::MouseEvent, *this);
}
