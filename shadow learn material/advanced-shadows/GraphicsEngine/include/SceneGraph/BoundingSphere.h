#pragma once

#include <list>

#include "Common/VectorMathTypes.h"
#include "SceneGraphExports.h"

class SCENEGRAPH_API BoundingSphere
{
private:
	struct SCENEGRAPH_API CurBoundingSphere
	{
		CurBoundingSphere();
		    
		double Excess(const tVec3d& p) const;

		void Reset();
		bool Push(const tVec3d& p);
		void Pop();

		double Slack() const;

		int size;
		int supportSize;

		double z[4];
		double f[4];
		double sqr[3];
		double cur_sqr;
		double cur_r;
		   
#pragma warning(push)
#pragma warning(disable : 4251)
		tVec3d q0;
		tVec3d cur_ctr;
		tVec3d v[4];
		tVec3d a[4];
		tVec3d ctr[4];
#pragma warning(pop)
	};

public:
	typedef std::list<tVec3d> tPtArray;
	typedef tPtArray::iterator tIt;
	typedef tPtArray::const_iterator tConstIt;

	BoundingSphere();

	double GetRadius() const;
	double GetSquaredRadius() const;
	const tVec3d& GetCenter() const;
	
	unsigned int GetPointCount() const;
	const tPtArray& GetPointArray() const;

	void Add(const tVec3d& pt);
	void Expand(const tVec3d& pt);
	void Build();

private:
  void MoveToFrontMBS(tIt i);
  void PivotMBS(tIt i);
  void MoveToFront(tIt j);
  double MaxExcess(tIt t, tIt i, tIt& pivot) const;

private:
	CurBoundingSphere _curSphere;
#pragma warning(push)
#pragma warning(disable : 4251)
	tPtArray _points;
	tIt support_end;
#pragma warning(pop)

};


