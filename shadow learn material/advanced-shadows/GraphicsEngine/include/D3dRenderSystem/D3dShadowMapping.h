#pragma once

#include "D3dRenderSystemExports.h"

#include "SceneGraph/LiSP.h"

#include "D3dRenderTarget.h"
#include "D3dEffect.h"

class D3dRenderSystem;

class D3DRENDERSYSTEM_API D3dShadowMapping : public Pass
{
private:
	class D3DRENDERSYSTEM_API GenDepthMapEffect : public D3dEffect
	{
	friend class D3dShadowMapping;
	friend class D3dRenderSystem;

	public:
		GenDepthMapEffect(const std::string& name);

		void UniformCallBack(D3dEffectManager& effectManager, const Material& material);
		void UniformCallBack(D3dEffectManager& effectManager, const Texture& texture, ID3D11ShaderResourceView& resource, float* texMtx);
		void UniformCallBack(D3dEffectManager& effectManager, const Transform& transform);

	private:
		bool Initialize(ID3DX11Effect& effect);

	private:
		static const std::string GenDepthMapSourceFile;
		static const std::string NbrOfSplitsMacro;
		ID3DX11EffectShaderResourceVariable* diffuseTex;
		ID3DX11EffectMatrixVariable* world;
		ID3DX11EffectMatrixVariable* view;
		ID3DX11EffectMatrixVariable* projection;
		ID3DX11EffectMatrixVariable* tex;
		ID3DX11EffectScalarVariable* textured;
		ID3DX11EffectVectorVariable* matDiffuse;
		ID3DX11EffectMatrixVariable* viewProj;
		ID3DX11EffectMatrixVariable* crop;
		ID3DX11EffectMatrixVariable* liSP;
		ID3DX11EffectScalarVariable* firstSplit;
		ID3DX11EffectScalarVariable* lastSplit;
		ID3DX11EffectVectorVariable* evsmconst;
	};

public:
	class D3DRENDERSYSTEM_API DispDepthMapEffect : public D3dEffect
	{
	friend class D3dShadowMapping;

	public:
		DispDepthMapEffect(const std::string& name);

	private:
		bool Initialize(ID3DX11Effect& effect);

	private:
		static const std::string DispDepthMapSourceFile;
		ID3DX11EffectShaderResourceVariable* depthMap;
		ID3DX11EffectShaderResourceVariable* idepthMap;
		ID3DX11EffectShaderResourceVariable* depthMapArray;
		ID3DX11EffectShaderResourceVariable* texMap;
		ID3DX11EffectShaderResourceVariable* texMapArray;
		ID3DX11EffectScalarVariable* arrayIndex;
		ID3DX11EffectVectorVariable* mapSize;
		ID3DX11EffectMatrixVariable* tex;
		ID3DX11EffectVectorVariable* texBounds;
		ID3DX11EffectVectorVariable* evsmconst;
		ID3DX11EffectScalarVariable* filterWidth;
	};

	class D3DRENDERSYSTEM_API GenSATEffect : public D3dEffect
	{
	friend class D3dShadowMapping;

	public:
		GenSATEffect(const std::string& name);

	private:
		bool Initialize(ID3DX11Effect& effect);

	private:
		static const std::string SATSourceFile;

		ID3DX11EffectShaderResourceVariable* map;
		ID3DX11EffectShaderResourceVariable* imap;
		ID3DX11EffectScalarVariable* offset;
		ID3DX11EffectScalarVariable* samplesPerPass;
		ID3DX11EffectScalarVariable* texheight;
		ID3DX11EffectScalarVariable* texwidth;
		ID3DX11EffectMatrixVariable* tex;
		ID3DX11EffectVectorVariable* texBounds;
	};

	class D3DRENDERSYSTEM_API BlurEffect : public D3dEffect
	{
	friend class D3dShadowMapping;

	public:
		BlurEffect(const std::string& name);

	private:
		bool Initialize(ID3DX11Effect& effect);

	private:
		static const std::string BlurSourceFile;
		ID3DX11EffectShaderResourceVariable* map;
		ID3DX11EffectVectorVariable* size;
		ID3DX11EffectVectorVariable* dim;
		ID3DX11EffectScalarVariable* samples;
		ID3DX11EffectMatrixVariable* tex;
		ID3DX11EffectVectorVariable* texBounds;
		ID3DX11EffectScalarVariable* gaussianWeights;
	};

	enum FilterMode
	{
		None = 0,
		HWPCF,
		Box4x4PCF,
		Poisson25PCF,
		Box6x6PCF,
		Poisson32PCF,
		Box8x8PCF,
		Poisson64PCF,
		PCFPCSS,
		ESM,
		VSM,
		EVSM,
		VarBoxPCF,
		SAVSM,
		UINTSAVSM
	};

	enum ErrDir
	{
		ZDir = 0,
		XDir,
		YDir,
		//Storage,
		ErrDirCOUNT
	};

	typedef std::vector<D3DXMATRIX> tMatrixArray;
	typedef std::vector<float> tPosArray;
	typedef std::vector<Frustum*> tFrustumArray;
	typedef std::vector<BoundingBox*> tBBArray;
	typedef std::vector<tVec2f> tDistArray;
	typedef std::vector<PolygonBody*> tBodyArray;
	typedef std::vector<D3DXVECTOR2> tVecArray;
	typedef std::vector<D3dRGFloatRT*> tMipmapChain;
	typedef std::vector<tVec4f> tUVArray;
	typedef std::vector< std::vector<tVec3f> > tErrValsArray;
	typedef std::vector<unsigned int> tSplitSmpleArray;

	D3dShadowMapping();
	~D3dShadowMapping();

	bool Initialize(D3dRenderSystem& renderSystem);
	void Render();

	const D3dRenderTarget& GetShadowMaps() const;
	float* GetNormLightViewProjMtx();
	const tMatrixArray& GetTexMatrixArray() const;
	float* GetSplitPositions();
	const tPosArray& GetSplitPos() const;
	float* GetLightViewMtx();
	float* GetLightProjMtx(unsigned int split);
	const tFrustumArray& GetSplitFrustums() const;
	tBBArray& GetSplitBBs();
	const tDistArray& GetNearAndFar() const;
	float GetAngleCamDirLightDir() const;
	const tBodyArray& GetBodyArray() const;	
	const tErrValsArray& GetErrorValues() const;
	const tDistArray& GetErrorRanges() const;

	GenDepthMapEffect& GetGenDepthMapEffect();

	unsigned int GetShadowMapSize() const;
	void SetShadowMapSize(unsigned int size);

	unsigned int GetRenderTargetSize();

	FilterMode GetFilterMode() const;
	void SetFilterMode(FilterMode mode);

	unsigned int GetNbrOfSplits() const;
	void SetNbrOfSplits(unsigned int nbrOfSplits);

	bool GetSwimmingRed() const;
	void SetSwimmingRed(bool enable);

	bool GetUseLiSP() const;
	void SetUseLiSP(bool enable);
	LiSP& GetLiSP();

	bool GetUseGS() const;
	void SetUseGS(bool enable);

	bool GetUseMinDepth() const;
	void SetUseMinDepth(bool enable);

	bool GetDistributePrecision() const;
	void SetDistributePrecision(bool enable);

	void SetDebug(bool enable);
	
	const Projection& GetLightProjection() const;
	void SetLightProjection(const Projection& proj);

	const View& GetLightView() const;
	void SetLightView(const View& view);

	Camera& GetCamera();
	void SetCamera(Camera& camera);

	float GetSplitParam() const;
	void SetSplitParam(float lambda);

	float GetBlurRadius() const;
	void SetBlurRadius(float width);
	int* GetFilterWidths();
	
	bool GetAlignmentLSVS() const;
	void SetAlignmentLSVS(bool enable);

	float* GetSplitScales();
	float GetLightRadius() const;
	void SetLightRadius(float radius);

	bool GetUseFocusing() const;
	void SetUseFocusing(bool enable);

	float GetZNear() const;
	void SetZNear(float value);

	const tVec2f& GetEVSMConst() const;
	void SetEVSMConst(float pos, float neg);

	bool GetUseLogBlur() const;
	void SetUseLogBlur(bool enable);

	bool GetMipMapped() const;
	void SetMipMapped(bool enable);

	bool GetUseGaussFilter() const;
	void SetUseGaussFilter(bool enable);

	float GetPseudoNear() const;
	void SetPseudoNear(float value);

	float GetPseudoFar() const;
	void SetPseudoFar(float value);

	bool GetCalcError() const;
	void SetCalcError(bool enable);

	bool GetUseBBSel() const;
	void SetUseBBSel(bool enable);

	const tVec2f& GetDepthRange() const;

	const tSplitSmpleArray& GetSplitPosSamples() const;

	const DispDepthMapEffect& Get2dEffect() const;
	
	const tSplitSmpleArray& GetPseudoNFSamples() const;

	void UseScene(unsigned int id);
	void DisplayDepthMaps(const RenderSystem::Viewport& viewport);
	void DisplayMap(RenderSystem::Viewport& viewport, const D3dRenderTarget& rt);
	bool GenerateMinMaxMipmap();
	bool Compile();

	bool CheckRecompile(FilterMode mode) const;
	unsigned int GetFilterMacroVal(FilterMode mode) const;

private:
	void GenerateArrays();
	void DeleteArrays();
	void UpdateEyeViewProj();
	void UpdateLDirVDirAngle(const View& eView, const View& lView);
	void UpdatePointLightViewProj(const View& eView);
	void UpdateDirLightViewProj(const View& eView, const PolygonBody& body);
	bool GenerateRenderTargets();
	void DeleteRenderTargets();
	D3dRenderTarget& GetActiveRenderTarget();
	void AdjustCameraPlanes(const BoundingBox& aabb, const Frustum& frust, float& zNear, float& zFar);
	//void AdjustCameraPlanes(const BoundingBox& aabb, float& zNear, float& zFar);
	void CalcSplitPositions(float zNear, float zFar, unsigned int nbrOfSplits);
	void GenerateDepthMap(unsigned int split);
	void CalcPolygonBodyB(PolygonBody& bodyB, const Projection& lProj, const View& lView, const Frustum& cFrust, const Frustum* lFrust, BoundingBox& s);
	void CalcLightViewProjMat(unsigned int split, const PolygonBody& body, const tMat4f& lProjMat, const tMat4f& lViewMat);
	void GenerateDebugInfos(const tMat4f& invEyeViewProj, const PolygonBody& body, unsigned int split);
	void GenerateTexAtlasConstants(float unit, unsigned int factor);
	void FindCasters(unsigned int split);
	void Blur(D3dRenderTarget& src, D3dRenderTarget& dest, const tVec2f& dim, unsigned int split);
	void CreateSAT(D3dRenderTarget* src, D3dRenderTarget* dest, const std::string technique, const unsigned int split);
	float GetMinDepth();
	void DeleteMinMaxMipmap();
	void CalcErrorValues();
	void CopyTexture(ID3D11Texture2D* texture);
	void ComputeGaussianWeights();
	
public:
	static const unsigned int MaxNbrOfSplits;

private:
	static const unsigned int NbrOfSamples;

	D3dRenderSystem* _renderSystem;
	
	GenDepthMapEffect _genSMEffect;
	DispDepthMapEffect _dispSMEffect;
	BlurEffect _blurSMEffect;
	GenSATEffect _genSATEffect;

	View _lView;
	Projection* _lProj;
	Camera* _camera;
	LiSP* _liSP;

	D3dFloatDepthRT _depthMap;
	D3dFloatRT _esmMap;
	D3dRGFloatRT _blur;
	D3dRGFloatRT _vsmMap;
	D3dRGFloatRT _vsmSat;
	D3dRGBAFloatRT _evsmMap;
	D3dRGBAFloatRT _vsmDistrMap;
	D3dRGBAFloatRT _vsmDistrSat;
	D3dRGUintRT _vsmUintMap;
	D3dRGUintRT _vsmUintSat;
	D3dRGFloatCPURT _minmaxMIP;

	unsigned int _shadowMapSz;
	unsigned int _nbrOfSplits;
	unsigned int _sceneId;
	unsigned int _samplesPerPassSat;

	bool _useLiSP;
	bool _updateLightUp;
	bool _reduceSwimming;
	bool _useGS;
	bool _debug;
	bool _useMinDepth;
	FilterMode _filterMode;
	bool _focusing;
	bool _useBBSel;
	bool _calcError;
	bool _distributePrecision;
	bool _logblur;
	bool _mipmapped;
	bool _useGaussFilter;

	float _gamma;
	float _angleCamDirLightDir;
	float _lambda;
	float _blurRadius;
	float _blurSplitCor;
	float _lightRadius;
	float _lightRadiusSplitCor;
	float _zNear;
	float _pseudoNear;
	float _pseudoFar;
	tVec2f _evsmConst;

#pragma warning(push)
#pragma warning(disable : 4251)
	static const D3DXMATRIX BiasMatrices[];
	static const tVec4f UV4[4];
	static const tVec4f UV9[9];
	static const tVec4f UV16[16];

	D3DXMATRIX _lViewMat;
	D3DXMATRIX _lProjMat;
	D3DXMATRIX _eViewMat;
	D3DXMATRIX _eProjMat;
	D3DXMATRIX _lSplitProjMat;
	D3DXMATRIX _lSplitViewMat;

	RenderSystem::tViewportArray _viewports;
	tMatrixArray _normLightViewProj;
	tMatrixArray _lightView;
	tMatrixArray _lightProj;
	tPosArray _splitPositions;
	tFrustumArray _splitFrustums;
	tBBArray _splitBBs;
	tBodyArray _intersectionBodys;
	tDistArray _planesNearFar;
	tMatrixArray _normMatrices;
	tMatrixArray _cropMatrices;
	tMatrixArray _liSPMatrices;
	tDistArray _filterScales;
	tVecArray _splitScales;
	tVec2f _depthRange;
	tMipmapChain _camDepthMipmap;
	tErrValsArray _errVals;
	tDistArray _errRanges;
	tUVArray _UV;
	tSplitSmpleArray _splitErrorSmples;
	tSplitSmpleArray _errPseudoNF;
	tSplitSmpleArray _filterWidths;
	tPosArray _gaussianWeights;

#pragma warning(pop)

};