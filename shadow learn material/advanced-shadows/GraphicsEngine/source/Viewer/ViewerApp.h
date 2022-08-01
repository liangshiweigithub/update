#pragma once

#include "MainPass.h"
#include "DXUTApp.h"

class ViewerApp : public DXUTApp, EventHandler::EventListener
{
private:
	class DrawQuadShader : public D3dEffect
	{
	friend class ViewerApp;

	public:
		DrawQuadShader(const std::string& name);

	public:
		static const std::string DrawQuadSource;

	private:
		bool Initialize(ID3DX11Effect& effect);

	private:
		ID3DX11EffectVectorVariable* color;
		ID3DX11EffectShaderResourceVariable* diffuseTex;
	};
public:
	struct KeyBindings
	{
		static const unsigned int Forward;
		static const unsigned int Backward;
		static const unsigned int StrafeRight;
		static const unsigned int StrafeLeft;
		static const unsigned int RollLeft;
		static const unsigned int RollRight;
		static const unsigned int ResetUp;
		static const unsigned int Upward;
		static const unsigned int Downward;
		static const unsigned int IncSpeed;
		static const unsigned int DecSpeed;
		static const unsigned int Quit;
		static const unsigned int Stats;
		static const unsigned int ChangeDev;
		static const unsigned int Help;
		static const unsigned int SwitchCam;
		static const unsigned int SwitchControls;
		static const unsigned int SwitchLiSPMode;
		static const unsigned int SwitchFilter;
		static const unsigned int SaveCamPos;
		static const unsigned int LoadCamPos;
		static const unsigned int SaveLightPos;
		static const unsigned int LoadLightPos;
		static const unsigned int HideGUI;
		static const unsigned int LightMvUpward;
		static const unsigned int LightMvDownward;
		static const unsigned int LightMvLeft;
		static const unsigned int LightMvRight;
		static const unsigned int DisplayShadowMap;
		static const unsigned int Shading;

		KeyBindings();

		/* states */
		bool forward;
		bool backward;
		bool left;
		bool right;
		bool up;
		bool down;
		bool stats;
		bool help;
		bool advGui;
		bool camActive;
		bool camCtrls;
		bool showGui;
	};

	enum GuiBindings
	{
		GUIChangeDev = 1,
		GUIShowHelp,
		GUIShowStats,
		GUIToggleAdvGui,
		GUIChooseScene,
		ExtGUISplits,
		ExtGUILiSP,
		ExtGUIFilter,
		ExtGUISize,
		ExtGUIVisSplits,
		ExtGUIVisTexels,
		ExtGUIVisTexelSize,
		ExtGUIAlignLS,
		ExtGUIRedSwim,
		ExtGUILambda,
		ExtGUILambdaStatic,
		ExtGUILambdaLog,
		ExtGUILambdaLin,
		ExtGUIBestResSel,
		ExtGUIVisDepthMap,
		ExtGUIUseGS,
		ExtGUIBlurStatic,
		ExtGUIBlur,
		ExtGUIHWPCF,
		ExtGUIBlockerSearch,
		ExtGUIFocusing,
		ExtGUIVisSampling,
		ExtGUIChooseSmp,
		ExtGUIRndrShadows,
		ExtGUIParamN0,
		ExtGUIParamN1,
		ExtGUIParamN2,
		ExtGUIParamN3,
		ExtGUIParamN4,
		ExtGUIParamN5,
		ExtGUIParamN6,
		ExtGUIParamN7,
		ExtGUIParamN8,
		ExtGUIParamN9,
		ExtGUIParamN10,
		ExtGUIParamN11,
		ExtGUIParamN12,
		ExtGUIParamN13,
		ExtGUIParamN14,
		ExtGUIParamN15,
		ExtGUIParamNUniform1,
		ExtGUIParamNPSM1,
		ExtGUIParamNUniform2,
		ExtGUIParamNPSM2,
		ExtGUIParamNCurrent,
		ExtGUIParamNScaled,
		ExtGUIZNear,
		ExtGUIZNearSt,
		ExtGUIUseMinZ,
		ExtGUIVisError,
		ExtGUIPseudoNearSt,
		ExtGUIPseudoNear,
		ExtGUIPseudoFarSt,
		ExtGUIPseudoFar,
		ExtGUIDistributePrecision,
		ExtGUIESMConst,
		ExtGUIESMConstSt,
		ExtGUIESMConstStMin,
		ExtGUIESMConstStMax,
		ExtGUIESMLogBlur,
		ExtGUIESMPCFRegion,
		ExtGUIEVSMConstSt,
		ExtGUIEVSMConstPos,
		ExtGUIEVSMConstNeg,
		ExtGUIEVSMConstStMin,
		ExtGUIEVSMConstStMax,
		ExtGUILBRAmount,
		ExtGUILBRAmountSt,
		ExtGUILBRAmountStMin,
		ExtGUILBRAmountStMax,
		ExtGUIMipMap,
		ExtGUIVisMipMap,
		ExtGUIMaxSamplesSt,
		ExtGUIMaxSamplesStMin,
		ExtGUIMaxSamplesStMax,
		ExtGUIMaxSamples,
		ExtGUIGaussFilter,
		ExtGUIBoxFilter,
		ExtGUIFilterGroup,
		VisGUIBodyExact,
		VisGUIBodyAABB,
		VisGUICamFrust,
		VisGUIMiniMap,
		VisGUIBEGroup,
		VisGUIBESolid,
		VisGUIBEWire,
		VisGUIBAGroup,
		VisGUIBASolid,
		VisGUIBAWire,
		VisGUICFGroup,
		VisGUICFSolid,
		VisGUICFWire,
		ErrGUIZDir,
		ErrGUIXDir,
		ErrGUIYDir,
		ErrGUIXYComb
	};

	~ViewerApp();

	static ViewerApp& GetInstance();
	bool Init();
	
	bool Initialize();

	void Update(float elapsedTime);
	void Display(float elapsedTime);
	
	HRESULT OnResize(ID3D11Device* d3dDevice, IDXGISwapChain* swapChain, const DXGI_SURFACE_DESC* bufferSurfaceDesc);
	LRESULT OnMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* noFurtherProcessing);
	void OnGuiEvent(unsigned int eventId, int controlId, CDXUTControl* control);
	void OnDestroyDevice();

	void KeyEvent(EventHandler& handler, unsigned int key, State state);
	void MouseEvent(EventHandler& handler, int x, int y, Button button, State state);
	void MouseWheelEvent(EventHandler& handler, int delta);

private:
	ViewerApp();
	void RegisterEvents();
	void UnregisterEvents();
	void UpdateSpectator(Camera& camera, const float speed, bool roll);
	void UpdateLight(float speed);
	void UpdateErrHudLoc();
	void UpdateGuiElements();
	bool LoadScene();
	void RenderStats();
	void RenderHelp();
	void DeleteMembers();
	HRESULT InitGUIs();
	bool InitializeGUIs();
	void StoreState();
	void RestoreState();
	void DrawInitStatus(unsigned int percent);
	void UpdateGuiVisibility();
	void RenderInfo();

public:
	static const std::string ConfigFile;
	static const std::wstring Title;
	static const char* LoadCamString;
	static const char* LoadLightString;

private:
	static Logger logger;
	static const std::string SceneFile;
	static const std::string SkyDomeFile;
	static const std::string ModelPath;
	static const std::string HelpStrings[];
	static const std::string CamPosFile;
	static const std::string LightPosFile;
	static const unsigned int SliderRes;
	static const float Factor;

	CDXUTDialog _mainHud;
	CDXUTDialog _extHud;
	CDXUTDialog _visHud;
	CDXUTDialog _errHud;

	GraphicsEngine* _graphicsEngine;
	KeyBindings _keyBindings;
	Spectator _spectator;
	Timing _timing;
	bool _initialized;
	bool _noFurtherProcessing;
	bool _freeLook;
	bool _leftBtnClick;
	tVec2i _refPoint;
	float _renderTime;
	float _speed;
	float _time;
	std::string _info;
	bool _firstCycle;
	bool _updateErrHud;

	DrawQuadShader _drawQuadShader;
	MainPass* _mainPass;

	// state variables
	std::vector<unsigned int> _sizes;
	unsigned int _sceneId;
	unsigned int _nbrOfSplits;
	D3dShadowMapping::FilterMode _filterMode;
	unsigned int _shadowMapSz;
	bool _useLisp;
	LiSP::Mode _lispMode;
	bool _visTexels;
	int _texelSize;
	bool _visSplits;
	bool _redSwim;
	bool _visDepthMap;
	bool _bestResSel;
	bool _alignLS;
	bool _spectate;
	bool _visBodyExact;
	bool _visBodyAABB;
	bool _visCamFrust;
	bool _visMiniMap;
	bool _useGS;
	float _lambda;
	float _lightRadius;
	bool _useFocusing;
	bool _visBESolid;
	bool _visBASolid;
	bool _visCFSolid;
	bool _visSampling;
	bool _renderShadows;
	bool _distributePrecision;
	LiSP::tPArray _paramN;
	unsigned int _visError;
	unsigned int _visPerspErr;
	float _zNear;
	bool _useMinDepth;
	bool _visErrorGraph;
	float _pseudoNear;
	float _pseudoFar;
	tVec2f _evsmconst;
	float _lbramount;
	bool _nArbScaled;
	bool _useHWPCF;
	bool _useBlockerSearch;
	bool _useLogBlur;
	bool _visPCFRegions;
	bool _useMipMap;
	bool _gaussFilter;
	bool _visMipMap;
	unsigned int _maxsamples;
	int _warpSliderPosX;
	int _warpSliderPosY;
	int _btnWidth;
	int _btnHeight;
};