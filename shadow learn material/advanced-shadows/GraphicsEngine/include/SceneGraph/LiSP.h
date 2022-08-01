#pragma once

#include "SceneGraphExports.h"
#include "Common/VectorMathTypes.h"
#include "Projection.h"
#include "Frustum.h"
#include "BoundingBox.h"
#include "PolygonBody.h"
#include "View.h"
#include "RenderSystem.h"

class SCENEGRAPH_API LiSP
{
public:
	enum Mode
	{
		NOpt = 1,
		Reparametrized,
		Arbitrary,
		PseudoNear,
		ModeCOUNT
	};

	typedef std::vector< std::vector<tVec2f> > tParamArray;
	typedef std::vector<float> tPArray;

	LiSP(unsigned int nbrOfSplits);

	void SetLightProjMat(const tMat4f& matrix, Projection::Type type);
	void SetLightView(const tMat4f& matrix, const View& view);
	void SetCamView(const tMat4f& matrix, const View& view);
	void SetCamProjMat(const tMat4f& matrix);
	void SetGamma(float gamma);
	void SetFieldOfView(float fov);
	void SetSplitParam(float lambda);
	void SetPseudoNear(float value);
	void SetPseudoFar(float value);
	
	const tPArray& GetParamN() const;
	void SetParamN(float n, unsigned int split);
	const tParamArray& GetParams() const;
	
	Mode GetMode() const;
	void SetMode(Mode mode);

	float GetParam() const;
	const tParamArray& GetDistances() const;

	bool GetArbScaled() const;
	void SetArbScaled(bool enable);

	const tMat4f& GetLiSPMtx(unsigned int split, const RenderSystem& renderSystem, const PolygonBody& bodyB,
		const Frustum& camFrust, const Frustum* lightFrust, const BoundingBox& sceneAABB, const tVec2f& zRange);

private:
	tVec3f GetNearCamPoint(const Frustum& camFrust, const Frustum* lightFrust, const BoundingBox& sceneAABB);
	float GetDistN(const tMat4f& lViewProjMat, const BoundingBox& bodyBaabb_LS, const tVec3f& nearCamPt,
		float nPseudo, float fPseudo, unsigned int split);
	float GetReparamDistN(const tMat4f& lViewProjMat, const BoundingBox& bodyBaabb_LS,
		const tVec3f& nearCamPt, const Frustum& camFrust, float nPseudo, float fPseudo, unsigned int split);
	float GetDistArbN(const tMat4f& lViewProjMat, const BoundingBox& bodyBaabb_LS, const tVec3f& nearCamPt, unsigned int split, const tVec2f& zRange);
	float GetPseudoNearN(const tMat4f& lViewProjMat, const BoundingBox& bodyBaabb_LS,  const tVec3f& nearCamPt, float nPseudo,
		unsigned int split);
	void CalcZ0Z1_LS(tVec3f& z0_LS, tVec3f& z1_LS, const tMat4f& lViewProjMat,
		const BoundingBox& bodyBaabb_LS, const tVec3f& nearCamPt) const;

public:
	static const float Infinity;

private:
	Projection::Type _lType;
	View _cView;
	View _lView;

	Mode _mode;
	bool _arbScaled;

	float _gamma;
	float _fov;
	float _eta;
	float _lambda;
	float _pseudoNear;
	float _pseudoFar;

#pragma warning(push)
#pragma warning(disable : 4251)
	static const tMat4f Transform;
	tMat4f _lViewMat;
	tMat4f _lProjMat;
	tMat4f _cViewMat;
	tMat4f _cProjMat;
	tMat4f _liSPMat;
	tParamArray _distances;
	tParamArray _nParams;
	tPArray _paramN;
#pragma warning(pop)
};