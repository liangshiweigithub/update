#pragma once

#include "DXUT/Opt/SDKmisc.h"
#include "SkyDome.h"
#include "D3dRenderSystem/D3dShadowMapping.h"

class MainPass : public Pass, EventHandler::EventListener
{
private:
	class SingleSMShader : public D3dEffect
	{
	friend class MainPass;

	public:
		SingleSMShader(const std::string& name);
		virtual ~SingleSMShader();

		virtual void UniformCallBack(D3dEffectManager& effectManager, const Material& material);
		virtual void UniformCallBack(D3dEffectManager& effectManager, const Texture& texture, ID3D11ShaderResourceView& resource, float* texMtx);
		virtual void UniformCallBack(D3dEffectManager& effectManager, const Transform& transform);

	public:
		static const std::string PCFPresetMacro;
		static const std::string SingleSMSource;

	protected:
		bool InitializeVariables(ID3DX11Effect& effect);

	private:
		bool Initialize(ID3DX11Effect& effect);

	protected:
		SingleSMShader();
		ID3DX11EffectShaderResourceVariable* diffuseTex;
		ID3DX11EffectShaderResourceVariable* shadowTex;
		ID3DX11EffectShaderResourceVariable* ishadowTex;
		ID3DX11EffectMatrixVariable* world;
		ID3DX11EffectMatrixVariable* view;
		ID3DX11EffectMatrixVariable* projection;
		ID3DX11EffectMatrixVariable* lightview;
		ID3DX11EffectVariable* effectLight;
		ID3DX11EffectMatrixVariable* tex;
		ID3DX11EffectScalarVariable* textured;
		ID3DX11EffectScalarVariable* gammaCorrect;
		ID3DX11EffectVectorVariable* matAmbient;
		ID3DX11EffectVectorVariable* matDiffuse;
		ID3DX11EffectVectorVariable* matSpecular;
		ID3DX11EffectScalarVariable* matShininess;
		ID3DX11EffectMatrixVariable* lightViewProj;
		ID3DX11EffectScalarVariable* visTexels;
		ID3DX11EffectScalarVariable* shadowMapSz;
		ID3DX11EffectVectorVariable* lightRadius;
		ID3DX11EffectScalarVariable* visTexelSz;
		ID3DX11EffectVectorVariable* camPos;
		ID3DX11EffectScalarVariable* visSampling;
		ID3DX11EffectScalarVariable* renderShadows;
		ID3DX11EffectScalarVariable* visError;
		ID3DX11EffectMatrixVariable* camView;
		ID3DX11EffectVectorVariable* viewVec;
		ID3DX11EffectVectorVariable* lightVec;
		ID3DX11EffectScalarVariable* useHWPCF;
		ID3DX11EffectScalarVariable* useBlockerSearch;
		ID3DX11EffectScalarVariable* shading;
		ID3DX11EffectScalarVariable* distributePrecision;
		ID3DX11EffectVectorVariable* evsmconst;
		ID3DX11EffectScalarVariable* lbramount;
		ID3DX11EffectScalarVariable* useLogBlur;
		ID3DX11EffectScalarVariable* filterWidth;
		ID3DX11EffectScalarVariable* visPCFRegions;
		ID3DX11EffectScalarVariable* useMipMap;
		ID3DX11EffectScalarVariable* visMipMap;
		ID3DX11EffectScalarVariable* maxsamples;
	};

	class PSSMShader : public SingleSMShader
	{
	friend class MainPass;

	public:
		PSSMShader(const std::string& name);

	public:
		static const std::string PCFPresetMacro;
		static const std::string NbrOfSplitsMacro;
		static const std::string PSSMSource;

	private:
		bool Initialize(ID3DX11Effect& effect);

	private:
		ID3DX11EffectScalarVariable* splits;
		ID3DX11EffectScalarVariable* visSplits;
		ID3DX11EffectScalarVariable* shadowRTSz;
	};

public:
	struct KeyBindings
	{
		static const unsigned int AABBDrawing;
		static const unsigned int ToggleLightProj;
		static const unsigned int DrawBodyAABB;
		static const unsigned int DrawBody;
		static const unsigned int UseHierarchy;

		KeyBindings();

		bool drawShadowMap;
		bool drawDepthValues;
		bool showSplits;
		bool directionalLight;
		bool drawMiniMap;
		bool drawErrGraph;
		bool drawBodyAABB;
		bool drawBodyExact;
		bool drawCamFrustum;
		bool showTexels;
		bool useHierarchy;
		bool showAABBs;
		bool aabbBodySolid;
		bool exactBodySolid;
		bool camFrustSolid;
		bool showSampling;
		bool showShadows;
		bool visStats;
		int visError;
		unsigned int visPerspError;
		bool useHWPCF;
		bool useBlockerSearch;
		bool shading;
		bool distributePrecision;
		float lbramount;
		bool useLogBlur;
		bool visPCFRegions;
		bool useMipMap;
		bool visMipMap;
		unsigned int maxsamples;
	};

	struct EffectLight
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR4 Diffuse;
		D3DXVECTOR4 Specular;
		D3DXVECTOR4 Ambient;
	};

	enum Map
	{
		DepthMap = 0,
		ErrGraph,
		MiniMap
	};

	MainPass(Scene& scene, CDXUTTextHelper& txtHelper);
	~MainPass();

	Camera& GetCamera();
	SkyDome& GetSkyDomePass();
	D3dShadowMapping& GetShadowMappingPass();

	bool Initialize(RenderSystem& renderSystem, const Perspective& p, const View& v);
	void UpdateLightView(float xAngle, float yAngle, const tVec4f& refPos);
	void Render();

	KeyBindings& GetKeyBindings();
	
	Light& GetLight();

	int GetVisTexelSz() const;
	void SetVisTexelSz(int size);

	bool GetBestResSplitSel() const;
	void SetBestResSplitSel(bool enable);
	
	bool GetSpectatorActive() const;
	void SetSpectatorActive(bool enable);

	unsigned int GetSceneId() const;
	void SetSceneId(unsigned int id);

	unsigned int GetNbrOfSplits() const;
	void SetNbrOfSplits(unsigned int nbrOfSplits);

	D3dShadowMapping::FilterMode GetFilterMode() const;
	void SetFilterMode(D3dShadowMapping::FilterMode mode);

	const std::string GetStats();
	
	void KeyEvent(EventHandler& handler, unsigned int key, State state);
	void MouseEvent(EventHandler& handler, int x, int y, Button button, State state);

	RenderSystem::Viewport* GetMapVP(Map map);
	bool IsEnlarged(Map map) const;

private:
	void SetupView(bool spectate);
	void RenderDebugInfos();
	void RenderMiniMap(bool enlarged);
	void RenderDepthMap(bool enlarged);
	void RenderErrGraph(bool enlarged);
	//void RenderCursorText(bool enlarged, const tVec2i& cursor);
	void RenderSingleSM(float& view, float& proj);
	void RenderPSSM(float& view, float& proj);
	void UpdateLightProj();
	//void RenderGraph();
	unsigned int GetMapCoords(Map map) const;
	unsigned int GetPass() const;
	void RegisterEvents();
	void UnregisterEvents();

private:
	static const float LFov;
	static const float LAspect;
	static const float LZNear;
	static const float LZFar;
	static const float HalfLSize;
	
	KeyBindings _keyBindings;
	D3dRenderSystem* _renderSystem;
	Scene* _scene;
	Camera _spectator;
	CDXUTTextHelper* _txtHelper;

	D3dShadowMapping _shadowMapping;

	Light _light;
	EffectLight _effectlight;
	tVec4f _lightRefPos;
	tVec2i _lightRef;
	bool _rotateLight;
	tVec2i _cursor;
	
	D3DXMATRIX _viewMat;
	D3DXMATRIX _projMat;

	SkyDome _skyPass;

	SingleSMShader _singlesmShader;
	PSSMShader _pssmShader;

	unsigned int _sceneId;
	bool _useBestResSplitSel;
	bool _spectatorActive;

	bool _enlargedMiniMap;
	bool _shadeMiniMap;
	RenderSystem::Viewport _miniMap;

	bool _enlargedDepthMap;
	bool _shadeDepthMap;
	RenderSystem::Viewport _depthMap;

	bool _enlargedErrGraph;
	bool _shadeErrGraph;
	RenderSystem::Viewport _errGraph;

	float _lightRadius;
	int _texelSz;
};