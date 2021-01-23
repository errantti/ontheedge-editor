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

#ifndef __BSPHERE3D_H__
#define __BSPHERE3D_H__

#include "d3dx9.h"

class bsphere3d
{
public:
	bsphere3d(float r) : center(D3DXVECTOR3(0.f, 0.f, 0.f)), radius(r) {}
	bsphere3d(D3DXVECTOR3 c, float r) : center(c), radius(r) {}
	bsphere3d() : center(D3DXVECTOR3(0.f, 0.f, 0.f)), radius(0.f) {}
	bsphere3d(float cx, float cy, float cz, float r)
		 : center(D3DXVECTOR3(cx, cy, cz)), radius(r) {}
	bsphere3d(bsphere3d &s) : center(s.center), radius(s.radius) {}

	D3DXVECTOR3 center;
	float radius;
};


#endif