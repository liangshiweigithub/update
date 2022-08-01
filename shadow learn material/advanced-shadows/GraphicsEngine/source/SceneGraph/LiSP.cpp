#include "stdafx.h"

#include <limits>
#include "LiSP.h"

const float LiSP::Infinity = 9.0e10f;

const tMat4f LiSP::Transform = tMat4f
(
	1.0f,   .0f,  .0f,  .0f,
	 .0f,   .0f, 1.0f,  .0f, // y -> z
	 .0f,  1.0f,  .0f,  .0f, // z -> y
	 .0f,   .0f,  .0f, 1.0f
);

LiSP::LiSP(unsigned int nbrOfSplits) :
	_lType(Projection::Ortho3d),
	_lViewMat(vmath::identity4<float>()),
	_lProjMat(vmath::identity4<float>()),
	_liSPMat(vmath::identity4<float>()),
	_cView(tVec3f(.0f, .0f, .0f), tVec3f(.0f, .0f, 1.0f), tVec3f(.0f, 1.0f, .0f)),
	_lView(tVec3f(.0f, 1.0f, .0f), tVec3f(.0f, .0f, 0.0f), tVec3f(.0f, .0f, -1.0f)),
	_mode(NOpt),
	_distances(ModeCOUNT),
	_nParams(ModeCOUNT),
	_paramN(nbrOfSplits, 1.0f),
	_gamma(90.0f),
	_fov(45.0f),
	_eta(-1.0f),
	_lambda(0.5f),
	_pseudoNear(.0f),
	_pseudoFar(.0f),
	_arbScaled(false)
{
	for (unsigned int i = NOpt; i < ModeCOUNT; ++i)
	{
		_nParams[i].resize(nbrOfSplits);
		_distances[i].resize(nbrOfSplits);
	}
}

void LiSP::SetLightProjMat(const tMat4f& matrix, Projection::Type type)
{
	_lProjMat = matrix;
	_lType = type;
}

void LiSP::SetLightView(const tMat4f& matrix, const View& view)
{
	_lViewMat = matrix;
	_lView = view;
}

void LiSP::SetCamView(const tMat4f& matrix, const View& view)
{
	_cViewMat = matrix;
	_cView = view;
}

void LiSP::SetCamProjMat(const tMat4f& matrix)
{
	_cProjMat = matrix;
}

void LiSP::SetGamma(float gamma)
{
	_gamma = gamma;
}

void LiSP::SetFieldOfView(float fov)
{
	_fov = fov;
}

void LiSP::SetSplitParam(float lambda)
{
	_lambda = lambda;
}

LiSP::Mode LiSP::GetMode() const
{
	return _mode;
}

void LiSP::SetMode(Mode mode)
{
	_mode = mode;
}

const LiSP::tParamArray& LiSP::GetDistances() const
{
	return _nParams;
}

float LiSP::GetParam() const
{
	return _eta;
}

const LiSP::tPArray& LiSP::GetParamN() const
{
	return _paramN;
}

void LiSP::SetParamN(float n, unsigned int split)
{
	if (split < _paramN.size())
		_paramN[split] = n;
}

void LiSP::SetPseudoNear(float value)
{
	_pseudoNear = value;
}

void LiSP::SetPseudoFar(float value)
{
	_pseudoFar = value;
}

bool LiSP::GetArbScaled() const
{
	return _arbScaled;
}

void LiSP::SetArbScaled(bool enable)
{
	_arbScaled = enable;
}

const tMat4f& LiSP::GetLiSPMtx(unsigned int split, const RenderSystem& renderSystem, const PolygonBody& bodyB,
							   const Frustum& camFrust, const Frustum* lightFrust, const BoundingBox& sceneAABB,
							   const tVec2f& zRange)
{
	tMat4f lProjMat = _lProjMat;
	tMat4f lViewMat = _lViewMat;

	const unsigned int nbrOfSplits = _paramN.size();
	unsigned int size = bodyB.GetVertexCount();
	const Polygon3d::tVertexArray& vertices = bodyB.GetVertexArray();

	tVec3f nearCamPt = GetNearCamPoint(camFrust, lightFrust, sceneAABB);		

	tMat4f lViewProjMat = lProjMat * lViewMat;

	const tVec3f viewDir(vmath::normalize(_cView.GetCenter() - _cView.GetEye()));
	const tVec3f leftVec(vmath::normalize(vmath::cross(_cView.GetUp(), viewDir)));
	const tVec3f up(vmath::normalize(vmath::cross(vmath::normalize(_lView.GetCenter() - _lView.GetEye()), leftVec)));
	const tVec3f center = nearCamPt + viewDir;

	tVec3f projViewDir_LS(vmath::transform(lViewProjMat, center) - vmath::transform(lViewProjMat, nearCamPt));		
	projViewDir_LS.z = .0f;

	// do Light Space Perspective shadow mapping
	// rotate the lightspace so that the proj light view always points upwards
	// calculate a frame matrix that uses the projViewDir[light-space] as up vector
	// look(from position, into the direction of the projected direction, with unchanged up-vector)
	tMat4f viewMat = vmath::look_matrix(tVec3f(.0f), projViewDir_LS, tVec3f(.0f, .0f, 1.0f));
	lProjMat = viewMat * lProjMat;

	lViewProjMat = lProjMat * lViewMat;

	BoundingBox bodyBaabb_LS;
	for (unsigned int i = 0; i < size; ++i)
		bodyBaabb_LS.Expand(vertices[i], lViewProjMat);

	_distances[NOpt][split].x = GetDistN(lViewProjMat, bodyBaabb_LS, nearCamPt,
		split == 0 ? _pseudoNear : .0f, split == nbrOfSplits - 1 ? _pseudoFar : .0f, split);
	
	_distances[Reparametrized][split].x = GetReparamDistN(lViewProjMat, bodyBaabb_LS, nearCamPt, camFrust,
		split == 0 ? _pseudoNear : .0f, split == nbrOfSplits - 1 ? _pseudoFar : .0f, split);

	_distances[Arbitrary][split].x = GetDistArbN(lViewProjMat, bodyBaabb_LS, nearCamPt, split, zRange);

	_distances[PseudoNear][split].x = GetPseudoNearN(lViewProjMat, bodyBaabb_LS, nearCamPt,
		split == 0 ? _pseudoNear : .0f, split); 

	const float projLen = abs(vmath::dot(viewDir, nearCamPt - _cView.GetEye()));
	const tVec3f nearCamCtr(_cView.GetEye() + projLen * viewDir);
	const tVec3f nearCamCtr_LS(vmath::transform(lViewProjMat, nearCamCtr));
	
	//tVec3f nearCamPt_LS(vmath::transform(lViewProjMat, nearCamPt));
	//const tVec3f cStart_LS = tVec3f(nearCamPt_LS.x, nearCamPt_LS.y, bodyBaabb_LS.GetMaximum().z);

	// c_start has the x and y coordinate of nearCamCtr_LS, the z coord of B.min() 
	const tVec3f cStart_LS = tVec3f(nearCamCtr_LS.x, nearCamCtr_LS.y, bodyBaabb_LS.GetMaximum().z);

	for (unsigned int mode = NOpt; mode < ModeCOUNT; ++mode)
	{
		float n = _distances[mode][split].x;
		
		// if n < infinity calculate LiSP matrix
		if (n < std::numeric_limits<float>::infinity())
		{
			// calc depthrange of the perspective transform depth or light space y extents
			float zRange = abs(bodyBaabb_LS.GetMinimum().z - bodyBaabb_LS.GetMaximum().z);
			_distances[mode][split].y = n + zRange;
			
			if (mode == static_cast<unsigned int>(_mode))
			{
				// calc projection center
				tVec3f projCenter(cStart_LS + tVec3f(.0f, .0f, n));

				tVec3f projCtr = vmath::transform(vmath::inverse(lViewProjMat), projCenter);

				// the lispsm perspective transformation
				// here done with a standard frustum call that maps lispPersp onto the unit cube
				tMat4f liSP = renderSystem.GetFrustumMatrix(-1.0f, 1.0f, -1.0f, 1.0f, n, n + zRange);
				liSP *= vmath::translation_matrix(-projCenter);
				liSP = Transform * liSP;
				
				_liSPMat = liSP * viewMat;
			}
		}
		else
		{
			//float inf = std::numeric_limits<float>::infinity();
			float inf = Infinity;
			_distances[mode][split] = tVec2f(inf, inf);
			_nParams[mode][split] = tVec2f(inf, inf);

			if (mode == static_cast<unsigned int>(_mode))
				_liSPMat = vmath::identity4<float>();
		}
	}

	return _liSPMat;
}

tVec3f LiSP::GetNearCamPoint(const Frustum& camFrust, const Frustum* lightFrust, const BoundingBox& sceneAABB)
{
	PolygonBody bodyLVS;

	bodyLVS.Add(camFrust);

	bodyLVS.Clip(sceneAABB);

	// perspective lightProjection
	if (_lType == Projection::Perspective) bodyLVS.Clip(*lightFrust);

	bodyLVS.CreateUniqueVertexArray();

	const Polygon3d::tVertexArray& vertices = bodyLVS.GetVertexArray();
	unsigned int size = bodyLVS.GetVertexCount();

	if (size)
	{
		// transform to eyespace and find the nearest point to camera -> z = max()
		unsigned int index = 0;
		for (unsigned int i = 1; i < size; ++i)
		{
			if (vmath::transform(_cViewMat, vertices[index]).z < vmath::transform(_cViewMat, vertices[i]).z)
				index = i;
		}
		return vertices[index];
	}
	else
	{
		return _cView.GetEye() + vmath::normalize(_cView.GetCenter() - _cView.GetEye());
	}
}

void LiSP::CalcZ0Z1_LS(tVec3f& z0_LS, tVec3f& z1_LS,
					   const tMat4f& lViewProjMat, const BoundingBox& bodyBaabb_LS, const tVec3f& nearCamPt) const
{
	// calculate the parallel plane to the near plane through eye
	Plane planeZ0(nearCamPt, _cView.GetCenter() - _cView.GetEye());
	
	// transform into lightspace with transposed inverted lightViewProj
	planeZ0.Transform(vmath::transpose(vmath::inverse(lViewProjMat)));

	// get the parameters of plane from the plane equation n dot d = 0
	const tVec3f& normal = planeZ0.GetNormal();
	const float distance = planeZ0.GetDistance();

	// z0_LS has the x coordinate of nearCamPoint_LS, the y coord of 
	// the plane intersection and the z coord of bodyB_LS.max() 
	const float bMaxZ = bodyBaabb_LS.GetMaximum().z;
	const float bMinZ = bodyBaabb_LS.GetMinimum().z;

	const tVec3f nearCamPt_LS(vmath::transform(lViewProjMat, nearCamPt));
	
	z0_LS = tVec3f
	(
		nearCamPt_LS.x,
		(distance - normal.z * bMaxZ - normal.x * nearCamPt_LS.x) / normal.y,
		bMaxZ
	);

	z1_LS = tVec3f
	(
		z0_LS.x, z0_LS.y, bMinZ
	);
}

float LiSP::GetDistN(const tMat4f& lViewProjMat, const BoundingBox& bodyBaabb_LS, const tVec3f& nearCamPt,
					 float nPseudo, float fPseudo, unsigned int split)
{
	tVec3f z0_LS;
	tVec3f z1_LS;

	CalcZ0Z1_LS(z0_LS, z1_LS, lViewProjMat, bodyBaabb_LS, nearCamPt);

	// transform to world
	const tVec3f z0_WS(vmath::transform(vmath::inverse(lViewProjMat), z0_LS));
	const tVec3f z1_WS(vmath::transform(vmath::inverse(lViewProjMat), z1_LS));

	// transform to eye
	const tVec3f z0_ES(vmath::transform(_cViewMat, z0_WS));
	const tVec3f z1_ES(vmath::transform(_cViewMat, z1_WS));

	const float n_e = abs(z0_ES.z);
	const float f_e = abs(z1_ES.z);
	const float zRange_LS = abs(bodyBaabb_LS.GetMaximum().z - bodyBaabb_LS.GetMinimum().z);
	const float zRange_ES = abs(f_e - n_e);

	//const float n = zRange_LS / ( sqrt(f_e / n_e) - 1.0f );
	const float n = zRange_LS / (f_e - n_e) * (n_e + sqrt((n_e + nPseudo * (f_e - n_e)) * (f_e - fPseudo * (f_e - n_e))));

	_nParams[NOpt][split].x = zRange_ES / zRange_LS * n;
	_nParams[NOpt][split].y = _nParams[NOpt][split].x + zRange_ES;

	return n;
}

float LiSP::GetReparamDistN(const tMat4f& lViewProjMat, const BoundingBox& bodyBaabb_LS, const tVec3f& nearCamPt,
							const Frustum& camFrust, float /*nPseudo*/, float /*fPseudo*/, unsigned int split)
{
	tVec3f z0_LS;
	tVec3f z1_LS;

	CalcZ0Z1_LS(z0_LS, z1_LS, lViewProjMat, bodyBaabb_LS, nearCamPt);

	// transform to world
	const tVec3f z0_WS(vmath::transform(vmath::inverse(lViewProjMat), z0_LS));
	const tVec3f z1_WS(vmath::transform(vmath::inverse(lViewProjMat), z1_LS));

	// transform to eye
	const tVec3f z0_ES(vmath::transform(_cViewMat, z0_WS));
	const tVec3f z1_ES(vmath::transform(_cViewMat, z1_WS));

	const float theta = _fov / 2.0f;
	const float gamma_a = theta / 3.0f;
	const float gamma_b = theta;
	const float gamma_c = theta + 0.3f * (90.0f - theta);
	const float eta_b = -0.2f;
	const float eta_c = 0.0f;
	const float n_e = abs(z0_ES.z);
	const float f_e = abs(z1_ES.z);

	if (_gamma <= gamma_a)
	{
		_eta = -1;
	}
	else if (_gamma > gamma_a && _gamma <= gamma_b)
	{
		_eta = -1 + (eta_b + 1) * (_gamma - gamma_a) / (gamma_b - gamma_a);
	}
	else if (_gamma > gamma_b && _gamma <= gamma_c)
	{
		_eta = eta_b + (eta_c - eta_b) * sin(90.0f * (_gamma - gamma_b)/(gamma_c - gamma_b) * static_cast<float>(M_PI) / 180.0f);
	}
	else // gamma > gamma_c
	{
		_eta = eta_c;
	}

	const float w_n = vmath::length(camFrust.GetCorner(Frustum::NearBottomRight) - camFrust.GetCorner(Frustum::NearTopRight));
	//const double w_f = w_n * f_e / n_e;
	const float w_s = (f_e - n_e) / cos(theta * static_cast<float>(M_PI) / 180.0f);

	const float w_n_proj = w_n * cos(_gamma * static_cast<float>(M_PI) / 180.0f);
	const float w_s1_proj = _gamma <= theta ? w_s * (1 - cos((theta - _gamma) * static_cast<float>(M_PI) / 180.0f)) : 0.0f;
	const float w_s2_proj = w_s * sin((theta + _gamma) * static_cast<float>(M_PI) / 180.0f);

	const float w_lz = w_n_proj + w_s1_proj + w_s2_proj; // equivalent to w_ly in paper
	const float alpha = f_e / n_e;

	float n = 0.0f;

	if (_eta < 0.0f)
	{
		n = w_lz / (alpha - 1) * (1 + sqrt(alpha) - _eta * (alpha - 1)) / (_eta + 1);
	}
	else
	{
		n = w_lz / (alpha - 1) * (1 + sqrt(alpha)) / (_eta * sqrt(alpha) + 1);
	}

	const float zRange_LS = abs(bodyBaabb_LS.GetMaximum().z - bodyBaabb_LS.GetMinimum().z);
	const float zRange_ES = f_e - n_e;

	_nParams[Reparametrized][split] = tVec2f(n, n + zRange_ES);

	return zRange_LS / zRange_ES * n;
}

float LiSP::GetDistArbN(const tMat4f& lViewProjMat, const BoundingBox& bodyBaabb_LS, const tVec3f& nearCamPt,
						unsigned int split, const tVec2f& zRange)
{
	if (_arbScaled && split > 0)
	{
		const float n = zRange.x;
		const float f = zRange.y;
		const float s = split / static_cast<float>(_paramN.size());
		const float schemeFactor = ((1.0f - _lambda) * (n + (f - n) * s) + _lambda * (n * powf(f / n, s)));
		
		_nParams[Arbitrary][split] = tVec2f(schemeFactor * _nParams[Arbitrary][0]);
		_paramN[split] = _nParams[Arbitrary][split].x;

		return _distances[Arbitrary][0].x / _nParams[Arbitrary][0].x * _nParams[Arbitrary][split].x;
	}
	else
	{
		tVec3f z0_LS;
		tVec3f z1_LS;

		CalcZ0Z1_LS(z0_LS, z1_LS, lViewProjMat, bodyBaabb_LS, nearCamPt);

		// transform to world
		const tVec3f z0_WS(vmath::transform(vmath::inverse(lViewProjMat), z0_LS));
		const tVec3f z1_WS(vmath::transform(vmath::inverse(lViewProjMat), z1_LS));

		// transform to eye
		const tVec3f z0_ES(vmath::transform(_cViewMat, z0_WS));
		const tVec3f z1_ES(vmath::transform(_cViewMat, z1_WS));

		const float n_e = abs(z0_ES.z);
		const float f_e = abs(z1_ES.z);

		const float zRange_LS = abs(bodyBaabb_LS.GetMaximum().z - bodyBaabb_LS.GetMinimum().z);
		const float zRange_ES = f_e - n_e;

		//const float n = max(_paramN[split], n_e);
		const float n = _paramN[split];

		_nParams[Arbitrary][split] = tVec2f(n, n + zRange_ES);

		return zRange_LS / zRange_ES * n;
	}
}

float LiSP::GetPseudoNearN(const tMat4f& lViewProjMat, const BoundingBox& bodyBaabb_LS, const tVec3f& nearCamPt,
						   float nPseudo, unsigned int split)
{
	if (nPseudo >= 2 / 3.0f)
		nPseudo = 2 / 3.0f;

	tVec3f z0_LS;
	tVec3f z1_LS;

	CalcZ0Z1_LS(z0_LS, z1_LS, lViewProjMat, bodyBaabb_LS, nearCamPt);

	// transform to world
	const tVec3f z0_WS(vmath::transform(vmath::inverse(lViewProjMat), z0_LS));
	const tVec3f z1_WS(vmath::transform(vmath::inverse(lViewProjMat), z1_LS));

	// transform to eye
	const tVec3f z0_ES(vmath::transform(_cViewMat, z0_WS));
	const tVec3f z1_ES(vmath::transform(_cViewMat, z1_WS));

	const float n_e = abs(z0_ES.z);
	const float f_e = abs(z1_ES.z);
	const float zRange_LS = abs(bodyBaabb_LS.GetMaximum().z - bodyBaabb_LS.GetMinimum().z);
	const float zRange_ES = f_e - n_e;
	
	const float n_0 = n_e / zRange_ES;
	const float n1 = n_0 + sqrt( ( n_0 + nPseudo ) * ( n_0 + 1 ) );
	const float n2 = abs( nPseudo / ( 2 - 3 * nPseudo ) );

	float n = max(n1, n2);

	_nParams[PseudoNear][split] = tVec2f(n * zRange_ES, n * zRange_ES + zRange_ES);

	return zRange_LS * n;
}

