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

#include "EditorUtilities.h"
#include "OtE\\OnTheEdge.h"

void CEditorUtilities::RenderWireframeAABB(aabbox3d &aabb, bool doubleFrame, float dist, bool disableZBuffer, D3DCOLOR color)
{
	CDirectX9 *dx = GetDXApp();

	if(disableZBuffer)
		dx->EnableZBuffer(false);
	//dx->EnableLighting(false);
	LPDIRECT3DDEVICE9 pd = dx->GetDevice();
	pd->SetTexture(0, NULL);

	DWORD fvfBack;
	pd->GetFVF(&fvfBack);
	pd->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);
	pd->SetTransform(D3DTS_WORLD, &m);

	D3DMATERIAL9 material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse.r = 255;
	material.Diffuse.g = 255;
	material.Diffuse.b = 255;
	material.Diffuse.a = 255;
	material.Ambient = material.Diffuse;
	pd->SetMaterial(&material);

	if(doubleFrame)
	{
		aabbox3d frame1(D3DXVECTOR3(aabb.minCorner.x + dist, aabb.minCorner.y + dist, aabb.minCorner.z + dist),
			D3DXVECTOR3(aabb.maxCorner.x - dist, aabb.maxCorner.y - dist, aabb.maxCorner.z - dist));

		aabbox3d frame2(D3DXVECTOR3(aabb.minCorner.x - dist, aabb.minCorner.y - dist, aabb.minCorner.z - dist),
			D3DXVECTOR3(aabb.maxCorner.x + dist, aabb.maxCorner.y + dist, aabb.maxCorner.z + dist));

		RenderWireframeAABB(frame1, color, pd);
		RenderWireframeAABB(frame2, color, pd);
	}
	else
		RenderWireframeAABB(aabb, color, pd);

	pd->SetFVF(fvfBack);

	//dx->EnableLighting(true);
	if(disableZBuffer)
		dx->EnableZBuffer(true);
}

void CEditorUtilities::RenderWireframeAABB(aabbox3d &aabb, D3DCOLOR &c, LPDIRECT3DDEVICE9 pd)
{
	/*D3DXVECTOR3 &i = aabb.minCorner;
	D3DXVECTOR3 &m = aabb.maxCorner;

	SWireframeVertex verts[24] = {
		// Top
		{i.x, m.y, i.z, c},
		{i.x, m.y, m.z, c},

		{i.x, m.y, m.z, c},
		{m.x, m.y, m.z, c},

		{m.x, m.y, m.z, c},
		{m.x, m.y, i.z, c},

		{m.x, m.y, i.z, c},
		{i.x, m.y, i.z, c},

		// Bottom
		{i.x, i.y, i.z, c},
		{i.x, i.y, m.z, c},

		{i.x, i.y, m.z, c},
		{m.x, i.y, m.z, c},

		{m.x, i.y, m.z, c},
		{m.x, i.y, i.z, c},

		{m.x, i.y, i.z, c},
		{i.x, i.y, i.z, c},

		// Sides
		{i.x, i.y, i.z, c},
		{i.x, m.y, i.z, c},

		{m.x, i.y, i.z, c},
		{m.x, m.y, i.z, c},

		{i.x, i.y, m.z, c},
		{i.x, m.y, m.z, c},

		{m.x, i.y, m.z, c},
		{m.x, m.y, m.z, c}
	};

	if(FAILED(pd->DrawPrimitiveUP(D3DPT_LINELIST, 12, 
		verts, sizeof(SWireframeVertex))))
		i.x = i.x;/**/

	D3DXVECTOR3 points[8];
	aabb.GetCorners(points);

	WORD indeces[24] = {0,1,1,3,3,2,2,0,
						4,5,5,7,7,6,6,4,
						0,4,1,5,2,6,3,7};

	pd->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 24, 12, 
		indeces, D3DFMT_INDEX16, points, sizeof(D3DXVECTOR3));/**/
}