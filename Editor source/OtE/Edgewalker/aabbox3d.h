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

#ifndef __AABBOX3D_H__
#define __AABBOX3D_H__

#include "d3dx9.h"

class aabbox3d
{
public:
	D3DXVECTOR3 minCorner, maxCorner;

	aabbox3d() {}

	aabbox3d(float value) : minCorner(D3DXVECTOR3(-value, -value, -value)),
		maxCorner(D3DXVECTOR3(value, value, value))
	{}

	aabbox3d(D3DXVECTOR3 &vec)
		: minCorner(vec), maxCorner(vec)
	{}

	aabbox3d(D3DXVECTOR3 &min, D3DXVECTOR3 &max)
		: minCorner(min), maxCorner(max)
	{}

	void Repair()
	{
		float temp;

		if(minCorner.x > maxCorner.x)
		{
			temp = minCorner.x;
			minCorner.x = maxCorner.x;
			maxCorner.x = temp;
		}
		if(minCorner.y > maxCorner.y)
		{
			temp = minCorner.y;
			minCorner.y = maxCorner.y;
			maxCorner.y = temp;
		}
		if(minCorner.z > maxCorner.z)
		{
			temp = minCorner.z;
			minCorner.z = maxCorner.z;
			maxCorner.z = temp;
		}
	}

	D3DXVECTOR3 GetCenter()
	{
		return D3DXVECTOR3((minCorner.x+maxCorner.x)*0.5f, (minCorner.y+maxCorner.y)*0.5f, (minCorner.z+maxCorner.z)*0.5f);
	}

	/**
	 * /param corners - array of (at least) eight D3DXVECTOR structures
	 * 0 - bottom left near
	 * 1 - top left near
	 * 2 - bottom right near
	 * 3 - top right near
	 * 4 - bottom left far
	 * 5 - top left far
	 * 6 - bottom right far
	 * 7 - top right far
	 **/
	void GetCorners(D3DXVECTOR3 *corners)
	{
		corners[0].x = minCorner.x;
		corners[0].y = minCorner.y;
		corners[0].z = minCorner.z;

		corners[1].x = minCorner.x;
		corners[1].y = maxCorner.y;
		corners[1].z = minCorner.z;

		corners[2].x = maxCorner.x;
		corners[2].y = minCorner.y;
		corners[2].z = minCorner.z;

		corners[3].x = maxCorner.x;
		corners[3].y = maxCorner.y;
		corners[3].z = minCorner.z;

		corners[4].x = minCorner.x;
		corners[4].y = minCorner.y;
		corners[4].z = maxCorner.z;

		corners[5].x = minCorner.x;
		corners[5].y = maxCorner.y;
		corners[5].z = maxCorner.z;

		corners[6].x = maxCorner.x;
		corners[6].y = minCorner.y;
		corners[6].z = maxCorner.z;

		corners[7].x = maxCorner.x;
		corners[7].y = maxCorner.y;
		corners[7].z = maxCorner.z;
	}

	bool IntersectsWithBox(aabbox3d &other)
	{
		if(other.maxCorner.x < minCorner.x) return FALSE;
		if(other.maxCorner.y < minCorner.y) return FALSE;
		if(other.maxCorner.z < minCorner.z) return FALSE;

		if(other.minCorner.x > maxCorner.x) return FALSE;
		if(other.minCorner.y > maxCorner.y) return FALSE;
		if(other.minCorner.z > maxCorner.z) return FALSE;

		return TRUE;
	}

	bool IntersectsWithPoint(D3DXVECTOR3 &point)
	{
		if(point.x < minCorner.x) return FALSE;
		if(point.y < minCorner.y) return FALSE;
		if(point.z < minCorner.z) return FALSE;

		if(point.x > maxCorner.x) return FALSE;
		if(point.y > maxCorner.y) return FALSE;
		if(point.z > maxCorner.z) return FALSE;

		return TRUE;
	}

	void AddInternalPoint(D3DXVECTOR3 &point)
	{
		if(point.x>maxCorner.x) maxCorner.x = point.x;
		if(point.y>maxCorner.y) maxCorner.y = point.y;
		if(point.z>maxCorner.z) maxCorner.z = point.z;

		if(point.x<minCorner.x) minCorner.x = point.x;
		if(point.y<minCorner.y) minCorner.y = point.y;
		if(point.z<minCorner.z) minCorner.z = point.z;
	}

	void AddInternalBox(aabbox3d &box)
	{
		AddInternalPoint(box.minCorner);
		AddInternalPoint(box.maxCorner);
	}

	inline float GetWidth() {return maxCorner.x-minCorner.x;}
	inline float GetHeight() {return maxCorner.y-minCorner.y;}
	inline float GetDepth() {return maxCorner.z-minCorner.z;}
};

#endif // #ifndef __AABBOX3D_H__