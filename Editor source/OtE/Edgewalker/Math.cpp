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

// Math.cpp: implementation of the CMath class.
//
//////////////////////////////////////////////////////////////////////

#include "Math.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMath::CMath()
{

}

CMath::~CMath()
{

}


D3DXMATRIX CMath::ConstructTransformationMatrix(D3DXVECTOR3 &pos, D3DXVECTOR3 &rot, D3DXVECTOR3 &scale)
{
	static D3DXMATRIX mat, matTrans, matScale, matRotX, matRotY, matRotZ;
	
	D3DXMatrixScaling(&matScale, scale.x, scale.y, scale.z);
	D3DXMatrixRotationX(&matRotX, rot.x);
	D3DXMatrixRotationY(&matRotY, rot.y);
	D3DXMatrixRotationZ(&matRotZ, rot.z);
	D3DXMatrixTranslation(&matTrans, pos.x, pos.y, pos.z);
	D3DXMatrixIdentity(&mat);

	D3DXMatrixMultiply(&mat, &matScale, &matRotX);
	D3DXMatrixMultiply(&mat, &mat, &matRotY);
	D3DXMatrixMultiply(&mat, &mat, &matRotZ);
	D3DXMatrixMultiply(&mat, &mat, &matTrans);/**/

	/*D3DXMatrixMultiply(&mat, &matTrans, &matScale);
	D3DXMatrixMultiply(&mat, &mat, &matRotX);
	D3DXMatrixMultiply(&mat, &mat, &matRotY);
	D3DXMatrixMultiply(&mat, &mat, &matRotZ);/**/

	return mat;
}

D3DXMATRIX CMath::ConstructTransformationMatrix(D3DXVECTOR3 &pos, D3DXVECTOR3 &rot, 
												D3DXVECTOR3 &scale, D3DXVECTOR3 &rotCenter)
{
	static D3DXMATRIX mat, matTrans, matScale, matRotX, matRotY, matRotZ, matTemp;
	
	D3DXMatrixScaling(&matScale, scale.x, scale.y, scale.z);
	D3DXMatrixRotationX(&matRotX, rot.x);
	D3DXMatrixRotationY(&matRotY, rot.y);
	D3DXMatrixRotationZ(&matRotZ, rot.z);
	D3DXMatrixTranslation(&matTrans, pos.x + rotCenter.x, pos.y+rotCenter.y, pos.z+rotCenter.z);
	D3DXMatrixTranslation(&matTemp, -rotCenter.x, -rotCenter.y, -rotCenter.z);
	D3DXMatrixIdentity(&mat);

	D3DXMatrixMultiply(&mat, &matScale, &matTemp);
	D3DXMatrixMultiply(&mat, &mat, &matRotX);
	D3DXMatrixMultiply(&mat, &mat, &matRotY);
	D3DXMatrixMultiply(&mat, &mat, &matRotZ);
	D3DXMatrixMultiply(&mat, &mat, &matTrans);

	return mat;
}

D3DXVECTOR3 CMath::GetTranslationFromMatrix(D3DXMATRIX &mat)
{
	return D3DXVECTOR3(mat._14, mat._24, mat._34);
}

void CMath::TransformBoundingBox(D3DXMATRIX &mat, aabbox3d &box)
{
	box.minCorner = MatrixVectorMultiply(mat, box.minCorner);
	box.maxCorner = MatrixVectorMultiply(mat, box.maxCorner);
	box.Repair();
}

D3DXVECTOR3 CMath::MatrixVectorMultiply(D3DXMATRIX &mat, D3DXVECTOR3 &vec)
{
	D3DXVECTOR3 res;

	res.x = mat._11 * vec.x + mat._21 * vec.y + mat._31 * vec.z + mat._41;
	res.y = mat._12 * vec.x + mat._22 * vec.y + mat._32 * vec.z + mat._42;
	res.z = mat._13 * vec.x + mat._23 * vec.y + mat._33 * vec.z + mat._43;

	return res;
}

BOOL CMath::DoesRayIntersectSphere(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir, D3DXVECTOR3 &center, float radius)
{
	D3DXVECTOR3 l = center - pos;
	float s = l.x*dir.x + l.y*dir.y + l.z * dir.z;
	float r2 = radius * radius;
	float l2 = l.x*l.x + l.y*l.y + l.z*l.z;
	if(s < 0.f && l2 > r2)
		return FALSE;
	float m2 = l2 - s*s;
	if(m2 > r2)
		return FALSE;
	else
		return TRUE;
}

BOOL CMath::GetRaySphereIntersection(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir, D3DXVECTOR3 &center, float radius, D3DXVECTOR3 &outIntersection, float &outDistance)
{
	D3DXVECTOR3 l = center - pos;
	float s = l.x*dir.x + l.y*dir.y + l.z * dir.z;
	float r2 = radius * radius;
	float l2 = l.x*l.x + l.y*l.y + l.z*l.z;
	if(s < 0.f && l2 > r2)
		return FALSE;
	float m2 = l2 - s*s;
	if(m2 > r2)
		return FALSE;
	float q = sqrtfast(r2-m2);
	float t;
	if(l2>r2)
		t = s - q;
	else
		t = s + q;
	outIntersection = pos + t*dir;
	outDistance = t;
	return TRUE;
}

BOOL CMath::DoesRayIntersectAABB(aabbox3d &aabb, D3DXVECTOR3 &pos, D3DXVECTOR3 &ray, float maxDistCoef)
{
	// Line segment / Box overlap test algorithm implementation
	// Algorithm from Real-time Rendering 2nd ed.
	D3DXVECTOR3 h((aabb.maxCorner.x-aabb.minCorner.x)*0.5f, (aabb.maxCorner.y-aabb.minCorner.y)*0.5f, (aabb.maxCorner.z-aabb.minCorner.z)*0.5f);

	D3DXVECTOR3 c(pos.x - (aabb.maxCorner.x + aabb.minCorner.x)*0.5f, 
		pos.y - (aabb.maxCorner.y + aabb.minCorner.y)*0.5f, pos.z - (aabb.maxCorner.z + aabb.minCorner.z)*0.5f);

	D3DXVECTOR3 w = ray * maxDistCoef;

	float vx = abs(w.x);
	float vy = abs(w.y);
	float vz = abs(w.z);

	if(abs(c.x) > vx + h.x) return FALSE;
	if(abs(c.y) > vy + h.y) return FALSE;
	if(abs(c.z) > vz + h.z) return FALSE;
	if(abs(c.y * w.z - c.z * w.y) > h.y * vz + h.z * vy) return FALSE;
	if(abs(c.x * w.z - c.z * w.x) > h.x * vz + h.z * vx) return FALSE;
	if(abs(c.x * w.y - c.y * w.x) > h.x * vy + h.y * vx) return FALSE;

	return TRUE;
}