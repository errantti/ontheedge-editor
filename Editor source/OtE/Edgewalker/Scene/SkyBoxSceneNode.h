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

#ifndef __CSKYBOXSCENENODE_H__
#define __CSKYBOXSCENENODE_H__

#include "ISceneNode.h"
#include "..\\DirectX9\\Material.h"

class CSkyBoxSceneNode : public ISceneNode  
{
public:
	CSkyBoxSceneNode(ISceneManager *mgr, ISceneNode *parent = 0, DWORD id = -1,
		CTexture *top = NULL, CTexture *bottom = NULL, CTexture *front = NULL,
		CTexture *back = NULL, CTexture *left = NULL, CTexture *right = NULL);
	virtual ~CSkyBoxSceneNode();

	void Render(bool isShadowPass);
	void Release();

	void OnPreRender(bool isShadowPass);

	aabbox3d &GetBoundingBox();

	void SetTextures(	CTexture *top = NULL, CTexture *bottom = NULL, CTexture *front = NULL,
						CTexture *back = NULL, CTexture *left = NULL, CTexture *right = NULL);

protected:
	struct SSkyboxVertex
	{
		float x,y,z;
		float u,v;
	};
	#define SkyboxFVF (D3DFVF_XYZ | D3DFVF_TEX1)

	PTEXTURE m_textures[6];
	CMaterial m_material;

	LPDIRECT3DVERTEXBUFFER9 m_vb;
};

#endif // #ifndef __CSKYBOXSCENENODE_H__
