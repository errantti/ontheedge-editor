// On the Edge Editor
// 
// Copyright � 2004-2021 Jukka Tykkyl�inen
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

#ifndef __CFRUSTUM_H__
#define __CFRUSTUM_H__

#include "..\\aabbox3d.h"
#include <d3dx9.h>

enum FRUSTUM_SIDE {
	FRUSTUM_NEAR=0,
	FRUSTUM_FAR,
	FRUSTUM_LEFT,
	FRUSTUM_RIGHT,
	FRUSTUM_TOP,
	FRUSTUM_BOTTOM
};

#define INTERSECTING	-1
#define OUTSIDE			0
#define INSIDE			1

class CFrustum
{
public:
	CFrustum();
	virtual ~CFrustum();

	void CalculateBoundingBox();

	aabbox3d &GetBoundingBox() {return m_bbox;}
	void Construct(D3DXMATRIX &Matrix);

	inline D3DXVECTOR3 GetFarRightUp();
	inline D3DXVECTOR3 GetFarRightDown();
	inline D3DXVECTOR3 GetFarLeftUp();
	inline D3DXVECTOR3 GetFarLeftDown();

	/**
	 * Returns 0 if the AABB is completely outside the frustum,
	 * 1 if the AABB is completely inside the frustum and -1
	 * if the AABB merely intersects the frustum
	 */
	int DoesIntersectWithAABB(aabbox3d &B);
	bool DoesIntersectWithSphere(D3DXVECTOR3 &pos, float radius);

	D3DXVECTOR3 m_camPos;

protected:
	D3DXPLANE m_planes[6];
	aabbox3d m_bbox;
};

#endif // #ifndef __CFRUSTUM_H__
