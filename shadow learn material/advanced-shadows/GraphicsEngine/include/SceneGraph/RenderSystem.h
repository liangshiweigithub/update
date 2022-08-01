#pragma once

#include <string>
#include <vector>

#include "SceneGraphExports.h"
#include "Common/VectorMathTypes.h"

class Geometry;
class Model;
class Scene;
class View;
class Projection;
class Culler;
class BoundingBox;
class Frustum;
class PolygonBody;
class TrackBar;
class Material;
class Transform;
class Line;
class Pass;


class SCENEGRAPH_API RenderSystem
{
public:
	enum RSType
	{
		OpenGL = 0,
		Direct3d
	};

	enum CullState
	{
		CullBackFace = 0,
		CullFrontFace,
		CullFaceOff
	};

	enum PolygonMode
	{
		Fill = 0,
		Wireframe
	};

	enum TextureFilter
	{
		Nearest = 0,
		NearestBilinear,
		NearestTrilinear,
		Linear,
		Bilinear,
		Trilinear
	};

	enum AnisotropicFilter
	{
		AnisotropyOff = 0,
		Anisotropy2,
		Anisotropy4,
		Anisotropy8,
		Anisotropy16
	};

	struct Viewport
	{
		Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) :
				 x(x), y(y), width(width), height(height) {}
		unsigned int x;
		unsigned int y;
		unsigned int width;
		unsigned int height;
	};

	typedef std::vector<Viewport*> tViewportArray;
	typedef std::vector<Line*> tLineList;

	virtual ~RenderSystem() {}

	//virtual bool Initialize() = 0;	

	virtual Culler& GetCuller() = 0;
	virtual RSType GetType() const = 0;

	virtual void GenerateRessources(Model& model) = 0;
	virtual void DeleteRessources(Model& model) = 0;

	virtual void Push(const Material& material) = 0;
	virtual void Pop(const Material& material) = 0;
	virtual void Push(const Transform& transform) = 0;
	virtual void Pop(const Transform& transform) = 0;

	virtual void Draw(const Geometry& geometry) = 0;
	virtual void Draw(const BoundingBox& boundingBox, const tVec4f& color) = 0;
	virtual void Draw(const Frustum& frustum, const tVec4f& color) = 0;
	virtual void Draw(const PolygonBody& body, const tVec4f& color) = 0;
	virtual void Draw(const Line& line, const tVec4f& color) = 0;
	virtual void Draw(const tLineList& lineList, const tVec4f& color) = 0;
	virtual void Draw(const Viewport& viewport, const tVec4f& color) = 0;
	virtual void Draw(const Viewport& viewport) = 0;
	virtual void Draw(const Viewport& inner, const Viewport& outer, const tVec4f& color) = 0;
	virtual void Draw(const Line& line, const tVec4f& color, const Viewport& viewport) = 0;
	virtual void Draw(const tLineList& lineList, const tVec4f& color, const Viewport& viewport) = 0;
	virtual void DrawStatus(unsigned int percent) = 0;

	virtual const Viewport GetViewport() = 0;
	virtual void SetViewport(const Viewport& viewport) = 0;
	virtual void SetViewports(const tViewportArray& viewports) = 0;
	
	virtual float* GetWorldMatrix() = 0;

	virtual float* GetViewMatrix() = 0;
	virtual float* GetViewMatrix(const View& view) const = 0;
	virtual void SetViewMatrix(float* viewMtx) = 0;
	virtual void SetViewMatrix(const View& view) = 0;
	
	virtual float* GetProjMatrix() = 0;
	virtual float* GetProjMatrix(const Projection& projection) const = 0;
	virtual void SetProjMatrix(float* projMtx) = 0;
	virtual void SetProjMatrix(const Projection& projection) = 0;

	virtual tMat4f GetFrustumMatrix(float l, float r, float b, float t, float n, float f) const = 0;
	
	virtual Pass* GetActivePass() = 0;
	virtual void SetActivePass(Pass* pass) = 0;

	virtual CullState GetCullState() const = 0;
	virtual void SetCullState(CullState state) = 0;

	virtual PolygonMode GetPolygonMode() const = 0;
	virtual void SetPolygonMode(PolygonMode mode) = 0;

	virtual unsigned int GetTriangleCounter() const = 0;
	virtual unsigned int GetVertexCounter() const = 0;
	virtual void SetGeometryCounter(bool enable) = 0;

	virtual void SetDrawBoundingBoxes(bool enable) = 0;
	virtual bool GetDrawBoundingBoxes() const = 0;

	virtual void CheckError() const = 0;
	virtual const std::string GetErrorString(long errorCode) const = 0;
};
