#include "stdafx.h"

#include "D3dRenderSystem.h"
#include "D3dShadowMapping.h"

/* D3dRenderSystem *********************************************************************/
Logger D3dRenderSystem::logger("D3dRenderSystem");

const unsigned int D3dRenderSystem::HighestSupportedFormat = static_cast<unsigned int>(DXGI_FORMAT_B8G8R8X8_UNORM);

const unsigned int D3dRenderSystem::NbrOfDepFormats = 3;

const DXGI_FORMAT D3dRenderSystem::DeprecatedFormats[D3dRenderSystem::NbrOfDepFormats] =
{
	DXGI_FORMAT_B5G6R5_UNORM, DXGI_FORMAT_B5G5R5A1_UNORM, DXGI_FORMAT_B8G8R8A8_UNORM
};

const std::string D3dRenderSystem::BasicWorldMtx("g_World");
const std::string D3dRenderSystem::BasicViewMtx("g_View");
const std::string D3dRenderSystem::BasicProjMtx("g_Proj");
const std::string D3dRenderSystem::BasicColor("g_color");
const std::string D3dRenderSystem::BasicViewport("g_viewport");

D3dRenderSystem::D3dRenderSystem() :
	_d3dDevice(0),
	_d3dContext(0),
	_renderer(0),
	_bufferManager(0),
	_effectManager(0),
	_stateManager(0),
	_msaaModes(HighestSupportedFormat),
	_indexId(0),
	_vertexId(0),
	_activePass(0),
	_drawBoundingBoxes(false),
	_swapChain(0),
	_gammaCorrect(true)
{
	D3DXMatrixIdentity(&_viewMatrix);
	D3DXMatrixIdentity(&_projMatrix);
}

D3dRenderSystem::~D3dRenderSystem()
{
	DeleteMembers();

	for (unsigned int i = 0; i < _msaaModes.size(); ++i)
		for (unsigned int j = 0; j < _msaaModes[i].size(); ++j)
			if (_msaaModes[i][j])
				delete _msaaModes[i][j];
}

bool D3dRenderSystem::Initialize(ID3D11Device& d3dDevice, ID3D11DeviceContext& d3dContext, IDXGISwapChain& swapChain, bool gammaCorrect)
{
	DeleteMembers();

	_gammaCorrect = gammaCorrect;

	_d3dDevice = &d3dDevice;
	_d3dContext = &d3dContext;

	_renderer = new D3dRenderer(d3dContext);	
	if (!_renderer->Initialize(*this))
		return false;
	
	_stateManager = new D3dStateManager(d3dDevice,d3dContext);
	if (!_stateManager->Initialize(*this))
		return false;

	_effectManager = new D3dEffectManager(d3dDevice);

	_bufferManager = new D3dBufferManager(d3dDevice,d3dContext);

	HRESULT hr = S_OK;

	hr = _bufferManager->CreateBuffer(D3D11_USAGE_DYNAMIC, sizeof(D3dRenderer::VertexData) * 2048,
									  D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0);

	if (FAILED(hr))
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << "Cannot create dynamic vertexbuffer (" <<
				GetErrorString(hr).c_str() << ").";
		return false;
	}

	_vertexId = _bufferManager->GetBufferArray().size() - 1;

	hr =  _bufferManager->CreateBuffer(D3D11_USAGE_DYNAMIC, sizeof(unsigned int) * 2048,
									   D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0);

	if (FAILED(hr))
	{
		if (logger.IsEnabled(Logger::Error))
			Logger::Stream(logger, Logger::Error) << "Cannot create dynamic indexbuffer (" <<
				GetErrorString(hr).c_str() << ").";
		return false;
	}

	_indexId = _bufferManager->GetBufferArray().size() - 1;
	
	_swapChain = &swapChain;

	GenerateMSAAModeArray();

	return true;
}

const bool D3dRenderSystem::IsInGammaCorrectMode() const
{
	return _gammaCorrect;
}

ID3D11Device* D3dRenderSystem::GetD3dDevice()
{
	return _d3dDevice;
}

ID3D11DeviceContext* D3dRenderSystem::GetD3dContext()
{
	return _d3dContext;
}

RenderSystem::RSType D3dRenderSystem::GetType() const
{
	return RenderSystem::Direct3d;
}

D3dRenderer* D3dRenderSystem::GetRenderer()
{
	return _renderer;
}

D3dEffectManager* D3dRenderSystem::GetEffectManager()
{
	return _effectManager;
}

D3dBufferManager* D3dRenderSystem::GetBufferManager()
{
	return _bufferManager;
}

D3dStateManager* D3dRenderSystem::GetStateManager()
{
	return _stateManager;
}

D3dCuller& D3dRenderSystem::GetCuller()
{
	return _culler;
}

const D3dRenderSystem::tMSAAModeList& D3dRenderSystem::GetMSAAModes(unsigned int format) const
{
	if (format > HighestSupportedFormat)
		return _msaaModes[0];

	return _msaaModes[format];
}

void D3dRenderSystem::GenerateRessources(Model& model)
{
	for (unsigned int i = 0; i < model.GetMeshCount(); ++i)
		_bufferManager->Generate(*model.GetMesh(i), *this);

	for (unsigned int i = 0; i < model.GetImageCount(); ++i)
		_bufferManager->Generate(*model.GetImage(i), *this);
}

void D3dRenderSystem::DeleteRessources(Model& model)
{
	for (unsigned int i = 0; i < model.GetMeshCount(); ++i)
		_bufferManager->Delete(*model.GetMesh(i));

	for (unsigned int i = 0; i < model.GetImageCount(); ++i)
		_bufferManager->Delete(*model.GetImage(i));
}

void D3dRenderSystem::Push(const Material& material)
{
	_stateManager->Push(material, *_effectManager, *_bufferManager);
}

void D3dRenderSystem::Pop(const Material& material)
{
	_stateManager->Pop(material);
}

void D3dRenderSystem::Push(const Transform& transform)
{
	_stateManager->Push(transform, *_effectManager);
}

void D3dRenderSystem::Pop(const Transform& transform)
{
	_stateManager->Pop(transform);
}

void D3dRenderSystem::Draw(const Geometry& geometry)
{
	if (_geometryCount)
	{
		_triangleCount += geometry.GetVertexIndicesSize() / 3;
		_vertexCount += geometry.GetVertexIndicesSize();
	}

	if (_activePass)
		ComputePassSpecs(geometry);

	_renderer->Draw(geometry, *_bufferManager, *_effectManager);
}

void D3dRenderSystem::Draw(const BoundingBox& boundingBox, const tVec4f& color)
{
	Push();
	bool wireframe = _stateManager->GetPolygonMode() == Wireframe;

	_effectManager->Use("RenderBasic3d", wireframe ? 1 : 0);
	
	_effectManager->SendUniformMat4f(BasicWorldMtx, _stateManager->GetWorldMatrix());
	_effectManager->SendUniformMat4f(BasicViewMtx, _viewMatrix);
	_effectManager->SendUniformMat4f(BasicProjMtx, _projMatrix);
	_effectManager->SendUniform4fv(BasicColor, color);	

	_renderer->Draw(boundingBox, *_bufferManager, *_effectManager, _indexId, _vertexId, wireframe);
	Pop();
}

void D3dRenderSystem::Draw(const Frustum& frustum, const tVec4f& color)
{
	Push();
	bool wireframe = _stateManager->GetPolygonMode() == Wireframe;

	_effectManager->Use("RenderBasic3d", wireframe ? 1 : 0);

	_effectManager->SendUniformMat4f(BasicWorldMtx, _stateManager->GetWorldMatrix());
	_effectManager->SendUniformMat4f(BasicViewMtx, _viewMatrix);
	_effectManager->SendUniformMat4f(BasicProjMtx, _projMatrix);
	_effectManager->SendUniform4fv(BasicColor, color);

	_renderer->Draw(frustum, *_bufferManager, *_effectManager, _indexId, _vertexId, wireframe);
	Pop();
}

void D3dRenderSystem::Draw(const PolygonBody& body, const tVec4f& color)
{
	Push();
	bool wireframe = _stateManager->GetPolygonMode() == Wireframe;

	_effectManager->Use("RenderBasic3d", wireframe ? 1 : 0);

	_effectManager->SendUniformMat4f(BasicWorldMtx, _stateManager->GetWorldMatrix());
	_effectManager->SendUniformMat4f(BasicViewMtx, _viewMatrix);
	_effectManager->SendUniformMat4f(BasicProjMtx, _projMatrix);
	_effectManager->SendUniform4fv(BasicColor, color);
	
	_renderer->Draw(body, *_bufferManager, *_effectManager, _indexId, _vertexId);
	Pop();
}

void D3dRenderSystem::Draw(const Line& line, const tVec4f& color)
{
	Push();
	_effectManager->Use("RenderBasic3d", 1);

	_effectManager->SendUniformMat4f(BasicWorldMtx, _stateManager->GetWorldMatrix());
	_effectManager->SendUniformMat4f(BasicViewMtx, _viewMatrix);
	_effectManager->SendUniformMat4f(BasicProjMtx, _projMatrix);
	_effectManager->SendUniform4fv(BasicColor, color);

	_renderer->Draw(line, *_bufferManager, *_effectManager, _indexId, _vertexId);
	Pop();
}

void D3dRenderSystem::Draw(const RenderSystem::tLineList& lineList, const tVec4f& color)
{
	Push();
	_effectManager->Use("RenderBasic3d", 1);

	_effectManager->SendUniformMat4f(BasicWorldMtx, _stateManager->GetWorldMatrix());
	_effectManager->SendUniformMat4f(BasicViewMtx, _viewMatrix);
	_effectManager->SendUniformMat4f(BasicProjMtx, _projMatrix);
	_effectManager->SendUniform4fv(BasicColor, color);

	_renderer->Draw(lineList, *_bufferManager, *_effectManager, _indexId, _vertexId);
	Pop();
}

void D3dRenderSystem::Draw(const Viewport& viewport)
{
	Viewport vp = GetViewport();
	
	SetViewport(viewport);
	_renderer->Draw(viewport, *_effectManager);

	SetViewport(vp);
}

void D3dRenderSystem::Draw(const Viewport& viewport, const tVec4f& color)
{
	Push();

	_effectManager->Use("RenderBasic2d", 0);

	_effectManager->SendUniform4fv(BasicColor, color);

	Draw(viewport);

	Pop();
}

void D3dRenderSystem::Draw(const Viewport& inner, const Viewport& outer, const tVec4f& color)
{
	Push();
	Viewport vp = GetViewport();

	Viewport diff[4] = 
	{
		Viewport(0, 0, inner.x, outer.height),
		Viewport(inner.x, 0, inner.width, inner.y),
		Viewport(inner.x + inner.width, 0, outer.width - (inner.x + inner.width), outer.height),
		Viewport(inner.x, inner.y + inner.height, inner.width, outer.height - (inner.y + inner.height))
	};

	_effectManager->Use("RenderBasic2d", 0);
	_effectManager->SendUniform4fv(BasicColor, color);

	for (unsigned int i = 0; i < 4; ++i)
	{
		SetViewport(diff[i]);	
		_renderer->Draw(diff[i], *_effectManager);
	}

	SetViewport(vp);
	Pop();
}

void D3dRenderSystem::Draw(const Line& line, const tVec4f& color, const Viewport& viewport)
{
	Push();
	Viewport vp = GetViewport();

	SetViewport(viewport);
	_effectManager->Use("RenderBasic3d", 2);

	_effectManager->SendUniform2iv(BasicViewport, tVec2i(static_cast<int>(viewport.width), static_cast<int>(viewport.height)));
	_effectManager->SendUniform4fv(BasicColor, color);

	_renderer->Draw(line, *_bufferManager, *_effectManager, _indexId, _vertexId);

	SetViewport(vp);
	Pop();
}

void D3dRenderSystem::Draw(const RenderSystem::tLineList& lineList, const tVec4f& color, const Viewport& viewport)
{
	Push();
	Viewport vp = GetViewport();

	SetViewport(viewport);
	_effectManager->Use("RenderBasic3d", 2);

	_effectManager->SendUniform2iv(BasicViewport, tVec2i(static_cast<int>(viewport.width), static_cast<int>(viewport.height)));
	_effectManager->SendUniform4fv(BasicColor, color);

	_renderer->Draw(lineList, *_bufferManager, *_effectManager, _indexId, _vertexId);

	SetViewport(vp);
	Pop();
}


void D3dRenderSystem::DrawStatus(unsigned int percent)
{
	Push();

	ID3D11RenderTargetView* rtv = 0;
	ID3D11DepthStencilView* dsv = 0;

	_d3dContext->OMGetRenderTargets(1, &rtv, &dsv);
	_d3dContext->ClearRenderTargetView(rtv, D3DXVECTOR4(.0f, .0f, .0f, 1.0f));

	Viewport vp = GetViewport();
	_effectManager->Use("DisplayQuad", 0);

	unsigned int width = static_cast<unsigned int>(0.01 * percent * (vp.width - 208));

	Viewport vpBorder(100, vp.height - 200, vp.width - 200, 30);
	Viewport vpback(102, vp.height - 198, vp.width - 204, 26);
	Viewport vpfore(104, vp.height - 196, width, 22);
	
	_effectManager->SendUniform4fv(BasicColor, tVec4f(1.0f, 1.0f, 1.0f, 1.0f));
	Draw(vpBorder);
	
	_effectManager->SendUniform4fv(BasicColor, tVec4f(.0f, .0f, .0f, 1.0f));
	Draw(vpback);
	
	_effectManager->SendUniform4fv(BasicColor, tVec4f(1.0f, .0f, .0f, 1.0f));
	Draw(vpfore);

	if (rtv)
		rtv->Release();

	if (dsv)
		dsv->Release();

	Pop();

	_swapChain->Present(0, 0);
}

const RenderSystem::Viewport D3dRenderSystem::GetViewport()
{
	unsigned int num = 1;
	D3D11_VIEWPORT vp;

	_d3dContext->RSGetViewports(&num, &vp);

	return RenderSystem::Viewport(vp.TopLeftX, vp.TopLeftY, vp.Width, vp.Height);
}

void D3dRenderSystem::SetViewport(const Viewport& viewport)
{
	D3D11_VIEWPORT vp;	
	vp.Width    = viewport.width;
	vp.Height   = viewport.height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = viewport.x;
	vp.TopLeftY = viewport.y;

	_d3dContext->RSSetViewports(1, &vp);
}

void D3dRenderSystem::SetViewports(const tViewportArray& viewports)
{
	unsigned int num = static_cast<unsigned int>(viewports.size());

	D3D11_VIEWPORT* vps = new D3D11_VIEWPORT[num]; 

	for (unsigned int i = 0; i < num; ++i)
	{
		vps[i].Width    = viewports[i]->width;
		vps[i].Height   = viewports[i]->height;
		vps[i].MinDepth = 0.0f;
		vps[i].MaxDepth = 1.0f;
		vps[i].TopLeftX = viewports[i]->x;
		vps[i].TopLeftY = viewports[i]->y;
	}

	_d3dContext->RSSetViewports(num, vps);

	delete[] vps;
}

float* D3dRenderSystem::GetWorldMatrix()
{
	return _stateManager->GetWorldMatrix();
}

float* D3dRenderSystem::GetViewMatrix()
{
	return &_viewMatrix.m[0][0];
}

float* D3dRenderSystem::GetViewMatrix(const View& view) const
{
	D3DXMATRIX viewMat;
	SetViewing(viewMat, view);

	return &viewMat.m[0][0];
}

void D3dRenderSystem::GetViewMatrix(D3DXMATRIX& viewMat, const View& view)
{
	SetViewing(viewMat, view);
}

void D3dRenderSystem::SetViewMatrix(float* viewMtx)
{
	_viewMatrix = *(reinterpret_cast<D3DXMATRIX*>(viewMtx));
}

float* D3dRenderSystem::GetProjMatrix()
{
	return &_projMatrix.m[0][0];
}

float* D3dRenderSystem::GetProjMatrix(const Projection& projection) const
{
	D3DXMATRIX projMat;
	SetProjection(projMat, projection);

	return &projMat.m[0][0];
}

void D3dRenderSystem::SetProjMatrix(float* projMtx)
{
	_projMatrix = *(reinterpret_cast<D3DXMATRIX*>(projMtx));
}

void D3dRenderSystem::SetViewMatrix(const View& view)
{
	SetViewing(_viewMatrix, view);
}

void D3dRenderSystem::SetProjMatrix(const Projection& projection)
{
	SetProjection(_projMatrix, projection);
}

tMat4f D3dRenderSystem::GetFrustumMatrix(float l, float r, float b, float t, float n, float f) const
{
	return tMat4f(
		2*n/(r-l), .0f,       (l+r)/(r-l), .0f,
		.0f,       2*n/(t-b), (t+b)/(t-b), .0f,
		.0f,       .0f,       f/(n-f),     n*f/(n-f),
		.0f,       .0f,       -1.0f,       .0f);

	//return tMat4f(
	//	2*n/(r-l), .0f,       .0f, .0f,
	//	.0f,       2*n/(t-b), .0f, .0f,
	//	.0f,       .0f,       f/(n-f),     n*f/(n-f),
	//	.0f,       .0f,       -1.0f,       .0f);
}

Pass* D3dRenderSystem::GetActivePass()
{
	return _activePass;
}

void D3dRenderSystem::SetActivePass(Pass* pass)
{
	_activePass = pass;
}

void D3dRenderSystem::SetViewing(D3DXMATRIX& matrix, const View& view) const
{
	const D3DXVECTOR3 eye(view.GetEye().x, view.GetEye().y, view.GetEye().z);
	const D3DXVECTOR3 center(view.GetCenter().x, view.GetCenter().y, view.GetCenter().z);
	const D3DXVECTOR3 up(view.GetUp().x, view.GetUp().y, view.GetUp().z);

	D3DXMatrixLookAtRH(&matrix, &eye, &center, &up);
}

void D3dRenderSystem::SetProjection(D3DXMATRIX& matrix, const Projection& projection) const
{
	switch(projection.GetType())
	{
		case Projection::Perspective:
		{
			const Perspective& pProj = dynamic_cast<const Perspective&>(projection);
			D3DXMatrixPerspectiveFovRH(&matrix, static_cast<float>(DEG2RAD(pProj.GetFOV())), pProj.GetAspect(), pProj.GetZNear(), pProj.GetZFar());
		}
		break;

		case Projection::Ortho3d:
		{
			const Ortho3d& oProj = dynamic_cast<const Ortho3d&>(projection);
			D3DXMatrixOrthoRH(&matrix, abs(oProj.GetLeft() - oProj.GetRight()),
									   abs(oProj.GetBottom() - oProj.GetTop()),
									   oProj.GetZNear(), oProj.GetZFar());
		}
		break;
	}
}

void D3dRenderSystem::ComputePassSpecs(const Geometry& geometry)
{
	D3dShadowMapping* sm = dynamic_cast<D3dShadowMapping*>(_activePass);

	if (sm && sm->GetUseGS())
	{
		sm->GetGenDepthMapEffect().firstSplit->SetInt(geometry.GetMesh()->GetFirstSplit());
		sm->GetGenDepthMapEffect().lastSplit->SetInt(geometry.GetMesh()->GetLastSplit());
	}
}

RenderSystem::CullState D3dRenderSystem::GetCullState() const
{
	return _stateManager->GetCullState();
}

void D3dRenderSystem::SetCullState(RenderSystem::CullState state)
{
	_stateManager->SetCullState(state);
}

RenderSystem::PolygonMode D3dRenderSystem::GetPolygonMode() const
{
	return _stateManager->GetPolygonMode();
}

void D3dRenderSystem::SetPolygonMode(RenderSystem::PolygonMode mode)
{
	_stateManager->SetPolygonMode(mode);
}

unsigned int D3dRenderSystem::GetTriangleCounter() const
{
	return _triangleCount;
}

unsigned int D3dRenderSystem::GetVertexCounter() const
{
	return _vertexCount;
}

void D3dRenderSystem::SetGeometryCounter(bool enable)
{
	_geometryCount = enable;
	_triangleCount = 0;
	_vertexCount = 0;
}

void D3dRenderSystem::SetDrawBoundingBoxes(bool enable)
{
	_drawBoundingBoxes = enable;
}

bool D3dRenderSystem::GetDrawBoundingBoxes() const
{
	return _drawBoundingBoxes;
}

void D3dRenderSystem::CheckError() const
{
	// nothing to do
}

void D3dRenderSystem::Push()
{
	_effectManager->Push();
}

void D3dRenderSystem::Pop()
{
	_effectManager->Pop();
}

const std::string D3dRenderSystem::GetErrorString(long errorCode) const
{
	std::string returnString("");

	switch (errorCode)
	{
		case D3D11_ERROR_FILE_NOT_FOUND:
			returnString.assign("File not found.");
			break;
		case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
			returnString.assign("Too many unique state objects.");
			break;
		case D3DERR_INVALIDCALL:
			returnString.assign("Invalid method call");
			break;
		case D3DERR_WASSTILLDRAWING:
			returnString.assign("Incomplete previous blit operation.");
			break;
		case E_FAIL:
			returnString.assign("Failed to create a device with the not installed debug layer.");
			break;
		case E_INVALIDARG:
			returnString.assign("Failed to pass invalid parameter to the returning function.");
			break;
		case E_OUTOFMEMORY:
			returnString.assign("Cannot allocate sufficient memory to complete the call.");
			break;
		case S_FALSE:
			returnString.assign("Error depending on context.");
			break;
		default:
			returnString.assign("Unknown Error");
			break;
	}

	return returnString;
}

void D3dRenderSystem::DeleteMembers()
{
	if (_bufferManager)
		delete _bufferManager;

	if (_effectManager)
		delete _effectManager;

	if (_stateManager)
		delete _stateManager;

	if (_renderer)
		delete _renderer;
}

void D3dRenderSystem::GenerateMSAAModeArray()
{
	for (unsigned int i = 1; i < HighestSupportedFormat; ++i)
	{
		bool deprecated = false;
		DXGI_FORMAT format = static_cast<DXGI_FORMAT>(i);

		for (unsigned int j = 0; j < NbrOfDepFormats && !deprecated; ++j)
			deprecated = format == DeprecatedFormats[j];

		if (deprecated)
			continue;

		for (unsigned int samples = 2; samples < D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; ++samples)
		{
			unsigned int nbrOfQualities;
			unsigned int formatSupport;

			if (SUCCEEDED(_d3dDevice->CheckMultisampleQualityLevels(format, samples, &nbrOfQualities)) &&
				nbrOfQualities > 0 &&
				SUCCEEDED(_d3dDevice->CheckFormatSupport(format, &formatSupport)) &&
				formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET &&
				formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE)
			{
				std::stringstream ss;
				ss << samples << "x";
				_msaaModes[i].push_back(new D3dMSAAMode(ss.str(), samples, nbrOfQualities - 1));
			}
		}
	}
}
