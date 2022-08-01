#include "stdafx.h"

#include "BoundingSphere.h"

BoundingSphere::CurBoundingSphere::CurBoundingSphere()
{
	Reset();
}

double BoundingSphere::CurBoundingSphere::Excess(const tVec3d& p) const
{
	double e = -cur_sqr;
	e += vmath::dot(p - cur_ctr, p - cur_ctr);
	 
	return e;
}

void BoundingSphere::CurBoundingSphere::Reset()
{
	size = supportSize = 0;

	ctr[0]  = tVec3d(.0, .0, .0);
	cur_ctr = ctr[0];
	cur_sqr = -1.0;
	cur_r = -1.0;
}

bool BoundingSphere::CurBoundingSphere::Push(const tVec3d& p)
{
	double eps = 1e-32;

	if (size == 0)
	{
		q0 = p;
		ctr[0] = q0;
		sqr[0] = 0;
	} 
	else
	{
		// set v_size to Q_size
		v[size] = p - q0;

		// compute the a_{size,i}, i < size
		for (int i = 1; i < size; ++i)
		{
			a[size][i] = 0;
			a[size] += v[i] * v[size];
			a[size][i] *= (2 / z[i]);
		}

		// update v_size to Q_size-\bar{Q}_size
		for (int i = 1; i < size; ++i)
			v[size] -= a[size] * v[i];

		// compute z_size
		z[size] = 0;
		z[size] += vmath::dot(v[size], v[size]); 
		z[size] *= 2;

		// reject push if z_size too small
		if (z[size] < eps * cur_sqr)
			return false;

		// update ctr, sqr_r, r
		double e = -sqr[size-1];
		e += vmath::dot(p - ctr[size-1], p - ctr[size-1]);

		f[size] = e / z[size];

		ctr[size] = ctr[size-1] + f[size] * v[size];
		sqr[size] = sqr[size-1] + e * f[size] / 2;
	}

	cur_ctr = ctr[size];
	cur_sqr = sqr[size];	
	cur_r = sqrt(cur_sqr);
	supportSize = ++size;

	return true;
}

void BoundingSphere::CurBoundingSphere::Pop()
{
	--size;
}

double BoundingSphere::CurBoundingSphere::Slack() const
{
	double l[4], min_l = 0;
	l[0] = 1;

	for (int i = supportSize-1; i > 0; --i)
	{
		l[i] = f[i];
	
		for (int k = supportSize-1; k > i; --k)
			l[i]-=a[k][i]*l[k];
	
		if (l[i] < min_l)
			min_l = l[i];
		
		l[0] -= l[i];
	}
	
	if (l[0] < min_l)
		min_l = l[0];
	
	return ( (min_l < 0) ? -min_l : 0 );

}

BoundingSphere::BoundingSphere()
{
}

double BoundingSphere::GetRadius() const
{
	return _curSphere.cur_r;
}

double BoundingSphere::GetSquaredRadius() const
{
	return _curSphere.cur_sqr;
}

const tVec3d& BoundingSphere::GetCenter() const
{
	return _curSphere.cur_ctr;
}
	
unsigned int BoundingSphere::GetPointCount() const
{
	return static_cast<unsigned int>(_points.size());
}

const BoundingSphere::tPtArray& BoundingSphere::GetPointArray() const
{
	return _points;
}

void BoundingSphere::Add(const tVec3d& pt)
{
	_points.push_back(pt);
}

void BoundingSphere::Expand(const tVec3d& pt)
{
	_points.push_back(pt);
	
	Build();
}

void BoundingSphere::MoveToFrontMBS(tIt i)
{
	support_end = _points.begin();
	if (_curSphere.size == 4)
		return;
	
	for (tIt k = _points.begin(); k != i;)
	{
		tIt j = k++;
		if (_curSphere.Excess(*j) > 0)
		{
			if (_curSphere.Push(*j))
			{
				MoveToFrontMBS(j);
				_curSphere.Pop();
				MoveToFront(j);
			}
		}
	}
}

void BoundingSphere::PivotMBS(tIt i)
{
	tIt t = ++_points.begin();
	MoveToFrontMBS(t);
	double max_e, old_sqr_r = -1;
	
	do
	{
		tIt pivot;
		max_e = MaxExcess(t, i, pivot);
		if (max_e > 0)
		{
			t = support_end;
			if (t == pivot)
				++t;
			old_sqr_r = _curSphere.cur_sqr;
			_curSphere.Push(*pivot);
			MoveToFrontMBS(support_end);
			_curSphere.Pop();
			MoveToFront(pivot);
		}
	}
	while ((max_e > 0) && (_curSphere.cur_sqr > old_sqr_r));

}

void BoundingSphere::MoveToFront(tIt j)
{
	if (support_end == j)
		support_end++;
	
	_points.splice(_points.begin(), _points, j);

}

double BoundingSphere::MaxExcess(tIt t, tIt i, tIt& pivot) const
{
	const tVec3d c = _curSphere.cur_ctr;
	double sqr_r = _curSphere.cur_sqr;
	double e, max_e = 0;
	
	for (tIt k = t; k != i; ++k)
	{
		const tVec3d p = (*k);
		e = -sqr_r;
		e += vmath::dot(p-c, p-c);
		
		if (e > max_e)
		{
			max_e = e;
			pivot = k;
		}
	}
	return max_e;
}

void BoundingSphere::Build()
{
	_curSphere.Reset();
	support_end = _points.begin();
	PivotMBS(_points.end());
}
