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

#ifndef __CMATH_H__
#define __CMATH_H__

#include <d3dx9.h>
#include "aabbox3d.h"

#define _2PI D3DX_PI*2.f
#define _HALF_PI	D3DX_PI*0.5f

class CMath  
{
public:
	CMath();
	virtual ~CMath();

	// Builds with translation, rotation and scaling matrix
	static D3DXMATRIX ConstructTransformationMatrix(D3DXVECTOR3 &pos, D3DXVECTOR3 &rot, D3DXVECTOR3 &scale);
	static D3DXMATRIX ConstructTransformationMatrix(D3DXVECTOR3 &pos, D3DXVECTOR3 &rot, 
													D3DXVECTOR3 &scale, D3DXVECTOR3 &rotCenter);

	// Returns a translation vector extracted from a matrix
	static D3DXVECTOR3 GetTranslationFromMatrix(D3DXMATRIX &mat);

	// Applies a 4x4 matrix to an axis-aligned bounding box
	static void TransformBoundingBox(D3DXMATRIX &mat, aabbox3d &box);

	// Multiply a 3D vector with 4x4 matrix
	static D3DXVECTOR3 MatrixVectorMultiply(D3DXMATRIX &mat, D3DXVECTOR3 &vec);

	static BOOL DoesRayIntersectSphere(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir, D3DXVECTOR3 &center, float radius);
	static BOOL GetRaySphereIntersection(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir, D3DXVECTOR3 &center, float radius, D3DXVECTOR3 &outIntersection, float &outDistance);

	static BOOL DoesRayIntersectAABB(aabbox3d &aabb, D3DXVECTOR3 &pos, D3DXVECTOR3 &ray, float maxDistCoef = 100.f);
	//static BOOL GetRayAABBIntersection(aabbox3d &aabb, D3DXVECTOR3 &pos, D3DXVECTOR3 &ray, D3DXVECTOR3 &outIntersection
};

// fast inline math functions

// fast square root algorithm that computes the square root of the
// sent float within a 5% margin of error
// From Tricks of the 3D Game Programming Gurus by Andre La Mothe
inline float sqrtfast(float f)
{
	float result;
	_asm
	{
		mov eax, f
		sub eax, 0x3f800000
		sar eax, 1
		add eax, 0x3f800000
		mov result, eax
	}
	return result;
}

#define roundftoi(x) (int)(x+0.5f)

#define absf(x) ((x<0.f)?-x:x)

#endif // #ifndef __CMATH_H__
