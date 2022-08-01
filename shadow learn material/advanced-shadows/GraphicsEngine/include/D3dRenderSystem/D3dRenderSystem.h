#pragma once

#include "D3dRenderSystemExports.h"

#include "SceneGraph/RenderSystem.h"

#include "D3dRenderer.h"
#include "D3dBufferManager.h"
#include "D3dEffectManager.h"
#include "D3dStateManager.h"
#include "D3dCuller.h"
#include "D3dMSAAMode.h"
#include "D3dRenderTarget.h"
#include "D3dEffect.h"
#include "D3dUtils.h"

class D3DRENDERSYSTEM_API D3dRenderSystem : public RenderSystem
{
public:
	typedef std::vector<D3dMSAAMode*> tMSAAModeList;
	typedef std::vector<tMSAAModeList> tMSAAModeArray;

	D3dRenderSystem();
	~D3dRenderSystem();

	/* special methods */
	bool Initialize(ID3D11Device& d3dDevice, ID3D11DeviceContext& d3dContext, IDXGISwapChain& swapChain, bool gammaCorrect);

	ID3D11Device* GetD3dDevice();
	ID3D11DeviceContext* GetD3dContext();
	D3dRenderer* GetRenderer();
	D3dEffectManager* GetEffectManager();
	D3dBufferManager* GetBufferManager();
	D3dStateManager* GetStateManager();
	const tMSAAModeList& GetMSAAModes(unsigned int format) const;
	
	/* from abstract interface */
	D3dCuller& GetCuller();	
	RSType GetType() const;

	void GenerateRessources(Model& model);
	void DeleteRessources(Model& model);

	void Push(const Material& material);
	void Pop(const Material& material);
	void Push(const Transform& transform);
	void Pop(const Transform& transform);

	void Draw(const Geometry& geometry);
	void Draw(const BoundingBox& boundingBox, const tVec4f& color);
	void Draw(const Frustum& frustum, const tVec4f& color);
	void Draw(const PolygonBody& body, const tVec4f& color);
	void Draw(const Line& line, const tVec4f& color);
	void Draw(const RenderSystem::tLineList& lineList, const tVec4f& color);
	void Draw(const Viewport& viewport, const tVec4f& color);
	void Draw(const Viewport& viewport);
	void Draw(const Viewport& inner, const Viewport& outer, const tVec4f& color);
	void Draw(const Line& line, const tVec4f& color, const Viewport& viewport);
	void Draw(const RenderSystem::tLineList& lineList, const tVec4f& color, const Viewport& viewport);
	void DrawStatus(unsigned int percent);

	const Viewport GetViewport();
	void SetViewport(const Viewport& viewport);
	void SetViewports(const tViewportArray& viewports);

	float* GetWorldMatrix();

	float* GetViewMatrix();
	float* GetViewMatrix(const View& view) const;
	void GetViewMatrix(D3DXMATRIX& viewMat, const View& view);
	void SetViewMatrix(float* viewMtx);
	void SetViewMatrix(const View& view);

	float* GetProjMatrix();
	float* GetProjMatrix(const Projection& projection) const;
	void SetProjMatrix(float* projMtx);
	void SetProjMatrix(const Projection& projection);

	tMat4f GetFrustumMatrix(float l, float r, float b, float t, float n, float f) const;

	Pass* GetActivePass();
	void SetActivePass(Pass* pass);

	RenderSystem::CullState GetCullState() const;
	void SetCullState(RenderSystem::CullState state);

	RenderSystem::PolygonMode GetPolygonMode() const;
	void SetPolygonMode(RenderSystem::PolygonMode mode);

	unsigned int GetTriangleCounter() const;
	unsigned int GetVertexCounter() const;
	void SetGeometryCounter(bool enable);

	void SetDrawBoundingBoxes(bool enable);
	bool GetDrawBoundingBoxes() const;

	void CheckError() const;
	const std::string GetErrorString(long errorCode) const;

	const bool IsInGammaCorrectMode() const;

private:
	void SetViewing(D3DXMATRIX& matrix, const View& view) const;
	void SetProjection(D3DXMATRIX& matrix, const Projection& projection) const;
	void DeleteMembers();
	void GenerateMSAAModeArray();
	void ComputePassSpecs(const Geometry& geometry);
	void Push();
	void Pop();

private:
	static Logger logger;

	static const unsigned int HighestSupportedFormat;
	static const DXGI_FORMAT DeprecatedFormats[];
	static const unsigned int NbrOfDepFormats;
	static const std::string BasicWorldMtx;
	static const std::string BasicViewMtx;
	static const std::string BasicProjMtx;
	static const std::string BasicColor;
	static const std::string BasicViewport;
	
	D3dRenderer* _renderer;
	D3dEffectManager* _effectManager;
	D3dBufferManager* _bufferManager;
	D3dStateManager* _stateManager;
	D3dCuller _culler;

	Pass* _activePass;

	ID3D11Device* _d3dDevice;
	ID3D11DeviceContext* _d3dContext;
	IDXGISwapChain* _swapChain;

	bool _geometryCount;
	unsigned int _vertexCount;
	unsigned int _triangleCount;
	unsigned int _indexId;
	unsigned int _vertexId;
	bool _drawBoundingBoxes;
	bool _gammaCorrect;

#pragma warning(push)
#pragma warning(disable : 4251)
	static const D3DXMATRIX Identity;
	D3DXMATRIX _viewMatrix;
	D3DXMATRIX _projMatrix;
	tMSAAModeArray _msaaModes;
#pragma warning(pop)
};