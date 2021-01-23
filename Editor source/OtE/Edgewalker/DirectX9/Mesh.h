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

#ifndef __CMESH_H__
#define __CMESH_H__


#include <d3dx9.h>

#include "..\\aabbox3d.h"
#include "..\\bsphere3d.h"
#include "Material.h"
//#include "DirectX9.h"

class CMesh  
{
public:
	CMesh();
	~CMesh();

	void Release();

	bool LoadMesh(const char *filename, IDirect3DDevice9 *pD3DDevice);
	void Render();
	void RenderWithoutTexture();
	void RenderWithCustomTexture(CTexture *tex);

	ID3DXMesh *GetMesh() {return m_mesh;}
	const string &GetFilename() {return m_filename;}
	//const char *GetFilename() {return m_filename.c_str();}

	aabbox3d &GetBoundingBox() {return m_bbox;}
	bsphere3d &GetBoudingSphere() {return m_bsphere;}

	//void SetScalingCoefficient(float coef);

	inline CMaterial &GetMaterial() {return m_material;}
	inline CTexture *GetTexture() {return m_material.m_pTexture;}

	static void SetTextureFolder(string folder) {m_textureFolder = folder;}
	static string GetCurrentTextureFolder() {return m_textureFolder;}

protected:
	//CTexture *GetTexture(char *filename, int limit);

	ID3DXMesh *m_mesh;
	CMaterial m_material;

	DWORD m_dwNumSubsets;	// Number of materials restricted to one
	string m_filename;

	IDirect3DDevice9 *m_pD3DDevice;

	aabbox3d m_bbox;
	bsphere3d m_bsphere;

	//float m_scalingCoefficient;

	void ComputeBoundingVolumes();
	bool ShaderModePreparation(CTexture *tex);

	static string m_textureFolder;
};

#endif // #ifndef __CMESH_H__
