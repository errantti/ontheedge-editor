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

#ifndef __CEDITORUTILITIES_H__
#define __CEDITORUTILITIES_H__

#include "OtE\\Edgewalker\\aabbox3d.h"
#include <d3d9.h>

/**
 * Provides utility functions for editor
 */
class CEditorUtilities
{
public:
	static void RenderWireframeAABB(aabbox3d &aabb, bool doubleFrame, float dist, bool disableZBuffer, D3DCOLOR color);

private:
	static void RenderWireframeAABB(aabbox3d &aabb, D3DCOLOR &c, LPDIRECT3DDEVICE9 pd);

	struct SWireframeVertex
	{
		float x,y,z;
		D3DCOLOR color;

		/*SWireframeVertex() {}
		SWireframeVertex(float px, float py, float pz, D3DCOLOR &c) :
			x(px), y(py), z(pz), color(c) 
		{}*/
	};
};

#endif // __CEDITORUTILITIES_H__