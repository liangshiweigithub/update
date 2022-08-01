#include "stdafx.h"
#include <limits>
#include "ViewerApp.h"

bool CALLBACK IsD3DDeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	return true;
}

HRESULT CALLBACK OnD3DCreateDevice(ID3D11Device* d3dDevice, const DXGI_SURFACE_DESC* bufferSurfaceDesc, void*)
{
	return ViewerApp::GetInstance().OnCreateDevice(d3dDevice, bufferSurfaceDesc);
}

HRESULT CALLBACK OnD3DResizedSwapChain(ID3D11Device* d3dDevice, IDXGISwapChain* swapChain, const DXGI_SURFACE_DESC* bufferSurfaceDesc, void*)
{
	return ViewerApp::GetInstance().OnResize(d3dDevice, swapChain, bufferSurfaceDesc);
}

void CALLBACK OnD3DFrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext)
{
	ViewerApp::GetInstance().Display(fElapsedTime);
}

LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* noFurtherProcessing, void*)
{
	//switch (msg)
	//{
	//	case WM_DESTROY:
	//		PostQuitMessage(0);
	//		break;		
	//	default:
	//		return DefWindowProc(hWnd, msg, wParam, lParam);
	//}

	return ViewerApp::GetInstance().OnMsg(hWnd, msg, wParam, lParam, noFurtherProcessing);
}

void CALLBACK OnKeyboard(UINT key, bool keyDown, bool altDown, void*)
{
	//ViewerApp::GetInstance().OnKeyEvent(key, keyDown, altDown);
}

void CALLBACK OnMouse(bool leftDown, bool rightDown, bool middleDown, bool, bool,
		int wheelDelta, int x, int y, void*)
{
	//ViewerApp::GetInstance().OnMouseEvent(leftDown, rightDown, middleDown, wheelDelta, x, y);
}

void CALLBACK OnFrameMove(double, float elapsedTime, void*)
{
	ViewerApp::GetInstance().Update(elapsedTime);
}

void CALLBACK OnD3DReleasingSwapChain(void*)
{
	ViewerApp::GetInstance().OnReleasingSwapChain();
}

void CALLBACK OnD3DDestroyDevice(void*)
{
	ViewerApp::GetInstance().OnDestroyDevice();
}

void CALLBACK OnD3DGUIEvent(UINT eventId, int controlId, CDXUTControl* control, void*)
{
	ViewerApp::GetInstance().OnGuiEvent(eventId, controlId, control);
}

/* ViewerApp::DrawQuadShader ***********************************************************/
const std::string ViewerApp::DrawQuadShader::DrawQuadSource("../shaders/PostProcess.fx");

ViewerApp::DrawQuadShader::DrawQuadShader(const std::string& name)
{
	SetName(name);
}

bool ViewerApp::DrawQuadShader::Initialize(ID3DX11Effect& effect)
{
	if (!effect.GetTechniqueByName("DisplayQuad")->IsValid())
		return false;
	if (!effect.GetTechniqueByName("DisplayTexturedQuad")->IsValid())
		return false;

	color = effect.GetVariableByName("g_color")->AsVector();
	if (!color->IsValid())
		return false;
	diffuseTex = effect.GetVariableByName("texDiffuse")->AsShaderResource();
	if (!diffuseTex->IsValid()) 
		return false;

	return true;
}

/* ViewerApp::KeyBindings **************************************************************/
const unsigned int ViewerApp::KeyBindings::Forward = 'W';
const unsigned int ViewerApp::KeyBindings::Backward = 'S';
const unsigned int ViewerApp::KeyBindings::StrafeLeft = 'A';
const unsigned int ViewerApp::KeyBindings::StrafeRight = 'D';
const unsigned int ViewerApp::KeyBindings::Upward = 'T';
const unsigned int ViewerApp::KeyBindings::Downward = 'G';
const unsigned int ViewerApp::KeyBindings::RollLeft = 'Q';
const unsigned int ViewerApp::KeyBindings::RollRight = 'E';
const unsigned int ViewerApp::KeyBindings::ResetUp = 'R';
const unsigned int ViewerApp::KeyBindings::IncSpeed = VK_OEM_PLUS;
const unsigned int ViewerApp::KeyBindings::DecSpeed = VK_OEM_MINUS;
const unsigned int ViewerApp::KeyBindings::Quit = VK_ESCAPE;
const unsigned int ViewerApp::KeyBindings::Help = VK_F1;
const unsigned int ViewerApp::KeyBindings::ChangeDev = VK_F2;
const unsigned int ViewerApp::KeyBindings::Stats = VK_F3;
const unsigned int ViewerApp::KeyBindings::SwitchCam = VK_F5;
const unsigned int ViewerApp::KeyBindings::SwitchControls = 'C';
const unsigned int ViewerApp::KeyBindings::SwitchLiSPMode = 'L';
const unsigned int ViewerApp::KeyBindings::SwitchFilter = 'F';
const unsigned int ViewerApp::KeyBindings::SaveCamPos = VK_F11;
const unsigned int ViewerApp::KeyBindings::LoadCamPos = VK_F12;
const unsigned int ViewerApp::KeyBindings::SaveLightPos = VK_F7;
const unsigned int ViewerApp::KeyBindings::LoadLightPos = VK_F8;
const unsigned int ViewerApp::KeyBindings::HideGUI = 'H';
const unsigned int ViewerApp::KeyBindings::DisplayShadowMap = 'M';
const unsigned int ViewerApp::KeyBindings::Shading = VK_F6;
const unsigned int ViewerApp::KeyBindings::LightMvUpward = VK_UP;
const unsigned int ViewerApp::KeyBindings::LightMvDownward = VK_DOWN;
const unsigned int ViewerApp::KeyBindings::LightMvLeft = VK_LEFT;
const unsigned int ViewerApp::KeyBindings::LightMvRight = VK_RIGHT;


ViewerApp::KeyBindings::KeyBindings() :
	forward(false),
	backward(false),
	left(false),
	right(false),
	up(false),
	down(false),
	stats(false),
	help(false),
	advGui(false),
	camActive(true),
	camCtrls(true),
	showGui(true)
{
}

/* ViewerApp ***************************************************************************/
Logger ViewerApp::logger("ViewerApp");

static int newevent = 0;

const unsigned int ViewerApp::SliderRes = 100;
const float ViewerApp::Factor = 0.18f;
const std::wstring ViewerApp::Title(L"Robust hard shadows");
const std::string ViewerApp::ConfigFile("config.ini");
const std::string ViewerApp::SceneFile("scenes.ini");
const std::string ViewerApp::ModelPath("../data/models/");
const std::string ViewerApp::SkyDomeFile("../data/models/SkyDome/SkyDome.dae");
const std::string ViewerApp::CamPosFile("c-positions.txt");
const std::string ViewerApp::LightPosFile("l-positions.txt");
const char* ViewerApp::LoadCamString = "First person: %f %f %f; %f %f %f; %f %f %f;\nThird person: %f %f %f; %f %f %f; %f %f %f;\n";
const char* ViewerApp::LoadLightString = "LightPos: %f %f %f;\n";
const std::string ViewerApp::HelpStrings[] =
{
	std::string("================================== C O M M O N ===================================="),
	std::string(" Camera movement ... Fwd/Bkwd: W/S, Strafe L/R: A/D, Roll L/R: Q/E, Upwd/Dwnwd: T/G"),
	std::string(" Camera free look ... Mousemove + Left button"),
	std::string(" Reset camera up ... R"),
	std::string(" Light movement ... Mousemove + Right button or UP/DOWN LEFT/RIGHT"),
	std::string(" Movement Speed ... Mousewheel or +/-"),
	std::string(" Toggle Lightprojection ... P"),
	std::string(" Show/Hide GUIs ... H"),
	std::string(" Show/Hide Help ... F1"),
	std::string(" Change Device ... F2"),
	std::string(" Show/Hide Statistics ... F3"),
	std::string(" Toggle Rendering / Visualization Mode ... F5"),
	std::string(" Shading On/Off ... F6"),
	std::string(" Save light position ... F7"),
	std::string(" Load light position ... F8"),
	std::string(" Save camera positions ... F11"),
	std::string(" Load camera positions ... F12"),
	//std::string(" "),
	std::string("============================ V I S U A L I Z A T I O N ============================"),
	std::string(" Switch Cameracontrols ... C")
};

ViewerApp::ViewerApp() :
	_mainPass(0),
	_graphicsEngine(0),
	_freeLook(false),
	_initialized(false),
	_leftBtnClick(false),
	_speed(31.0f),
	_sceneId(0),
	_sizes(0),
	_drawQuadShader("ViewerApp::DrawQuadShader"),
	_nbrOfSplits(1),
	_filterMode(D3dShadowMapping::None),
	_shadowMapSz(1024),
	_useLisp(false),
	_lispMode(LiSP::NOpt),
	_visTexels(false),
	_visSplits(false),
	_redSwim(false),
	_visDepthMap(false),
	_bestResSel(false),
	_alignLS(false),
	_spectate(false),
	_visBodyExact(false),
	_visBodyAABB(false),
	_visCamFrust(false),
	_visMiniMap(false),
	_useGS(false),
	_lambda(.5f),
	_lightRadius(.2f),
	_time(-1.0f),
	_firstCycle(false),
	_useFocusing(true),
	_texelSize(1),
	_visBESolid(true),
	_visBASolid(true),
	_visCFSolid(true),
	_visSampling(false),
	_renderShadows(true),
	_paramN(D3dShadowMapping::MaxNbrOfSplits, 1.0f),
	_visError(0),
	_visPerspErr(6),
	_zNear(.0f),
	_useMinDepth(false),
	_visErrorGraph(false),
	_pseudoNear(.0f),
	_pseudoFar(.0f),
	_nArbScaled(false),
	_useHWPCF(true),
	_useBlockerSearch(true),
	_visPCFRegions(false),
	_useMipMap(false),
	_visMipMap(false),
	_maxsamples(64),
	_gaussFilter(true)
{
	RegisterEvents();

	unsigned int size = 128;
	for (unsigned int i = 0; i < 4; ++i)
		_sizes.push_back(size *= 2);
}

ViewerApp::~ViewerApp()
{
	// static instance !
	// UnregisterEvents();
	
	DeleteMembers();
}

void ViewerApp::DeleteMembers()
{
	if (_graphicsEngine)
		_graphicsEngine->GetRenderSystem()->GetEffectManager()->Delete(_drawQuadShader);

	SAFE_DELETE(_graphicsEngine);
}

ViewerApp& ViewerApp::GetInstance()
{
	static ViewerApp instance;

	return instance;
}

bool ViewerApp::Init()
{
	if (!DXUTApp::Init())
		return false;

	HRESULT hr = S_OK;
	
	if ((hr = InitGUIs()) != S_OK)
	{
		PrintErrorMsg(logger, GetDXUTErrorMsg(hr), GetDXUTErrorMsg(hr), L"Init GUI Error");
		return false;
	}

	return true;
}

HRESULT ViewerApp::InitGUIs()
{
	HRESULT hr = S_OK;

	// init MainGui
	_mainHud.Init(&_dialogResourceManager);
	_mainHud.SetCallback(OnD3DGUIEvent);

	int btnX = 155, btnY = 10, btnDistY = 26, btnWidth = 170, btnHeight = 23, cmbX = -90, cmbY = 10;
	
	_mainHud.AddButton(GUIShowHelp, L"Help (F1)", btnX, btnY, btnWidth, btnHeight, VK_F1);
	_mainHud.AddButton(GUIChangeDev, L"Change Device (F2)", btnX, btnY += btnDistY, btnWidth, btnHeight, VK_F2);
	_mainHud.AddButton(GUIShowStats, L"Show/Hide Stats (F3)", btnX, btnY += btnDistY, btnWidth, btnHeight, VK_F3);
	_mainHud.AddButton(GUIToggleAdvGui, L"Toggle Ext Gui (F4)", btnX, btnY += btnDistY, btnWidth, btnHeight, VK_F4);
	_mainHud.AddComboBox(GUIChooseScene, cmbX, cmbY, 240, 25);

	// init AdvancedGui
	_extHud.Init(&_dialogResourceManager);
	_extHud.SetCallback(OnD3DGUIEvent);

	int dist = 25, advBtnX = 195, splitCmbY = 0, gsCmbY = splitCmbY + dist, lispCmbY = gsCmbY + dist,
		filterCmbY = lispCmbY + dist, szCmbY = filterCmbY + dist, cmbHeight = dist, focusY = szCmbY + 28,
		redSwimY = focusY + dist, alignLSVSY = redSwimY + dist, useminZY = alignLSVSY + dist,
		bestSelY = useminZY + dist;

	_extHud.AddComboBox(ExtGUISplits, advBtnX, splitCmbY, btnWidth, cmbHeight);
	_extHud.AddComboBox(ExtGUIUseGS, advBtnX, gsCmbY, btnWidth, cmbHeight);
	_extHud.AddComboBox(ExtGUILiSP, advBtnX, lispCmbY, btnWidth, cmbHeight, KeyBindings::SwitchLiSPMode);
	_extHud.AddComboBox(ExtGUIFilter, advBtnX, filterCmbY, btnWidth, cmbHeight, KeyBindings::SwitchFilter);
	_extHud.AddComboBox(ExtGUISize, advBtnX, szCmbY, btnWidth, cmbHeight);
	_extHud.AddCheckBox(ExtGUIFocusing, L"Focus Shadowmap", advBtnX, focusY, btnWidth, btnHeight, _useFocusing);
	_extHud.AddCheckBox(ExtGUIRedSwim, L"Reduce Swimming", advBtnX, redSwimY, btnWidth, btnHeight, _redSwim);
	_extHud.AddCheckBox(ExtGUIAlignLS, L"Lightspace-Alignment", advBtnX, alignLSVSY, btnWidth, btnHeight, _alignLS);
	_extHud.AddCheckBox(ExtGUIUseMinZ, L"Use min Depth", advBtnX, useminZY, btnWidth, btnHeight, _useMinDepth); 
	_extHud.AddCheckBox(ExtGUIBestResSel, L"Best Res Selection", advBtnX, bestSelY, btnWidth, btnHeight, _bestResSel);

	// extended gui visualizations
	int visChkBxX = advBtnX, visTexelsY = bestSelY + dist, visTexelSzY = visTexelsY + dist, visMapY = visTexelSzY + dist,
		visSmplY = visMapY + dist, visChsSmplY = visSmplY + dist, visErrGrY = visChsSmplY + dist,
		rndrShdwsY = visErrGrY + dist, visSplitsY = rndrShdwsY + dist;

	_extHud.AddCheckBox(ExtGUIVisTexels, L"Show Texels", visChkBxX, visTexelsY, btnWidth, btnHeight, _visTexels);
	_extHud.AddSlider(ExtGUIVisTexelSize, visChkBxX, visTexelSzY, btnWidth, btnHeight, 1 * SliderRes, 100 * SliderRes, static_cast<int>(_texelSize * SliderRes));
	_extHud.AddCheckBox(ExtGUIVisDepthMap, L"Show Lightviews", visChkBxX, visMapY, btnWidth, btnHeight, _visDepthMap);
	_extHud.AddCheckBox(ExtGUIVisSampling, L"Show Samplingfrequ.", visChkBxX, visSmplY, btnWidth, btnHeight, _visSampling);
	_extHud.AddComboBox(ExtGUIChooseSmp, visChkBxX, visChsSmplY, btnWidth, btnHeight);
	_extHud.AddCheckBox(ExtGUIVisError, L"Visualize Error", visChkBxX, visErrGrY, btnWidth, btnHeight, _visErrorGraph);
	_extHud.AddCheckBox(ExtGUIRndrShadows, L"Render Shadows", visChkBxX, rndrShdwsY, btnWidth, btnHeight, _renderShadows);
	_extHud.AddCheckBox(ExtGUIVisSplits, L"Show Splits", visChkBxX, visSplitsY, btnWidth, btnHeight, _visSplits);

	int zNY = visSplitsY + 25, zNSlY = zNY + 20;
	_extHud.AddStatic(ExtGUIZNearSt, L"ZNear:", advBtnX, zNY, 35, btnHeight);
	_extHud.AddSlider(ExtGUIZNear, advBtnX, zNSlY, btnWidth, btnHeight, 0, SliderRes, static_cast<int>((_zNear) * SliderRes));
	
	int stPNY = zNSlY + 25, pnY = stPNY + 20, stPFY = pnY + 25, pfY = stPFY + 20;

	// pseudoNear
	_extHud.AddStatic(ExtGUIPseudoNearSt, L"Pseudo near plane", advBtnX, stPNY, 93, btnHeight);
	_extHud.AddSlider(ExtGUIPseudoNear, advBtnX, pnY, btnWidth, btnHeight, 0, SliderRes, static_cast<int>(_pseudoNear * SliderRes));

	// pseudoFar
	_extHud.AddStatic(ExtGUIPseudoFarSt, L"Pseudo far plane", advBtnX, stPFY, 85, btnHeight);
	_extHud.AddSlider(ExtGUIPseudoFar, advBtnX, pfY, btnWidth, btnHeight, 0, SliderRes, static_cast<int>((1 - _pseudoFar) * SliderRes));

	int minStatX = advBtnX - 2, maxStatX = minStatX + 115, spStatY = pfY + 28, spSliderY = spStatY + 20,
		minStatY = spSliderY + 17;

	WCHAR sz[50];

	swprintf_s(sz, 50, L"Split Parameter: %.2f", _lambda);	
	_extHud.AddStatic(ExtGUILambdaStatic, sz, minStatX, spStatY, 100, btnHeight);
	_extHud.AddSlider(ExtGUILambda, advBtnX, spSliderY, btnWidth, btnHeight, 0, SliderRes, static_cast<int>(_lambda * SliderRes));
	_extHud.AddStatic(ExtGUILambdaLin, L"Uni", minStatX, minStatY, 20, 20);
	_extHud.AddStatic(ExtGUILambdaLog, L"Log", maxStatX, minStatY, 20, 20);

	int blStatX = -20, blurStY = splitCmbY + 60, blurSlY = blurStY + dist, hwPCFY = blurSlY + dist, blockerY = hwPCFY + dist;
	int useMipMap = hwPCFY + dist, visMipMap = useMipMap + dist, maxSamplesStY = visMipMap + dist, maxSamplesSlY = maxSamplesStY + dist, maxSamplesSt2Y = maxSamplesSlY + dist;
	int evsmStY = visMipMap + dist + 10, evsmSlY = evsmStY + dist, evsmSl2Y = evsmSlY + dist, evsmSt2Y = evsmSl2Y + dist, evsmLogBlurY = evsmSl2Y + dist;
	int visPCFFallbackY = evsmLogBlurY + dist;

	// filtering
	WCHAR kernel[50];
	int kernelsz = 2 * floor(_lightRadius * 16.0f + 0.5f) + 1;
	swprintf_s(kernel, 50, L"Filter kernel size: %ix%i", kernelsz, kernelsz);
	_extHud.AddStatic(ExtGUIBlurStatic, kernel, blStatX, blurStY, 80, btnHeight);
	_extHud.AddSlider(ExtGUIBlur, blStatX, blurSlY, btnWidth, btnHeight, 0, SliderRes, static_cast<int>(_lightRadius * SliderRes));
	_extHud.AddCheckBox(ExtGUIHWPCF, L"Use HW PCF Sampling", blStatX, hwPCFY, btnWidth, btnHeight, _useHWPCF);
	_extHud.AddCheckBox(ExtGUIMipMap, L"Use MipMapping", blStatX, useMipMap, btnWidth, btnHeight, _useMipMap);
	_extHud.AddCheckBox(ExtGUIVisMipMap, L"Vis. MipMap LOD", blStatX, visMipMap, btnWidth, btnHeight, _visMipMap);
	_extHud.AddCheckBox(ExtGUIBlockerSearch, L"Use Blocker search", blStatX, blockerY, btnWidth, btnHeight, _useBlockerSearch);
	_extHud.AddRadioButton(ExtGUIGaussFilter, ExtGUIFilterGroup, L"Gauss", blStatX, hwPCFY, btnWidth/2, btnHeight, _gaussFilter); 
	_extHud.AddRadioButton(ExtGUIBoxFilter, ExtGUIFilterGroup, L"Box", blStatX+btnWidth/2, hwPCFY, btnWidth/2, btnHeight, !_gaussFilter);

	//var pcf
	WCHAR maxsamp[50];
	swprintf_s(maxsamp, 50, L"Max. Samples: %i", _maxsamples);
	_extHud.AddStatic(ExtGUIMaxSamplesSt, maxsamp, blStatX, maxSamplesStY, 80, btnHeight);
	_extHud.AddSlider(ExtGUIMaxSamples, blStatX, maxSamplesSlY, btnWidth, btnHeight, 0, 512, _maxsamples);
	_extHud.AddStatic(ExtGUIMaxSamplesStMin, L"0", blStatX, maxSamplesSt2Y, 15, 20);
	_extHud.AddStatic(ExtGUIMaxSamplesStMax, L"512", blStatX + btnWidth - 28, maxSamplesSt2Y, 30, 20);

	// savsm precision
	_extHud.AddCheckBox(ExtGUIDistributePrecision, L"Distribute Prec.", blStatX, hwPCFY, btnWidth, btnHeight, _distributePrecision);

	// esm const
	WCHAR esm[50];
	swprintf_s(esm, 50, L"ESM Constant: %i", _evsmconst[0]);
	_extHud.AddStatic(ExtGUIESMConstSt, esm, blStatX, evsmStY, 50, 20);
	_extHud.AddSlider(ExtGUIESMConst, blStatX, evsmSlY, btnWidth, btnHeight, 0, 150, _evsmconst[0]);
	_extHud.AddStatic(ExtGUIESMConstStMin, L"0", blStatX, evsmSl2Y, 15, 20);
	_extHud.AddStatic(ExtGUIESMConstStMax, L"150", blStatX + btnWidth - 28, evsmSl2Y, 30, 20);
	_extHud.AddCheckBox(ExtGUIESMLogBlur, L"Log. Blur", blStatX, evsmLogBlurY, btnWidth, btnHeight, _useLogBlur);
	_extHud.AddCheckBox(ExtGUIESMPCFRegion, L"Vis. PCF Regions", blStatX, visPCFFallbackY, btnWidth, btnHeight, _visPCFRegions);

	// evsm consts
	WCHAR evsm[50];
	swprintf_s(evsm, 50, L"EVSM Constant (p/n): %i/%i", _evsmconst[0], _evsmconst[1]);
	_extHud.AddStatic(ExtGUIEVSMConstSt, evsm, blStatX, evsmStY, 50, 20);
	_extHud.AddSlider(ExtGUIEVSMConstPos, blStatX, evsmSlY, btnWidth, btnHeight, 0, 45, _evsmconst[0]);
	_extHud.AddSlider(ExtGUIEVSMConstNeg, blStatX, evsmSl2Y, btnWidth, btnHeight, 0, 45, _evsmconst[1]);
	_extHud.AddStatic(ExtGUIEVSMConstStMin, L"0", blStatX, evsmSt2Y, 15, 20);
	_extHud.AddStatic(ExtGUIEVSMConstStMax, L"45", blStatX + btnWidth - 24, evsmSt2Y, 30, 20);

	// vsm light bleeding reduction amount
	_extHud.AddStatic(ExtGUILBRAmountSt, L"Light Bleeding Reduction", blStatX, evsmStY, 80, 20);
	_extHud.AddSlider(ExtGUILBRAmount, blStatX, evsmSlY, btnWidth, btnHeight, 0, SliderRes, static_cast<int>(_lbramount * SliderRes));
	_extHud.AddStatic(ExtGUILBRAmountStMin, L"0.0", blStatX, evsmSl2Y, 22, 20);
	_extHud.AddStatic(ExtGUILBRAmountStMax, L"1.0", blStatX + btnWidth - 22, evsmSl2Y, 22, 20);

	int wStatX = blStatX, nStatY = visSmplY, chkNY = nStatY + 22, minNStatY = chkNY + 22, nSliderY = minNStatY + 20;		

	swprintf_s(sz, 50, L"Warping Parameter");
	_extHud.AddStatic(ExtGUIParamNCurrent, sz, wStatX, nStatY, 95, btnHeight);
	_extHud.AddCheckBox(ExtGUIParamNScaled, L"Based on first Split", wStatX, chkNY, btnWidth, btnHeight, _nArbScaled); 
	_extHud.AddStatic(ExtGUIParamNPSM1, L"PSM", wStatX, minNStatY, 22, 20);
	_extHud.AddStatic(ExtGUIParamNUniform2, L"Uni", wStatX+57, minNStatY, 20, 20);
	_extHud.AddStatic(ExtGUIParamNPSM2, L"PSM", wStatX + 83, minNStatY, 22, 20);
	_extHud.AddStatic(ExtGUIParamNUniform1, L"Uni", wStatX+140, minNStatY, 20, 20);

	_warpSliderPosX = wStatX;
	_warpSliderPosY = nSliderY;
	_btnHeight = btnHeight;
	_btnWidth = btnWidth;

	for (unsigned int i = 0; i < _paramN.size(); ++i)
		_extHud.AddSlider(ExtGUIParamN0 + i, wStatX, nSliderY + i * 25, btnWidth, btnHeight, 0 * SliderRes, 60 * SliderRes, static_cast<int>(_paramN[i] * SliderRes));

	// init VisualizationGui
	_visHud.Init(&_dialogResourceManager);
	_visHud.SetCallback(OnD3DGUIEvent);

	int visRB1X = -100, visRB2X = visRB1X + 65, visCBX = visRB2X + 100, visExactY = 0,
		visAABBY = visExactY + 28, visFrustY = visAABBY + 28, visMiniY = visFrustY + 28,
		radioSWdth = 50, radioWWdth = 80;

	_visHud.AddCheckBox(VisGUIBodyExact, L"Show exact Body", visCBX, visExactY, btnWidth, btnHeight, _visBodyExact);
	_visHud.AddRadioButton(VisGUIBESolid, VisGUIBEGroup, L"Solid", visRB1X, visExactY, radioSWdth, btnHeight, _visBESolid); 
	_visHud.AddRadioButton(VisGUIBEWire, VisGUIBEGroup, L"Wireframe", visRB2X, visExactY, radioWWdth, btnHeight, !_visBESolid); 

	_visHud.AddCheckBox(VisGUIBodyAABB, L"Show Body AABB", visCBX, visAABBY, btnWidth, btnHeight, _visBodyAABB);
	_visHud.AddRadioButton(VisGUIBASolid, VisGUIBAGroup, L"Solid", visRB1X, visAABBY, radioSWdth, btnHeight, _visBASolid); 
	_visHud.AddRadioButton(VisGUIBAWire, VisGUIBAGroup, L"Wireframe", visRB2X, visAABBY, radioWWdth, btnHeight, !_visBASolid); 

	_visHud.AddCheckBox(VisGUICamFrust, L"Show View Frustum", visCBX, visFrustY, btnWidth, btnHeight, _visCamFrust);
	_visHud.AddRadioButton(VisGUICFSolid, VisGUICFGroup, L"Solid", visRB1X, visFrustY, radioSWdth, btnHeight, _visCFSolid); 
	_visHud.AddRadioButton(VisGUICFWire, VisGUICFGroup, L"Wireframe", visRB2X, visFrustY, radioWWdth, btnHeight, !_visCFSolid); 

	_visHud.AddCheckBox(VisGUIMiniMap, L"Show MiniMap", visCBX, visMiniY, btnWidth, btnHeight, _visMiniMap);

	// init ErrorGui
	_errHud.Init(&_dialogResourceManager);
	_errHud.SetCallback(OnD3DGUIEvent);

	int errY = 0, errZX = 0, errXX = 0 /*errZX + btnWidth*/, errYX = errXX + btnWidth, errCombX = errYX + btnWidth;

	//_errHud.AddCheckBox(ErrGUIZDir, L"zDir", errZX, errY, btnWidth, btnHeight, (_visPerspErr & 0x1) == 1);
	_errHud.AddCheckBox(ErrGUIXDir, L"xDir", errXX, errY, btnWidth, btnHeight, (_visPerspErr & 0x2) == 2);
	_errHud.AddCheckBox(ErrGUIZDir, L"zDir", errYX, errY, btnWidth, btnHeight, (_visPerspErr & 0x4) == 4);
	//_errHud.AddCheckBox(ErrGUIXYComb, L"xDir x yDir", errCombX, errY, btnWidth, btnHeight, (_visPerspErr & 0x8) == 8);

	return hr;
}

bool ViewerApp::InitializeGUIs()
{
	LiSP::Mode lispModes[] = {LiSP::NOpt, LiSP::Reparametrized, LiSP::Arbitrary, LiSP::PseudoNear};
	
	CDXUTComboBox* combobox = _extHud.GetComboBox(ExtGUILiSP);
	if (combobox->AddItem(L"Uniform (L)", 0) != S_OK) return false;
	if (combobox->AddItem(L"NOpt LiSP (L)", 0) != S_OK) return false;
	if (combobox->AddItem(L"NRepar LiSP (L)", 0) != S_OK) return false;
	if (combobox->AddItem(L"N arbitrary (L)", 0) != S_OK) return false;
	//if (combobox->AddItem(L"N pseudo near (L)", 0) != S_OK) return false;
	if (_useLisp)
		combobox->SetSelectedByIndex(0);
	else
	{
		for (unsigned int i = 0; i < 4; ++i)
			if (_lispMode == lispModes[i])
				combobox->SetSelectedByIndex(i);
	}

	combobox = _extHud.GetComboBox(ExtGUIFilter);
	if (combobox->AddItem(L"No Filtering (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"2x2 HW PCF (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"4x4 Box PCF (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"25 Poisson PCF (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"6x6 Box PCF (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"32 Poisson PCF (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"8x8 Box PCF (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"64 Poisson PCF (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"64 Poisson PCSS (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"ESM (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"VSM (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"EVSM (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"Ref. Box PCF (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"SAVSM Float (F)", 0) != S_OK) return false;
	if (combobox->AddItem(L"SAVSM Int (F)", 0) != S_OK) return false;
	combobox->SetSelectedByIndex(static_cast<unsigned int>(_filterMode));

	combobox = _extHud.GetComboBox(ExtGUIUseGS);
	if (combobox->AddItem(L"n+1 passes", 0) != S_OK) return false;
	if (combobox->AddItem(L"1+1 passes", 0) != S_OK) return false;
	combobox->SetSelectedByIndex(_useGS ? 1 : 0);

	combobox = _extHud.GetComboBox(ExtGUISize);
	for (unsigned int i = 0; i < _sizes.size(); ++i)
	{
		std::wstringstream ss;
		ss << _sizes[i] << L" x " << _sizes[i];

		if (combobox->AddItem(ss.str().c_str(), &_sizes[i]) != S_OK) return false;
		if (_sizes[i] == _shadowMapSz) combobox->SetSelectedByIndex(i);
	}

	combobox = _extHud.GetComboBox(ExtGUISplits);
	for (unsigned int i = 1; i < D3dShadowMapping::MaxNbrOfSplits + 1; ++i)
	{
		std::wstringstream ss;
		ss << i << (i > 1 ? L" Splits" : L" Split");

		if (combobox->AddItem(ss.str().c_str(), 0) != S_OK) return false;
		if (i == _nbrOfSplits) combobox->SetSelectedByIndex(i - 1);
	}

	combobox = _extHud.GetComboBox(ExtGUIChooseSmp);
	if (combobox->AddItem(L"xDir x yDir", 0) != S_OK) return false;
	if (combobox->AddItem(L"xDir", 0) != S_OK) return false;
	if (combobox->AddItem(L"yDir", 0) != S_OK) return false;
	combobox->SetSelectedByIndex(_visError);

	return true;
}

bool ViewerApp::Initialize()
{
	DeleteMembers();

	std::wstring caption(L"Initializaton Error");

	ID3D11Device* d3dDevice = DXUTGetD3D11Device();
	ID3D11DeviceContext* d3dContext = DXUTGetD3D11DeviceContext();

	const char c_szName[] = "MY_DEVICE";
	d3dDevice->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( c_szName ) - 1, c_szName );

	if (!d3dDevice)
	{
		PrintErrorMsg(logger, L"Cannot find a valid D3D11Device.", L"Cannot find a valid D3D11Device.", caption);
		return false;
	}

	_graphicsEngine = new GraphicsEngine();

	if (!_graphicsEngine->Initialize())
	{
		PrintErrorMsg(logger, L"Cannot initalize GraphicsEngine.", L"Cannot initalize GraphicsEngine.", caption);
		return false;
	}

	if (!_graphicsEngine->GetRenderSystem()->Initialize(*d3dDevice, *d3dContext, *_swapChain, _gammaCorrect))
	{
		PrintErrorMsg(logger, L"Cannot initalize D3dRenderSystem.", L"Cannot initalize D3dRenderSystem.", caption);
		return false;
	}

	_drawQuadShader.SetFilename(DrawQuadShader::DrawQuadSource, Shader::CompleteSource);
	if (!_graphicsEngine->GetRenderSystem()->GetEffectManager()->Generate(_drawQuadShader, *_graphicsEngine->GetRenderSystem()))
	{
		PrintErrorMsg(logger, L"Cannot create DrawQuadShader.", L"Cannot create DrawQuadShader.", caption);
		return false;
	}

	DrawInitStatus(0);

	_mainPass = new MainPass(_graphicsEngine->GetScene(),*_txtHelper);

	RestoreState();

	if (!InitializeGUIs())
	{
		PrintErrorMsg(logger, L"Cannot initialize advanced GUIs.", L"Cannot initialize advanced GUIs.", caption);
		return false;
	}

	if (!_mainPass->Initialize(*_graphicsEngine->GetRenderSystem(),
		Perspective(
			static_cast<float>(_config_fov),										// field of view
			_graphicsEngine->GetScene().GetCamera().GetPerspective().GetAspect(),	// aspect ration
			static_cast<float>(_config_zNear),										// near plane distance
			static_cast<float>(_config_zFar)),										// far plane distance
		View(
			tVec3f(.0f, 20.0f,    .0f),	// eye
			tVec3f(.0f, 20.0f, -30.0f),	// center
			tVec3f(.0f,  1.0f,    .0f))	// up
		))
	{
		PrintErrorMsg(logger, L"Cannot initalize MainPass.", L"Cannot initalize MainPass.", caption);
		return false;
	}

	DrawInitStatus(5);

	if (!LoadScene())
	{
		PrintErrorMsg(logger, L"Cannot load Scene.", L"Cannot load Scene.", caption);
		return false;
	}

	DrawInitStatus(100);

	_mainPass->SetSceneId(_sceneId);
	_mainHud.GetComboBox(GUIChooseScene)->SetSelectedByIndex(_sceneId);

	RenderSystem::Viewport vp = _graphicsEngine->GetRenderSystem()->GetViewport();
	float aspect = vp.width / static_cast<float>(vp.height);
	const Perspective& cPersp = _graphicsEngine->GetScene().GetCamera().GetPerspective();
	_graphicsEngine->GetScene().GetCamera().SetPerspective(
		Perspective(cPersp.GetFOV(), aspect, cPersp.GetZNear(), cPersp.GetZFar()));

	const Perspective& sPersp = _mainPass->GetCamera().GetPerspective();
	_mainPass->GetCamera().SetPerspective(
		Perspective(sPersp.GetFOV(), aspect, sPersp.GetZNear(), sPersp.GetZFar()));

	UpdateGuiVisibility();

	_initialized = true;

	return true;
}

bool ViewerApp::LoadScene()
{
	std::string scenes;
	if(!FileIO::LoadFromFile(SceneFile, scenes))
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << "Error loading \"" << SceneFile << "\".";
		return false;
	}

	unsigned int lines = FileIO::GetNbrOfLines(scenes);
	if (!lines)
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << SceneFile << " is empty.";
		return false;
	}

	Model* skydome = 0;
	if((skydome = _graphicsEngine->LoadModel(SkyDomeFile)) == 0)
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << "Cannot find \"" << SkyDomeFile << "\".";
		return false;
	}

	skydome->GetModelNode()->GetTransform().Rotate(-90.0f, tVec3f(1.0f, .0f, .0f));
	_mainPass->GetSkyDomePass().AddNode(skydome->GetModelNode());

	float estScenes = .0f;
	for (unsigned int i = 1; i <= lines; ++i)
	{
		std::string line = FileIO::GetLine(scenes, i);

		if (line.length() > 3 && line[0] != '#')
			estScenes++;				
	}

	float start = 5.0f;
	float step = 95.0f / estScenes;
	unsigned int sceneIndex = 0;
	for (unsigned int i = 1; i <= lines; ++i)
	{
		std::string line = FileIO::GetLine(scenes, i);

		if (line.length() > 3 && line[0] != '#')
		{
			const std::string file = ModelPath + line;
			Model* scene = 0;
			if (!_initialized)
				scene = _graphicsEngine->LoadModel(file, start + step * (sceneIndex), start + step * (sceneIndex + 1));
			else
				scene = _graphicsEngine->LoadModel(file);

			if (scene)
			{
				_mainHud.GetComboBox(GUIChooseScene)->AddItem(Util::StrToWStr(line).c_str(), static_cast<void*>(&sceneIndex));
				sceneIndex++;

				// rotate scene to coordinate system with y = up				
				scene->GetModelNode()->GetTransform().Rotate(-90.0f, tVec3f(1.0f, .0f, .0f));

				// get scene AABB
				BoundingBox sceneAABB;
				sceneAABB.Expand(*scene->GetModelNode());

				// translate scene to origin
				tVec3f trans = -(sceneAABB.GetMinimum() + sceneAABB.GetMaximum());
				scene->GetModelNode()->GetTransform().Translate(
					tVec3f(0.5f * trans.x, -sceneAABB.GetMinimum().y - 0.1f, 0.5f * trans.z));

				_mainPass->AddNode(scene->GetModelNode());
				_mainPass->GetShadowMappingPass().AddNode(scene->GetModelNode());
			}
			else
			{
				if (logger.IsEnabled(Logger::Error))
					Logger::Stream(logger, Logger::Error) << "Cannot find " << line.c_str();
			}
		}		
	}

	_graphicsEngine->AddPass(_mainPass);

	if (!_mainPass->GetNodeCount())
	{
		if (logger.IsEnabled(Logger::Warn))
			Logger::Stream(logger, Logger::Warn) << "There were no scenes loaded!";
		return false;
	}

	return true;
}

void ViewerApp::Update(float elapsedTime)
{
	
	//const bool sceneCam = _keyBindings.camActive || (!_keyBindings.camActive && !_keyBindings.camCtrls);
	//const float speed = _speed * static_cast<float>(_timing.GetCycleTime());
	////const float speed = _speed * elapsedTime;

	//if (sceneCam)
	//	UpdateSpectator(_graphicsEngine->GetScene().GetCamera(), speed);
	//else
	//	UpdateSpectator(_mainPass->GetCamera(), speed);
}

void ViewerApp::Display(float elapsedTime)
{
	_timing.TriggerCycle();

	if (_settingsDlg.IsActive())
	{
		RenderSystem::CullState cull = _graphicsEngine->GetRenderSystem()->GetCullState();
		_graphicsEngine->GetRenderSystem()->SetCullState(RenderSystem::CullFrontFace);
		_settingsDlg.OnRender(elapsedTime);
		_graphicsEngine->GetRenderSystem()->SetCullState(cull);
		return;
	}

	// update camera
	const bool sceneCam = _keyBindings.camActive || (!_keyBindings.camActive && !_keyBindings.camCtrls);
	const float speed = _speed * static_cast<float>(_timing.GetCycleTime());

	if (sceneCam)
		UpdateSpectator(_mainPass->GetShadowMappingPass().GetCamera(), speed, true);
	else
		UpdateSpectator(_mainPass->GetCamera(), speed, false);
	
	UpdateLight(speed);

	_graphicsEngine->GetRenderSystem()->SetGeometryCounter(true);

	// render scene
	//double startRenderTime = PerformanceCounter::Time();
	_graphicsEngine->RenderScene();
	//_renderTime = static_cast<float>(PerformanceCounter::Time() - startRenderTime);

	// render gui
	RenderSystem::CullState cull = _graphicsEngine->GetRenderSystem()->GetCullState();
	_graphicsEngine->GetRenderSystem()->SetCullState(RenderSystem::CullFrontFace);

	_visHud.OnRender(elapsedTime);

	_mainHud.OnRender(elapsedTime);

	UpdateErrHudLoc();
	UpdateGuiVisibility();
	UpdateGuiElements();

	if (_mainPass->GetKeyBindings().drawErrGraph)
		_errHud.OnRender(elapsedTime);

	_extHud.OnRender(elapsedTime);

	if(_keyBindings.stats)
		RenderStats();

	if (_keyBindings.help)
		RenderHelp();
	
	if (_time >= .0f)
	{
		RenderInfo();
		if (!_firstCycle)
			_time -= elapsedTime;
	}
	else
	{
		_time = -1.0f;
	}

	_graphicsEngine->GetRenderSystem()->SetCullState(cull);

	_firstCycle = false;
}

void ViewerApp::RenderStats()
{
	// Output statistics
	_txtHelper->Begin();

	_txtHelper->SetInsertionPos(8, 8);
	_txtHelper->SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	_txtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
	_txtHelper->DrawTextLine( DXUTGetDeviceStats() );
	_txtHelper->DrawFormattedTextLine(L"Triangles: %d", _graphicsEngine->GetRenderSystem()->GetTriangleCounter());
	_txtHelper->DrawTextLine(Util::StrToWStr(_mainPass->GetStats()).c_str());

	_txtHelper->End();
}

void ViewerApp::RenderHelp()
{
	RenderSystem::Viewport vp = _graphicsEngine->GetRenderSystem()->GetViewport();

	unsigned int width = 650;
	unsigned int height = 445;
	unsigned int x = vp.width/2-width/2;
	unsigned int y = vp.height/2-height/2;

	_graphicsEngine->GetRenderSystem()->GetEffectManager()->Use(
		_mainPass->GetShadowMappingPass().Get2dEffect(), 0 , 0);
	RenderSystem::Viewport viewport(x, y, width, height);
	_graphicsEngine->GetRenderSystem()->Draw(viewport, tVec4f(.0f, .0f, .0f, 0.75f));
	
	_txtHelper->Begin();

	_txtHelper->SetInsertionPos(x + 15, y + 15);
	_txtHelper->SetForegroundColor( D3DXCOLOR(1.0f, 1.0f, .0f, 1.0f));

	unsigned int lines = sizeof(HelpStrings) / sizeof(HelpStrings[0]);

	for (unsigned int i = 0; i < lines; ++i)
	{
		_txtHelper->DrawTextLine(Util::StrToWStr(HelpStrings[i]).c_str());
	}

	_txtHelper->End();
}

void ViewerApp::RenderInfo()
{
	float alpha = 1.0f;

	if (_time < .6f)
		alpha *= _time;

	int offset = _info.length() / 2 * 15;

	RenderSystem::Viewport vp(_graphicsEngine->GetRenderSystem()->GetViewport());

	_txtHelper->Begin();

	_txtHelper->SetInsertionPos(vp.width/2 - offset, vp.height - 350);
	_txtHelper->SetForegroundColor( D3DXCOLOR(1.0f, 1.0f, .0f, alpha));
	_txtHelper->DrawTextLine(Util::StrToWStr(_info).c_str());

	_txtHelper->End();		
}

HRESULT ViewerApp::OnResize(ID3D11Device* d3dDevice, IDXGISwapChain* swapchain, const DXGI_SURFACE_DESC* bufferSurfaceDesc)
{
	HRESULT hr = S_OK;

	V_RETURN(DXUTApp::OnResize(d3dDevice, swapchain, bufferSurfaceDesc));

    // Setup the camera's projection parameters
    float aspect = static_cast<float>(bufferSurfaceDesc->Width) / static_cast<float>(bufferSurfaceDesc->Height);

	if (_graphicsEngine)
	{
		const Perspective& cPersp = _graphicsEngine->GetScene().GetCamera().GetPerspective();
		_graphicsEngine->GetScene().GetCamera().SetPerspective(
			Perspective(cPersp.GetFOV(), aspect, cPersp.GetZNear(), cPersp.GetZFar()));

		_graphicsEngine->GetRenderSystem()->SetViewport(
			RenderSystem::Viewport(0, 0, bufferSurfaceDesc->Width, bufferSurfaceDesc->Height));
	}

	_mainHud.SetLocation(bufferSurfaceDesc->Width - 340, 0);
	_mainHud.SetSize(340, 340);

	_extHud.SetLocation(bufferSurfaceDesc->Width - 380, 115);
	_extHud.SetSize(380, 400);

	_visHud.SetLocation(bufferSurfaceDesc->Width - 475, 50);
	_visHud.SetSize(310, 120);

	_errHud.SetLocation(105, bufferSurfaceDesc->Height - 40);
	_errHud.SetSize(500, 50);

	if (_mainPass)
	{
		const Perspective& sPersp = _mainPass->GetCamera().GetPerspective();
		_mainPass->GetCamera().SetPerspective(
			Perspective(sPersp.GetFOV(), aspect, sPersp.GetZNear(), sPersp.GetZFar()));

		_mainPass->GetShadowMappingPass().GenerateMinMaxMipmap();
	}

	return hr;
}

LRESULT ViewerApp::OnMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* noFurtherProcessing)
{
	*noFurtherProcessing = _dialogResourceManager.MsgProc( hWnd, msg, wParam, lParam );
    if ( *noFurtherProcessing )
        return 0;

	if (_settingsDlg.IsActive())
	{
		_settingsDlg.MsgProc(hWnd, msg, wParam, lParam);
		return 0;
	}

    // Give the dialogs a chance to handle the message first
	*noFurtherProcessing = _mainHud.MsgProc(hWnd, msg, wParam, lParam);
    if ( *noFurtherProcessing )
        return 0;

	*noFurtherProcessing = _errHud.MsgProc(hWnd, msg, wParam, lParam);
    if ( *noFurtherProcessing )
        return 0;

	*noFurtherProcessing = _extHud.MsgProc(hWnd, msg, wParam, lParam);
	if ( *noFurtherProcessing )
		return 0;

	*noFurtherProcessing = _visHud.MsgProc(hWnd, msg, wParam, lParam);
	if ( *noFurtherProcessing )
		return 0;

	bool dispatch = true;
	std::string out;

	switch (msg)
	{		
		case WM_KEYDOWN:
			// on key repeat send only one event
			dispatch &= !(HIWORD(lParam) & 0x4000);
			out.assign("KeyDown Event");
			break;
		case WM_KEYUP:
			// do nothing
			out.assign("KeyUp Event");
			break;
		case WM_LBUTTONDOWN:
			{
				POINT current;
				GetCursorPos(&current);
				_spectator.SetReferencePoint(tVec2i(current.x, current.y));
				_freeLook = true;
			}
			break;
		case WM_LBUTTONUP:
			_freeLook = false;
			break;
		default:
			dispatch &= !(HIWORD(lParam) & 0x4000);
			out.assign("Default Event");
			break;	
	}

	if (dispatch)
	{
		double time = PerformanceCounter::Time();
		//std::cout << newevent++ << ": " << out.c_str() << " (" << wParam << ") Time: " << time << std::endl;;
		EventHandler::Instance().DispatchMsg(msg, wParam, lParam);
	}
	return 0;
}

void ViewerApp::OnGuiEvent(unsigned int, int controlId, CDXUTControl* control)
{
	const unsigned int size = 256;
	const float time = 4.0f;
	char text[size];

	switch (controlId)
    {
		case GUIShowHelp:
			_keyBindings.help = !_keyBindings.help;
			break;
		case GUIShowStats:
			_keyBindings.stats = !_keyBindings.stats;
			_mainPass->GetKeyBindings().visStats = _keyBindings.stats;
			break;
		case GUIChangeDev:
			_settingsDlg.SetActive(!_settingsDlg.IsActive());
			if (_settingsDlg.IsActive())
			{
				EditSettingDlg();			
				StoreState();
			}
			break;
		case GUIToggleAdvGui:
			_keyBindings.advGui = !_keyBindings.advGui;
			break;
		case GUIChooseScene:
			{
				CDXUTComboBox* comboBox = dynamic_cast<CDXUTComboBox*>(control);
				_mainPass->SetSceneId(static_cast<unsigned int>(comboBox->GetSelectedIndex()));
			}
			break;
		case ExtGUISplits:
			{
				unsigned int value = _extHud.GetComboBox(ExtGUISplits)->GetSelectedIndex() + 1;
				if (value != _mainPass->GetNbrOfSplits())
				{
					_mainPass->SetNbrOfSplits(value);
					_time = time;
					sprintf_s(text, size, "%d %s", _mainPass->GetNbrOfSplits(), _mainPass->GetNbrOfSplits() > 1 ? "Splits" : "Split");
					_info.assign(text);
				}
			}
			break;
		case ExtGUILiSP:
			{
				CDXUTComboBox* comboBox = dynamic_cast<CDXUTComboBox*>(control);
				int index = comboBox->GetSelectedIndex();
				if (index > 0)
				{
					_mainPass->GetShadowMappingPass().SetUseLiSP(true);
					_mainPass->GetShadowMappingPass().GetLiSP().SetMode(static_cast<LiSP::Mode>(index));
				}
				else
					_mainPass->GetShadowMappingPass().SetUseLiSP(false);
			}
			break;
		case ExtGUIFilter:
			{
				CDXUTComboBox* comboBox = dynamic_cast<CDXUTComboBox*>(control);
				_mainPass->SetFilterMode(static_cast<D3dShadowMapping::FilterMode>(comboBox->GetSelectedIndex()));
			}
			break;
		case ExtGUISize:
			{
				CDXUTComboBox* comboBox = dynamic_cast<CDXUTComboBox*>(control);
				_mainPass->GetShadowMappingPass().SetShadowMapSize(*static_cast<unsigned int*>(comboBox->GetSelectedData()));
			}
			break;
		case ExtGUIVisSplits:
			_mainPass->GetKeyBindings().showSplits = _extHud.GetCheckBox(ExtGUIVisSplits)->GetChecked();
			break;
		case ExtGUIVisTexels:
			_mainPass->GetKeyBindings().showTexels = _extHud.GetCheckBox(ExtGUIVisTexels)->GetChecked();
			break;
		case ExtGUIVisTexelSize:
			_mainPass->SetVisTexelSz(_extHud.GetSlider(ExtGUIVisTexelSize)->GetValue() / static_cast<int>(SliderRes));
			break;
		case ExtGUIRedSwim:
			_mainPass->GetShadowMappingPass().SetSwimmingRed(!_mainPass->GetShadowMappingPass().GetSwimmingRed());
			break;
		case ExtGUILambda:
			{
				float value = _extHud.GetSlider(ExtGUILambda)->GetValue() / static_cast<float>(SliderRes);
				_mainPass->GetShadowMappingPass().SetSplitParam(value);

				WCHAR sz[50];
				swprintf_s(sz, 50, L"Split Parameter: %.2f", value);
				_extHud.GetStatic(ExtGUILambdaStatic)->SetText(sz);
			}
			break;
		case ExtGUIAlignLS:
			_mainPass->GetShadowMappingPass().SetAlignmentLSVS(_extHud.GetCheckBox(ExtGUIAlignLS)->GetChecked());
			break;
		case ExtGUIBestResSel:
			_mainPass->SetBestResSplitSel(_extHud.GetCheckBox(ExtGUIBestResSel)->GetChecked());
			break;
		case ExtGUIVisDepthMap:
			_mainPass->GetKeyBindings().drawShadowMap = _extHud.GetCheckBox(ExtGUIVisDepthMap)->GetChecked();
			break;
		case ExtGUIUseGS:
			_mainPass->GetShadowMappingPass().SetUseGS(_extHud.GetComboBox(ExtGUIUseGS)->GetSelectedIndex() == 1);
			break;
		case ExtGUIBlur:
			{
				float value = _extHud.GetSlider(ExtGUIBlur)->GetValue() / static_cast<float>(SliderRes);
				_mainPass->GetShadowMappingPass().SetBlurRadius(value);
				_mainPass->GetShadowMappingPass().SetLightRadius(value);

				WCHAR kernel[50];
				int kernelsz = 2 * floor(value * 16.0f + 0.5f) + 1;
				swprintf_s(kernel, 50, L"Filter kernel size: %ix%i", kernelsz, kernelsz);
				_extHud.GetStatic(ExtGUIBlurStatic)->SetText(kernel);
			}
			break;
		case ExtGUIHWPCF:
			_mainPass->GetKeyBindings().useHWPCF = _extHud.GetCheckBox(ExtGUIHWPCF)->GetChecked();
			break;
		case ExtGUIMipMap:
			{
				bool val = _extHud.GetCheckBox(ExtGUIMipMap)->GetChecked();
				_mainPass->GetKeyBindings().useMipMap = val;
				_mainPass->GetShadowMappingPass().SetMipMapped(val);
				//mipmapping disables logblur
				if(val)
				{
					_extHud.GetCheckBox(ExtGUIESMLogBlur)->SetChecked(false);
					_mainPass->GetKeyBindings().useLogBlur = false;
					_mainPass->GetShadowMappingPass().SetUseLogBlur(false);
				}
			}
			break;
		case ExtGUIBlockerSearch:
			_mainPass->GetKeyBindings().useBlockerSearch = _extHud.GetCheckBox(ExtGUIBlockerSearch)->GetChecked();
			break;
		case ExtGUIRndrShadows:
			_mainPass->GetKeyBindings().showShadows = _extHud.GetCheckBox(ExtGUIRndrShadows)->GetChecked();
			break;
		case ExtGUIFocusing:
			_mainPass->GetShadowMappingPass().SetUseFocusing(_extHud.GetCheckBox(ExtGUIFocusing)->GetChecked());
			break;
		case ExtGUIDistributePrecision:
			_mainPass->GetKeyBindings().distributePrecision = _extHud.GetCheckBox(ExtGUIDistributePrecision)->GetChecked();
			_mainPass->GetShadowMappingPass().SetDistributePrecision(_mainPass->GetKeyBindings().distributePrecision);
			break;
		case ExtGUIESMConst:
			{
				int value = _extHud.GetSlider(ExtGUIESMConst)->GetValue();
				_mainPass->GetShadowMappingPass().SetEVSMConst(value, 0.0);

				WCHAR text[50];
				swprintf_s(text, 50, L"ESM Constant: %i", value);
				_extHud.GetStatic(ExtGUIESMConstSt)->SetText(text);
			}
			break;
		case ExtGUIEVSMConstPos:
		case ExtGUIEVSMConstNeg:
			{
				int pos = _extHud.GetSlider(ExtGUIEVSMConstPos)->GetValue();
				int neg = _extHud.GetSlider(ExtGUIEVSMConstNeg)->GetValue();
				_mainPass->GetShadowMappingPass().SetEVSMConst(pos, neg);

				WCHAR text[50];
				swprintf_s(text, 50, L"EVSM Constant (p/n): %i/%i", pos, neg);
				_extHud.GetStatic(ExtGUIEVSMConstSt)->SetText(text);
			}
			break;
		case ExtGUILBRAmount:
			_mainPass->GetKeyBindings().lbramount = _extHud.GetSlider(ExtGUILBRAmount)->GetValue() / static_cast<float>(SliderRes);
			break;
		case ExtGUIESMLogBlur:
			{
				bool value = _extHud.GetCheckBox(ExtGUIESMLogBlur)->GetChecked();
				_mainPass->GetKeyBindings().useLogBlur = value;
				_mainPass->GetShadowMappingPass().SetUseLogBlur(value);

				//logblur disables mipmapping
				if(value)
				{
					_extHud.GetCheckBox(ExtGUIMipMap)->SetChecked(false);
					_mainPass->GetKeyBindings().useMipMap = false;
					_mainPass->GetShadowMappingPass().SetMipMapped(false);
				}
			}
			break;
		case ExtGUIESMPCFRegion:
			_mainPass->GetKeyBindings().visPCFRegions = _extHud.GetCheckBox(ExtGUIESMPCFRegion)->GetChecked();
			break;
		case ExtGUIVisMipMap:
			_mainPass->GetKeyBindings().visMipMap = _extHud.GetCheckBox(ExtGUIVisMipMap)->GetChecked();
			break;
		case ExtGUIMaxSamples:
			{
				int value = _extHud.GetSlider(ExtGUIMaxSamples)->GetValue();
				_mainPass->GetKeyBindings().maxsamples = value;

				WCHAR text[50];
				swprintf_s(text, 50, L"Max. Samples: %i", value);
				_extHud.GetStatic(ExtGUIMaxSamplesSt)->SetText(text);
			}
			break;
		case ExtGUIParamN0:
		case ExtGUIParamN1:
		case ExtGUIParamN2:
		case ExtGUIParamN3:
		case ExtGUIParamN4:
		case ExtGUIParamN5:
		case ExtGUIParamN6:
		case ExtGUIParamN7:
		case ExtGUIParamN8:
		case ExtGUIParamN9:
		case ExtGUIParamN10:
		case ExtGUIParamN11:
		case ExtGUIParamN12:
		case ExtGUIParamN13:
		case ExtGUIParamN14:
		case ExtGUIParamN15:
			{
				CDXUTSlider* slider = dynamic_cast<CDXUTSlider*>(control);
				unsigned int split = static_cast<unsigned int>(slider->GetID() - ExtGUIParamN0);
				float value = exp(slider->GetValue() / static_cast<float>(SliderRes) * Factor);				
				
				if (value < std::numeric_limits<float>::infinity())
					_mainPass->GetShadowMappingPass().GetLiSP().SetParamN(value, split);
				else
					_mainPass->GetShadowMappingPass().GetLiSP().SetParamN(std::numeric_limits<float>::infinity(), split);
			}
			break;
		case ExtGUIParamNScaled:
			_mainPass->GetShadowMappingPass().GetLiSP().SetArbScaled(_extHud.GetCheckBox(ExtGUIParamNScaled)->GetChecked());
			for (unsigned int i = 1; i <= 15; ++i)
				_extHud.GetSlider(ExtGUIParamN0 + i)->SetEnabled(!_mainPass->GetShadowMappingPass().GetLiSP().GetArbScaled());
			break;
		case ExtGUIZNear:
			{
				float value = _extHud.GetSlider(ExtGUIZNear)->GetValue() / static_cast<float>(SliderRes);
				_mainPass->GetShadowMappingPass().SetZNear(value);
			}
			break;
		case ExtGUIUseMinZ:
			_mainPass->GetShadowMappingPass().SetUseMinDepth(_extHud.GetCheckBox(ExtGUIUseMinZ)->GetChecked());
			_extHud.GetSlider(ExtGUIParamN0)->SetEnabled(!_extHud.GetCheckBox(ExtGUIUseMinZ)->GetChecked());
			break;
		case ExtGUIVisSampling:
			_mainPass->GetKeyBindings().showSampling = _extHud.GetCheckBox(ExtGUIVisSampling)->GetChecked();
			break;
		case ExtGUIVisError:
			_mainPass->GetShadowMappingPass().SetCalcError(_extHud.GetCheckBox(ExtGUIVisError)->GetChecked());
			break;
		case ExtGUIChooseSmp:
			_mainPass->GetKeyBindings().visError = _extHud.GetComboBox(ExtGUIChooseSmp)->GetSelectedIndex();
			break;
		case ExtGUIPseudoNear:
			{
				float value = _extHud.GetSlider(ExtGUIPseudoNear)->GetValue() / static_cast<float>(SliderRes);
				_mainPass->GetShadowMappingPass().SetPseudoNear(value);
			}
			break;
		case ExtGUIPseudoFar:
			{
				float value = _extHud.GetSlider(ExtGUIPseudoFar)->GetValue() / static_cast<float>(SliderRes);
				_mainPass->GetShadowMappingPass().SetPseudoFar(1 - value);
			}
			break;
		case ExtGUIGaussFilter:
		case ExtGUIBoxFilter:
			_mainPass->GetShadowMappingPass().SetUseGaussFilter(_extHud.GetRadioButton(ExtGUIGaussFilter)->GetChecked());
			break;
		//case ErrGUIZDir:
		//	if (_errHud.GetCheckBox(ErrGUIZDir)->GetChecked())
		//		_mainPass->GetKeyBindings().visPerspError = _mainPass->GetKeyBindings().visPerspError | 0x1;
		//	else
		//		_mainPass->GetKeyBindings().visPerspError = _mainPass->GetKeyBindings().visPerspError & 0xE;
		//	break;
		case ErrGUIXDir:
			if (_errHud.GetCheckBox(ErrGUIXDir)->GetChecked())
				_mainPass->GetKeyBindings().visPerspError = _mainPass->GetKeyBindings().visPerspError | 0x2;
			else
				_mainPass->GetKeyBindings().visPerspError = _mainPass->GetKeyBindings().visPerspError & 0xD;
			break;
		case ErrGUIZDir:
			if (_errHud.GetCheckBox(ErrGUIZDir)->GetChecked())
				_mainPass->GetKeyBindings().visPerspError = _mainPass->GetKeyBindings().visPerspError | 0x4;
			else
				_mainPass->GetKeyBindings().visPerspError = _mainPass->GetKeyBindings().visPerspError & 0xB;
			break;
		//case ErrGUIXYComb:
		//	if (_errHud.GetCheckBox(ErrGUIXYComb)->GetChecked())
		//		_mainPass->GetKeyBindings().visPerspError = _mainPass->GetKeyBindings().visPerspError | 0x8;
		//	else
		//		_mainPass->GetKeyBindings().visPerspError = _mainPass->GetKeyBindings().visPerspError & 0x7;
		//	break;
		case VisGUIBodyExact:
			_mainPass->GetKeyBindings().drawBodyExact = _visHud.GetCheckBox(VisGUIBodyExact)->GetChecked();
			break;
		case VisGUIBodyAABB:
			_mainPass->GetKeyBindings().drawBodyAABB =  _visHud.GetCheckBox(VisGUIBodyAABB)->GetChecked();
			break;
		case VisGUICamFrust:
			_mainPass->GetKeyBindings().drawCamFrustum = _visHud.GetCheckBox(VisGUICamFrust)->GetChecked();
			break;
		case VisGUIMiniMap:
			_mainPass->GetKeyBindings().drawMiniMap = _visHud.GetCheckBox(VisGUIMiniMap)->GetChecked();
			break;
		case VisGUIBASolid:
		case VisGUIBAWire:
			_mainPass->GetKeyBindings().aabbBodySolid = _visHud.GetRadioButton(VisGUIBASolid)->GetChecked();
			break;
		case VisGUIBESolid:
		case VisGUIBEWire:
			_mainPass->GetKeyBindings().exactBodySolid = _visHud.GetRadioButton(VisGUIBESolid)->GetChecked();
			break;
		case VisGUICFSolid:
		case VisGUICFWire:
			_mainPass->GetKeyBindings().camFrustSolid = _visHud.GetRadioButton(VisGUICFSolid)->GetChecked();
			break;		
		default:
			break;
    }

	_firstCycle = true;
	//_keyBindings.updateSliders = _mainPass->GetShadowMappingPass().GetLiSP().GetArbScaled();
}

void ViewerApp::UpdateErrHudLoc()
{
	// update gui pos for error graph
	const RenderSystem::Viewport* vp = _mainPass->GetMapVP(MainPass::ErrGraph);
	if (vp)
		_errHud.SetLocation(vp->x + 95, vp->y + vp->height - 30);
}

void ViewerApp::StoreState()
{
	_sceneId = _mainPass->GetSceneId();
	_nbrOfSplits = _mainPass->GetNbrOfSplits();
	_spectate = _mainPass->GetSpectatorActive();
	_bestResSel = _mainPass->GetBestResSplitSel();
	_texelSize = _mainPass->GetVisTexelSz();

	MainPass::KeyBindings& kb = _mainPass->GetKeyBindings();
	_visSplits = kb.showSplits;
	_visTexels = kb.showTexels;
	_visDepthMap = kb.drawShadowMap;
	kb.drawDepthValues;
	_visCamFrust = kb.drawCamFrustum;
	_visBodyExact = kb.drawBodyExact;
	_visBodyAABB = kb.drawBodyAABB;
	_visMiniMap = kb.drawMiniMap;
	_visBESolid = kb.exactBodySolid;
	_visBASolid = kb.aabbBodySolid;
	_visCFSolid = kb.camFrustSolid;
	_visSampling = kb.showSampling;
	_renderShadows = kb.showShadows;
	_visError = kb.visError;
	_visPerspErr = kb.visPerspError;
	_useHWPCF = kb.useHWPCF;
	_useBlockerSearch = kb.useBlockerSearch;
	_distributePrecision = kb.distributePrecision;
	_lbramount = kb.lbramount;
	_useLogBlur = kb.useLogBlur;
	_visPCFRegions = kb.visPCFRegions;
	_useMipMap = kb.useMipMap;
	_visMipMap = kb.visMipMap;
	_maxsamples = kb.maxsamples;

	_filterMode = _mainPass->GetShadowMappingPass().GetFilterMode();
	_shadowMapSz = _mainPass->GetShadowMappingPass().GetShadowMapSize();
	_useLisp = _mainPass->GetShadowMappingPass().GetUseLiSP();
	_lispMode = _mainPass->GetShadowMappingPass().GetLiSP().GetMode();
	_nArbScaled = _mainPass->GetShadowMappingPass().GetLiSP().GetArbScaled();
	
	const LiSP::tPArray& nArbs = _mainPass->GetShadowMappingPass().GetLiSP().GetParamN();
	for (unsigned int i = 0; i < _nbrOfSplits; ++i)
		_paramN[i] = nArbs[i];

	_alignLS = _mainPass->GetShadowMappingPass().GetAlignmentLSVS();
	_redSwim = _mainPass->GetShadowMappingPass().GetSwimmingRed();
	_useGS = _mainPass->GetShadowMappingPass().GetUseGS();
	_lambda = _mainPass->GetShadowMappingPass().GetSplitParam();
	_lightRadius = _mainPass->GetShadowMappingPass().GetLightRadius();
	_useFocusing = _mainPass->GetShadowMappingPass().GetUseFocusing();
	_zNear = _mainPass->GetShadowMappingPass().GetZNear();
	_useMinDepth = _mainPass->GetShadowMappingPass().GetUseMinDepth();
	_pseudoNear = _mainPass->GetShadowMappingPass().GetPseudoNear();
	_pseudoFar = _mainPass->GetShadowMappingPass().GetPseudoFar();
	_visErrorGraph = _mainPass->GetShadowMappingPass().GetCalcError();
}

void ViewerApp::RestoreState()
{
	_mainPass->SetSceneId(_sceneId);
	_mainPass->SetNbrOfSplits(_nbrOfSplits);
	_mainPass->GetShadowMappingPass().SetCamera(_graphicsEngine->GetScene().GetCamera());
	_mainPass->SetSpectatorActive(_spectate);
	_mainPass->SetBestResSplitSel(_bestResSel);
	_mainPass->SetVisTexelSz(_texelSize);

	MainPass::KeyBindings& kb = _mainPass->GetKeyBindings();	
	kb.showSplits = _visSplits;
	kb.showTexels = _visTexels;
	kb.drawShadowMap = _visDepthMap;
	kb.drawCamFrustum = _visCamFrust;
	kb.drawBodyExact = _visBodyExact;
	kb.drawBodyAABB = _visBodyAABB;
	kb.drawMiniMap = _visMiniMap;
	kb.exactBodySolid = _visBESolid;
	kb.aabbBodySolid = _visBASolid;
	kb.camFrustSolid = _visCFSolid;
	kb.showSampling = _visSampling;
	kb.showShadows = _renderShadows;
	kb.visError = _visError;
	kb.visPerspError = _visPerspErr;
	kb.useHWPCF = _useHWPCF;
	kb.useBlockerSearch = _useBlockerSearch;
	kb.distributePrecision = _distributePrecision;
	kb.lbramount = _lbramount;
	kb.useLogBlur = _useLogBlur;
	kb.visPCFRegions = _visPCFRegions;
	kb.useMipMap = _useMipMap;
	kb.visMipMap = _visMipMap;
	kb.maxsamples = _maxsamples;

	_mainPass->GetShadowMappingPass().SetDistributePrecision(_distributePrecision);
	_mainPass->GetShadowMappingPass().SetFilterMode(_filterMode);
	_mainPass->GetShadowMappingPass().SetShadowMapSize(_shadowMapSz);
	_mainPass->GetShadowMappingPass().SetUseLiSP(_useLisp);
	_mainPass->GetShadowMappingPass().GetLiSP().SetMode(_lispMode);
	_mainPass->GetShadowMappingPass().GetLiSP().SetArbScaled(_nArbScaled);
	
	for (unsigned int i = 0; i < _nbrOfSplits; ++i)
		_mainPass->GetShadowMappingPass().GetLiSP().SetParamN(_paramN[i], i);
	
	_mainPass->GetShadowMappingPass().SetAlignmentLSVS(_alignLS);
	_mainPass->GetShadowMappingPass().SetSwimmingRed(_redSwim);
	_mainPass->GetShadowMappingPass().SetUseGS(_useGS);
	_mainPass->GetShadowMappingPass().SetSplitParam(_lambda);
	_mainPass->GetShadowMappingPass().SetLightRadius(_lightRadius);
	_mainPass->GetShadowMappingPass().SetBlurRadius(_lightRadius);
	_mainPass->GetShadowMappingPass().SetUseFocusing(_useFocusing);
	_mainPass->GetShadowMappingPass().SetZNear(_zNear);
	_mainPass->GetShadowMappingPass().SetUseMinDepth(_useMinDepth);
	_mainPass->GetShadowMappingPass().SetPseudoNear(_pseudoNear);
	_mainPass->GetShadowMappingPass().SetPseudoFar(_pseudoFar);
	_mainPass->GetShadowMappingPass().SetCalcError(_visErrorGraph);
}

void ViewerApp::UpdateGuiVisibility()
{
	_mainHud.SetVisible(_keyBindings.showGui);
	_extHud.SetVisible(_keyBindings.showGui && _keyBindings.advGui);
	_visHud.SetVisible(_keyBindings.showGui && !_keyBindings.camActive);
	
	bool errHudVis = _extHud.GetCheckBox(ExtGUIVisError)->GetChecked() &&
		!_mainPass->IsEnlarged(MainPass::DepthMap) && 
		!_mainPass->IsEnlarged(MainPass::MiniMap);
	
	_errHud.SetVisible(errHudVis);

	if (!_keyBindings.showGui)
		return;

	_extHud.GetCheckBox(ExtGUIVisSampling)->SetVisible(_keyBindings.camActive);
	_extHud.GetComboBox(ExtGUIChooseSmp)->SetVisible(_extHud.GetCheckBox(ExtGUIVisSampling)->GetChecked() && _keyBindings.camActive);

	if (!_mainPass)
		return;

	bool pssm = _mainPass->GetNbrOfSplits() > 1;
	bool lisp = _mainPass->GetShadowMappingPass().GetUseLiSP();
	bool arbLisp = lisp && (_mainPass->GetShadowMappingPass().GetLiSP().GetMode() == LiSP::Arbitrary);
	bool updateLUp = _mainPass->GetShadowMappingPass().GetAlignmentLSVS();

	_extHud.GetCheckBox(ExtGUIVisError)->SetVisible(lisp || updateLUp);

	_extHud.GetCheckBox(ExtGUIFocusing)->SetVisible(!(lisp || pssm));
	_extHud.GetCheckBox(ExtGUIVisSplits)->SetVisible(pssm);
	_extHud.GetSlider(ExtGUILambda)->SetVisible(pssm);
	_extHud.GetStatic(ExtGUILambdaLin)->SetVisible(pssm);
	_extHud.GetStatic(ExtGUILambdaLog)->SetVisible(pssm);
	_extHud.GetStatic(ExtGUILambdaStatic)->SetVisible(pssm);
	_extHud.GetCheckBox(ExtGUIBestResSel)->SetVisible(pssm);
	_extHud.GetComboBox(ExtGUIUseGS)->SetVisible(pssm);

	_extHud.GetCheckBox(ExtGUIAlignLS)->SetVisible(!lisp);
	_extHud.GetCheckBox(ExtGUIRedSwim)->SetVisible(!(lisp || updateLUp));

	bool lispMode = (_mainPass->GetShadowMappingPass().GetLiSP().GetMode() == LiSP::NOpt) ||
					(_mainPass->GetShadowMappingPass().GetLiSP().GetMode() == LiSP::PseudoNear);
	bool pseudoNAct = pssm || (lisp && lispMode);
	bool pseudoFAct = pssm || (lisp && _mainPass->GetShadowMappingPass().GetLiSP().GetMode() == LiSP::NOpt);
	_extHud.GetStatic(ExtGUIPseudoNearSt)->SetVisible(pseudoNAct);
	_extHud.GetSlider(ExtGUIPseudoNear)->SetVisible(pseudoNAct);
	_extHud.GetStatic(ExtGUIPseudoFarSt)->SetVisible(pseudoFAct);
	_extHud.GetSlider(ExtGUIPseudoFar)->SetVisible(pseudoFAct);

	_extHud.GetStatic(ExtGUIParamNCurrent)->SetVisible(arbLisp);
	int nrsplits = _mainPass->GetNbrOfSplits();
	for (unsigned int split = 0; split < _paramN.size(); ++split)
	{
		_extHud.GetSlider(ExtGUIParamN0+split)->SetVisible(arbLisp && _mainPass->GetNbrOfSplits() > split);
		if(nrsplits > 8)
		{
			_extHud.GetSlider(ExtGUIParamN0+split)->SetLocation(_warpSliderPosX + ((int)split/8 * (_btnWidth / 2 + 3)),_warpSliderPosY + (split%8) * 25);
			_extHud.GetSlider(ExtGUIParamN0+split)->SetSize((_btnWidth - 3) / 2, _btnHeight);
		}
		else
		{
			_extHud.GetSlider(ExtGUIParamN0+split)->SetLocation(_warpSliderPosX,_warpSliderPosY + split * 25);
			_extHud.GetSlider(ExtGUIParamN0+split)->SetSize(_btnWidth, _btnHeight);
		}
	}

	_extHud.GetStatic(ExtGUIParamNPSM1)->SetVisible(arbLisp);
	_extHud.GetStatic(ExtGUIParamNUniform1)->SetVisible(arbLisp);
	_extHud.GetStatic(ExtGUIParamNPSM2)->SetVisible(arbLisp && _mainPass->GetNbrOfSplits() > 8);
	_extHud.GetStatic(ExtGUIParamNUniform2)->SetVisible(arbLisp && _mainPass->GetNbrOfSplits() > 8);
	_extHud.GetCheckBox(ExtGUIParamNScaled)->SetVisible(arbLisp && pssm);
	
	D3dShadowMapping::FilterMode filtermode = _mainPass->GetShadowMappingPass().GetFilterMode();
	bool filtering = filtermode != D3dShadowMapping::None;
	bool showCBs = filtering && filtermode > D3dShadowMapping::HWPCF && filtermode <= D3dShadowMapping::PCFPCSS || filtermode == D3dShadowMapping::VarBoxPCF;
	_extHud.GetStatic(ExtGUIBlurStatic)->SetVisible(filtering && filtermode >= D3dShadowMapping::PCFPCSS);
	_extHud.GetSlider(ExtGUIBlur)->SetVisible(filtering && filtermode >= D3dShadowMapping::PCFPCSS);
	_extHud.GetCheckBox(ExtGUIHWPCF)->SetVisible(showCBs);
	_extHud.GetCheckBox(ExtGUIBlockerSearch)->SetVisible(showCBs && filtermode != D3dShadowMapping::PCFPCSS && false);
	_extHud.GetCheckBox(ExtGUIMipMap)->SetVisible(filtermode > D3dShadowMapping::PCFPCSS);
	_extHud.GetCheckBox(ExtGUIVisMipMap)->SetVisible(filtermode > D3dShadowMapping::PCFPCSS);
	_extHud.GetSlider(ExtGUIVisTexelSize)->SetVisible(_mainPass->GetKeyBindings().showTexels);
	_extHud.GetRadioButton(ExtGUIGaussFilter)->SetVisible(filtermode == D3dShadowMapping::ESM || filtermode == D3dShadowMapping::VSM || filtermode == D3dShadowMapping::EVSM);
	_extHud.GetRadioButton(ExtGUIBoxFilter)->SetVisible(filtermode == D3dShadowMapping::ESM || filtermode == D3dShadowMapping::VSM || filtermode == D3dShadowMapping::EVSM);

	_extHud.GetCheckBox(ExtGUIDistributePrecision)->SetVisible(filtermode == D3dShadowMapping::SAVSM);

	_extHud.GetSlider(ExtGUIESMConst)->SetVisible(filtermode == D3dShadowMapping::ESM);
	_extHud.GetStatic(ExtGUIESMConstSt)->SetVisible(filtermode == D3dShadowMapping::ESM);
	_extHud.GetStatic(ExtGUIESMConstStMin)->SetVisible(filtermode == D3dShadowMapping::ESM);
	_extHud.GetStatic(ExtGUIESMConstStMax)->SetVisible(filtermode == D3dShadowMapping::ESM);
	_extHud.GetCheckBox(ExtGUIESMLogBlur)->SetVisible(filtermode == D3dShadowMapping::ESM);
	_extHud.GetCheckBox(ExtGUIESMPCFRegion)->SetVisible(filtermode == D3dShadowMapping::ESM);

	_extHud.GetSlider(ExtGUIEVSMConstPos)->SetVisible(filtermode == D3dShadowMapping::EVSM);
	_extHud.GetSlider(ExtGUIEVSMConstNeg)->SetVisible(filtermode == D3dShadowMapping::EVSM);
	_extHud.GetStatic(ExtGUIEVSMConstSt)->SetVisible(filtermode == D3dShadowMapping::EVSM);
	_extHud.GetStatic(ExtGUIEVSMConstStMin)->SetVisible(filtermode == D3dShadowMapping::EVSM);
	_extHud.GetStatic(ExtGUIEVSMConstStMax)->SetVisible(filtermode == D3dShadowMapping::EVSM);

	_extHud.GetStatic(ExtGUIMaxSamplesSt)->SetVisible(filtermode == D3dShadowMapping::VarBoxPCF);
	_extHud.GetStatic(ExtGUIMaxSamplesStMin)->SetVisible(filtermode == D3dShadowMapping::VarBoxPCF);
	_extHud.GetStatic(ExtGUIMaxSamplesStMax)->SetVisible(filtermode == D3dShadowMapping::VarBoxPCF);
	_extHud.GetSlider(ExtGUIMaxSamples)->SetVisible(filtermode == D3dShadowMapping::VarBoxPCF);

	bool showlbr = filtermode == D3dShadowMapping::VSM || filtermode == D3dShadowMapping::SAVSM || filtermode == D3dShadowMapping::UINTSAVSM;
	_extHud.GetSlider(ExtGUILBRAmount)->SetVisible(showlbr);
	_extHud.GetStatic(ExtGUILBRAmountSt)->SetVisible(showlbr);
	_extHud.GetStatic(ExtGUILBRAmountStMin)->SetVisible(showlbr);
	_extHud.GetStatic(ExtGUILBRAmountStMax)->SetVisible(showlbr);

	bool useZSlider = !_mainPass->GetShadowMappingPass().GetUseMinDepth();
	_extHud.GetStatic(ExtGUIZNearSt)->SetVisible(useZSlider);
	_extHud.GetSlider(ExtGUIZNear)->SetVisible(useZSlider);

	_visHud.GetRadioButton(VisGUIBESolid)->SetVisible(_mainPass->GetKeyBindings().drawBodyExact);
	_visHud.GetRadioButton(VisGUIBEWire)->SetVisible(_mainPass->GetKeyBindings().drawBodyExact);
	_visHud.GetRadioButton(VisGUIBASolid)->SetVisible(_mainPass->GetKeyBindings().drawBodyAABB);
	_visHud.GetRadioButton(VisGUIBAWire)->SetVisible(_mainPass->GetKeyBindings().drawBodyAABB);
	_visHud.GetRadioButton(VisGUICFSolid)->SetVisible(_mainPass->GetKeyBindings().drawCamFrustum);
	_visHud.GetRadioButton(VisGUICFWire)->SetVisible(_mainPass->GetKeyBindings().drawCamFrustum);
}

void ViewerApp::OnDestroyDevice()
{
	DeleteMembers();

	_mainHud.GetComboBox(GUIChooseScene)->RemoveAllItems();
	_extHud.GetComboBox(ExtGUISplits)->RemoveAllItems();
	_extHud.GetComboBox(ExtGUILiSP)->RemoveAllItems();
	_extHud.GetComboBox(ExtGUIFilter)->RemoveAllItems();
	_extHud.GetComboBox(ExtGUISize)->RemoveAllItems();
	_extHud.GetComboBox(ExtGUIUseGS)->RemoveAllItems();
	_extHud.GetComboBox(ExtGUIChooseSmp)->RemoveAllItems();

	DXUTApp::OnDestroyDevice();
}

void ViewerApp::UpdateSpectator(Camera& camera, const float speed, bool roll)
{
	//if(_keyBindings.forward)
	//	_spectator.Move(camera, speed);	
	//else if(_keyBindings.backward)
	//	_spectator.Move(camera, -speed);

	//if(_keyBindings.left)
	//	_spectator.Strafe(camera, speed);
	//else if(_keyBindings.right)
	//	_spectator.Strafe(camera, -speed);

	//if(_keyBindings.up)
	//	_spectator.Lift(camera, speed);
	//else if(_keyBindings.down)
	//	_spectator.Lift(camera, -speed);

	//if (!_freeLook)
	//	return;

	//const tVec2i oldRef(_spectator.GetReferencePoint());

	////if (_refPoint.x != oldRef.x)
	////	_spectator.Yaw(camera, -0.5f, _refPoint.x);
	////	
	////if (_refPoint.y != oldRef.y)
	////	_spectator.Pitch(camera, .5f, _refPoint.y);

	////if (_refPoint.x != oldRef.x || _refPoint.y != oldRef.y)
	////	_spectator.SetReferencePoint(_refPoint);

	//POINT current;
	//GetCursorPos(&current);

	//if (current.x != oldRef.x)
	//	_spectator.Yaw(camera, 0.5f, current.x);

	//if (current.y != oldRef.y)
	//	_spectator.Pitch(camera, 0.5f, current.y);

	//if (current.x != oldRef.x || current.y != oldRef.y)
	//	_spectator.SetReferencePoint(tVec2i(current.x, current.y));

	//std::cout << "Update Spectator " << PerformanceCounter::Time() << std::endl;

	HWND hWnd = DXUTGetHWND();

	if(EventHandler::Instance().GetStateDown(VK_LBUTTON, hWnd) && _freeLook)
	{
		POINT current;
		GetCursorPos(&current);		

		_spectator.Yaw(camera, .5f, current.x);		
		_spectator.Pitch(camera, .5f, current.y);
		_spectator.SetReferencePoint(tVec2i(current.x, current.y));
	}

	if (EventHandler::Instance().GetStateDown(KeyBindings::Forward, hWnd))
		_spectator.Move(camera, speed);	
	else if(EventHandler::Instance().GetStateDown(KeyBindings::Backward, hWnd))
		_spectator.Move(camera, -speed);

	if (EventHandler::Instance().GetStateDown(KeyBindings::StrafeLeft, hWnd))
		_spectator.Strafe(camera, speed);
	else if(EventHandler::Instance().GetStateDown(KeyBindings::StrafeRight, hWnd))
		_spectator.Strafe(camera, -speed);

	if (EventHandler::Instance().GetStateDown(KeyBindings::Upward, hWnd))
		_spectator.Lift(camera, speed);
	else if(EventHandler::Instance().GetStateDown(KeyBindings::Downward, hWnd))
		_spectator.Lift(camera, -speed);

	if (roll)
	{
		if (EventHandler::Instance().GetStateDown(KeyBindings::RollLeft, hWnd))
			_spectator.Roll(camera, speed);
		else if(EventHandler::Instance().GetStateDown(KeyBindings::RollRight, hWnd))
			_spectator.Roll(camera, -speed);
	}
	else
		camera.GetView().SetUp(tVec3f(.0f, 1.0f, .0f));

}

void ViewerApp::UpdateLight(float speed)
{
	HWND hWnd = DXUTGetHWND();

	tVec4f lightPos(_mainPass->GetLight().GetPosition());

	if (EventHandler::Instance().GetStateDown(KeyBindings::LightMvUpward, hWnd))
		_mainPass->UpdateLightView(.0f, -speed, lightPos);
	else if(EventHandler::Instance().GetStateDown(KeyBindings::LightMvDownward, hWnd))
		_mainPass->UpdateLightView(.0f, speed, lightPos);

	if (EventHandler::Instance().GetStateDown(KeyBindings::LightMvLeft, hWnd))
		_mainPass->UpdateLightView(speed, .0f, lightPos);
	else if(EventHandler::Instance().GetStateDown(KeyBindings::LightMvRight, hWnd))
		_mainPass->UpdateLightView(-speed, .0f, lightPos);

}

void ViewerApp::UpdateGuiElements()
{
	if (!_mainPass)
		return;

	if (_mainPass->GetShadowMappingPass().GetUseMinDepth())
	{
		float depth = _mainPass->GetShadowMappingPass().GetDepthRange().x;
		_mainPass->GetShadowMappingPass().GetLiSP().SetParamN(depth, 0);

		float val = log(depth) * static_cast<float>(SliderRes) / Factor;
		_extHud.GetSlider(ExtGUIParamN0)->SetValue(static_cast<int>(val));
	}

	if (_mainPass->GetShadowMappingPass().GetLiSP().GetArbScaled() && _mainPass->GetNbrOfSplits() > 1)
	{
		for (unsigned int i = 1; i < _mainPass->GetNbrOfSplits(); ++i)
		{
			float val = log(_mainPass->GetShadowMappingPass().GetLiSP().GetParamN()[i]) * static_cast<float>(SliderRes) / Factor;
			_extHud.GetSlider(ExtGUIParamN0 + i)->SetValue(static_cast<int>(val));
		}
	}

	if (_mainPass->GetShadowMappingPass().GetFilterMode() == D3dShadowMapping::ESM)
	{
		int value = _extHud.GetSlider(ExtGUIESMConst)->GetValue();
		_mainPass->GetShadowMappingPass().SetEVSMConst(value, 0.0);
	}

	if (_mainPass->GetShadowMappingPass().GetFilterMode() == D3dShadowMapping::EVSM)
	{
		int pos = _extHud.GetSlider(ExtGUIEVSMConstPos)->GetValue();
		int neg = _extHud.GetSlider(ExtGUIEVSMConstNeg)->GetValue();
		_mainPass->GetShadowMappingPass().SetEVSMConst(pos, neg);
	}
}

void ViewerApp::KeyEvent(EventHandler& handler, unsigned int key, State state)
{
	const unsigned int size = 256;
	const float time = 4.0f;
	char text[size];

	double timestamp = PerformanceCounter::Time();
	//std::cout << newevent++ << ": Update Keyevent (" << key << ") Time: " << timestamp << std::endl;

	switch (key)
	{
		//case KeyBindings::Forward:
		//	_keyBindings.forward = (state == Down);
		//	break;
		//case KeyBindings::Backward:
		//	_keyBindings.backward = (state == Down);
		//	break;
		//case KeyBindings::StrafeLeft:
		//	_keyBindings.left = (state == Down);
		//	break;
		//case KeyBindings::StrafeRight:
		//	_keyBindings.right = (state == Down);
		//	break;
		//case KeyBindings::Upward:
		//	_keyBindings.up = (state == Down);
		//	break;
		//case KeyBindings::Downward:
		//	_keyBindings.down = (state == Down);
		//	break;
		case KeyBindings::ResetUp:
			if (state == Down)
				_mainPass->GetShadowMappingPass().GetCamera().GetView().SetUp(tVec3f(.0f, 1.0f, .0f));
			break;
		case KeyBindings::SaveLightPos:
			if (state == Down)
			{
				const Light& light = _mainPass->GetLight();
				char line[1024];
				sprintf_s(line, 1024, ViewerApp::LoadLightString,
					light.GetPosition().x, light.GetPosition().y, light.GetPosition().z);

				FileIO::Write(LightPosFile, line);
				_time = time;
				_info.assign("Light position saved");
			}
			break;
		case KeyBindings::LoadLightPos:
			if (state == Down)
			{
				std::string out;
				if (!FileIO::Load(LightPosFile, out)) break;

				tVec3f pos;
				sscanf_s(out.c_str(), ViewerApp::LoadLightString, &pos.x, &pos.y, &pos.z);
				
				_mainPass->UpdateLightView(.0f, .0f, tVec4f(pos, 1.0f));

				_time = time;
				_info.assign("Light position loaded");

			}
			break;
		case KeyBindings::SaveCamPos:
			if (state == Down)
			{
				const View& first = _mainPass->GetShadowMappingPass().GetCamera().GetView();
				const View& third = _mainPass->GetCamera().GetView();
				char line[1024];
				sprintf_s(line, 1024, ViewerApp::LoadCamString, 
					first.GetEye().x, first.GetEye().y, first.GetEye().z, 
					first.GetCenter().x, first.GetCenter().y, first.GetCenter().z, 
					first.GetUp().x, first.GetUp().y, first.GetUp().z,
					third.GetEye().x, third.GetEye().y, third.GetEye().z, 
					third.GetCenter().x, third.GetCenter().y, third.GetCenter().z, 
					third.GetUp().x, third.GetUp().y, third.GetUp().z);
				
				FileIO::Write(CamPosFile, line);

				_time = time;
				_info.assign("Camera positions saved");
			}
			break;
		case KeyBindings::LoadCamPos:
			if (state == Down)
			{
				std::string out;
				if (!FileIO::Load(CamPosFile, out)) break;

				View& first = _mainPass->GetShadowMappingPass().GetCamera().GetView();
				View& third = _mainPass->GetCamera().GetView();
				tVec3f eye1, center1, up1, eye3, center3, up3;

				sscanf_s(out.c_str(), ViewerApp::LoadCamString, 
					&eye1.x, &eye1.y, &eye1.z, 
					&center1.x, &center1.y, &center1.z, 
					&up1.x, &up1.y, &up1.z,
					&eye3.x, &eye3.y, &eye3.z, 
					&center3.x, &center3.y, &center3.z, 
					&up3.x, &up3.y, &up3.z);
				
				first.SetEye(eye1);
				first.SetCenter(center1);
				first.SetUp(up1);
				third.SetEye(eye3);
				third.SetCenter(center3);
				third.SetUp(up3);

				_time = time;
				_info.assign("Camera positions loaded");
			}
			break;
		case KeyBindings::Quit:
			if (state == Down)
				handler.SendQuitEvent();
			break;
		case KeyBindings::SwitchCam:
			if (state == Down)
			{
				_mainPass->SetSpectatorActive(_keyBindings.camActive);
				_keyBindings.camActive = !_keyBindings.camActive;
				_keyBindings.camCtrls = true;
			}
			break;
		case KeyBindings::SwitchControls:
			if (state == Down)
			{
				if (!_keyBindings.camActive)
					_keyBindings.camCtrls = !_keyBindings.camCtrls;
			}
			break;
		case KeyBindings::Help:
			if (state == Down)
				_keyBindings.help = !_keyBindings.help;
			break;
		case KeyBindings::Stats:
			if (state == Down)
			{
				_keyBindings.stats = !_keyBindings.stats;
				_mainPass->GetKeyBindings().visStats = _keyBindings.stats;
			}
			break;
		case KeyBindings::ChangeDev:
			if (state == Down)
			{
				_settingsDlg.SetActive(!_settingsDlg.IsActive());
				if (_settingsDlg.IsActive())
				{
					EditSettingDlg();			
					StoreState();
				}
			}
			break;
		case KeyBindings::IncSpeed:
			if (state == Down)
			{
				_speed += 5;
				_time = time;
				sprintf_s(text, size, "Movement speed: %d", static_cast<int>(_speed));
				_info.assign(text);
			}
			break;
		case KeyBindings::DecSpeed:
			if (state == Down)
			{
				_speed = _speed < 6.0 ? 1 : _speed - 5;
				_time = time;
				sprintf_s(text, size, "Movement speed: %d", static_cast<int>(_speed));
				_info.assign(text);
			}
			break;
		case KeyBindings::HideGUI:
			if (state == Down)
				_keyBindings.showGui = !_keyBindings.showGui;
			break;
		case KeyBindings::DisplayShadowMap:
			if(state == Down)
				_mainPass->GetKeyBindings().drawDepthValues = true;
			else if(state == Up)
				_mainPass->GetKeyBindings().drawDepthValues = false;
			break;
		case KeyBindings::Shading:
			if(state == Down)
				_mainPass->GetKeyBindings().shading = !_mainPass->GetKeyBindings().shading;
			break;
	}
}

void ViewerApp::MouseEvent(EventHandler&, int, int, Button button, State state)
{
	//std::cout << newevent++ << ": Update Mouseevent" << std::endl;

	//switch(button)
	//{
	//	case Left:
			//if (state == Down)
			//{
			//	//_spectator.SetReferencePoint(tVec2i(x, y));
			//	POINT current;
			//	GetCursorPos(&current);
			//	_spectator.SetReferencePoint(tVec2i(current.x, current.y));
			//}
			//
			//_freeLook = (state == Down);
	//}

	//if (_freeLook)
	//{
	//	_refPoint = tVec2i(x, y);
	//}
}

void ViewerApp::MouseWheelEvent(EventHandler&, int delta)
{
	_speed += 5.0f * delta / WHEEL_DELTA;

	if (_speed < 6.0)
		_speed = 1.0;

	const unsigned int size = 256;
	const float time = 4.0f;
	char text[size];
	_time = time;
	sprintf_s(text, size, "Movement speed: %d", static_cast<int>(_speed));
	_info.assign(text);
}

//void ViewerApp::QuitEvent(EventHandler&)
//{
//	_quitFlag = true;
//}

void ViewerApp::DrawInitStatus(unsigned int percent)
{
	if (_initialized)
		return;

	_graphicsEngine->GetRenderSystem()->GetEffectManager()->Use(_drawQuadShader, 0, 0);
	_graphicsEngine->GetRenderSystem()->DrawStatus(percent);
}

void ViewerApp::RegisterEvents()
{
	EventHandler::Instance().Register(EventHandler::KeyEvent, *this);
	EventHandler::Instance().Register(EventHandler::MouseEvent, *this);
	EventHandler::Instance().Register(EventHandler::MouseWheelEvent, *this);
}

void ViewerApp::UnregisterEvents()
{
	EventHandler::Instance().UnRegister(EventHandler::KeyEvent, *this);
	EventHandler::Instance().UnRegister(EventHandler::MouseEvent, *this);
	EventHandler::Instance().UnRegister(EventHandler::MouseWheelEvent, *this);
}

