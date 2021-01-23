// On the Edge Editor
// 
// Copyright © 2004-2021 Jukka Tykkyläinen
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, copy, modify, merge, publish, 
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or 
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "Frustum.h"
#include "..\\Math.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFrustum::CFrustum()
{
}

CFrustum::~CFrustum()
{
}

void CFrustum::CalculateBoundingBox()
{
	aabbox3d box(m_camPos);

	box.AddInternalPoint(GetFarRightUp());
	box.AddInternalPoint(GetFarRightDown());
	box.AddInternalPoint(GetFarLeftUp());
	box.AddInternalPoint(GetFarLeftDown());

	m_bbox = box;
}

void CFrustum::Construct(D3DXMATRIX &Matrix)
{
	// Calculate the planes
	m_planes[0].a = Matrix._14 + Matrix._13; // Near
	m_planes[0].b = Matrix._24 + Matrix._23;
	m_planes[0].c = Matrix._34 + Matrix._33;
	m_planes[0].d = Matrix._44 + Matrix._43;
	D3DXPlaneNormalize(&m_planes[0], &m_planes[0]);

	m_planes[1].a = Matrix._14 - Matrix._13; // Far
	m_planes[1].b = Matrix._24 - Matrix._23;
	m_planes[1].c = Matrix._34 - Matrix._33;
	m_planes[1].d = Matrix._44 - Matrix._43;
	D3DXPlaneNormalize(&m_planes[1], &m_planes[1]);

	m_planes[2].a = Matrix._14 + Matrix._11; // Left
	m_planes[2].b = Matrix._24 + Matrix._21;
	m_planes[2].c = Matrix._34 + Matrix._31;
	m_planes[2].d = Matrix._44 + Matrix._41;
	D3DXPlaneNormalize(&m_planes[2], &m_planes[2]);

	m_planes[3].a = Matrix._14 - Matrix._11; // Right
	m_planes[3].b = Matrix._24 - Matrix._21;
	m_planes[3].c = Matrix._34 - Matrix._31;
	m_planes[3].d = Matrix._44 - Matrix._41;
	D3DXPlaneNormalize(&m_planes[3], &m_planes[3]);

	m_planes[4].a = Matrix._14 - Matrix._12; // Top
	m_planes[4].b = Matrix._24 - Matrix._22;
	m_planes[4].c = Matrix._34 - Matrix._32;
	m_planes[4].d = Matrix._44 - Matrix._42;
	D3DXPlaneNormalize(&m_planes[4], &m_planes[4]);

	m_planes[5].a = Matrix._14 + Matrix._12; // Bottom
	m_planes[5].b = Matrix._24 + Matrix._22;
	m_planes[5].c = Matrix._34 + Matrix._32;
	m_planes[5].d = Matrix._44 + Matrix._42;
	D3DXPlaneNormalize(&m_planes[5], &m_planes[5]);
}

D3DXVECTOR3 CFrustum::GetFarRightUp()
{
	D3DXMATRIX mat;
	mat._11 = m_planes[FRUSTUM_FAR].a;
	mat._21 = m_planes[FRUSTUM_FAR].b;
	mat._31 = m_planes[FRUSTUM_FAR].c;
	mat._41 = 0.0f;

	mat._12 = m_planes[FRUSTUM_RIGHT].a;
	mat._22 = m_planes[FRUSTUM_RIGHT].b;
	mat._32 = m_planes[FRUSTUM_RIGHT].c;
	mat._42 = 0.0f;

	mat._13 = m_planes[FRUSTUM_TOP].a;
	mat._23 = m_planes[FRUSTUM_TOP].b;
	mat._33 = m_planes[FRUSTUM_TOP].c;
	mat._43 = 0.0f;

	mat._14 = mat._24 = mat._34 = 0.0f;
	mat._44 = 1.0f;
	D3DXMatrixInverse(&mat, NULL, &mat);

	D3DXVECTOR3 vec(-m_planes[FRUSTUM_FAR].d, -m_planes[FRUSTUM_RIGHT].d, -m_planes[FRUSTUM_TOP].d);
	return CMath::MatrixVectorMultiply(mat, vec);
}

D3DXVECTOR3 CFrustum::GetFarRightDown()
{
	D3DXMATRIX mat;
	mat._11 = m_planes[FRUSTUM_FAR].a;
	mat._21 = m_planes[FRUSTUM_FAR].b;
	mat._31 = m_planes[FRUSTUM_FAR].c;
	mat._41 = 0.0f;

	mat._12 = m_planes[FRUSTUM_RIGHT].a;
	mat._22 = m_planes[FRUSTUM_RIGHT].b;
	mat._32 = m_planes[FRUSTUM_RIGHT].c;
	mat._42 = 0.0f;

	mat._13 = m_planes[FRUSTUM_BOTTOM].a;
	mat._23 = m_planes[FRUSTUM_BOTTOM].b;
	mat._33 = m_planes[FRUSTUM_BOTTOM].c;
	mat._43 = 0.0f;

	mat._14 = mat._24 = mat._34 = 0.0f;
	mat._44 = 1.0f;
	D3DXMatrixInverse(&mat, NULL, &mat);

	D3DXVECTOR3 vec(-m_planes[FRUSTUM_FAR].d, -m_planes[FRUSTUM_RIGHT].d, -m_planes[FRUSTUM_BOTTOM].d);
	return CMath::MatrixVectorMultiply(mat, vec);
}

D3DXVECTOR3 CFrustum::GetFarLeftUp()
{
	D3DXMATRIX mat;
	mat._11 = m_planes[FRUSTUM_FAR].a;
	mat._21 = m_planes[FRUSTUM_FAR].b;
	mat._31 = m_planes[FRUSTUM_FAR].c;
	mat._41 = 0.0f;

	mat._12 = m_planes[FRUSTUM_LEFT].a;
	mat._22 = m_planes[FRUSTUM_LEFT].b;
	mat._32 = m_planes[FRUSTUM_LEFT].c;
	mat._42 = 0.0f;

	mat._13 = m_planes[FRUSTUM_TOP].a;
	mat._23 = m_planes[FRUSTUM_TOP].b;
	mat._33 = m_planes[FRUSTUM_TOP].c;
	mat._43 = 0.0f;

	mat._14 = mat._24 = mat._34 = 0.0f;
	mat._44 = 1.0f;
	D3DXMatrixInverse(&mat, NULL, &mat);

	D3DXVECTOR3 vec(-m_planes[FRUSTUM_FAR].d, -m_planes[FRUSTUM_LEFT].d, -m_planes[FRUSTUM_TOP].d);
	return CMath::MatrixVectorMultiply(mat, vec);
}

D3DXVECTOR3 CFrustum::GetFarLeftDown()
{
	D3DXMATRIX mat;
	mat._11 = m_planes[FRUSTUM_FAR].a;
	mat._21 = m_planes[FRUSTUM_FAR].b;
	mat._31 = m_planes[FRUSTUM_FAR].c;
	mat._41 = 0.0f;

	mat._12 = m_planes[FRUSTUM_LEFT].a;
	mat._22 = m_planes[FRUSTUM_LEFT].b;
	mat._32 = m_planes[FRUSTUM_LEFT].c;
	mat._42 = 0.0f;

	mat._13 = m_planes[FRUSTUM_BOTTOM].a;
	mat._23 = m_planes[FRUSTUM_BOTTOM].b;
	mat._33 = m_planes[FRUSTUM_BOTTOM].c;
	mat._43 = 0.0f;

	mat._14 = mat._24 = mat._34 = 0.0f;
	mat._44 = 1.0f;
	D3DXMatrixInverse(&mat, NULL, &mat);

	D3DXVECTOR3 vec(-m_planes[FRUSTUM_FAR].d, -m_planes[FRUSTUM_LEFT].d, -m_planes[FRUSTUM_BOTTOM].d);
	return CMath::MatrixVectorMultiply(mat, vec);
}

int CFrustum::DoesIntersectWithAABB(aabbox3d &B)
{
	int result = INSIDE;

	/**
	 * Frustum - AABB intersection algorithm 1
	 **/
	/*D3DXVECTOR3 corners[8];
	B.GetCorners(&corners);
	D3DXVECTOR3 v;

	int k,i;
	for(k=0; k<6; ++k)
	{
		if(
		for(int i=0;i<3;++i)
		{
		}
	}/**/

	/**
	 * Algorithm 2
	 **/

	float m, n;
	D3DXVECTOR3 bm = B.GetCenter();
	D3DXVECTOR3 bd = B.maxCorner - bm;

	for(int i=0; i<6; ++i)
	{
		D3DXPLANE *p = &m_planes[i];
		m = (bm.x * p->a) + (bm.y * p->b) + (bm.z * p->c) + p->d;
		n = (bd.x * fabs(p->a)) + (bd.y * fabs(p->b)) + (bd.z * fabs(p->c));

		if(m + n < 0.f) 
			return OUTSIDE;
		if(m - n < 0.f) 
			result = INTERSECTING;
	}/**/

	return result;
}