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

#ifndef __CDIRECTX9_H__
#define __CDIRECTX9_H__

#include <d3d9.h>
#include <d3dx9.h>

#include "..\\IApplication.h"
#include "..\\ResourceManager.h"
#include "Material.h"
//#include "Mesh.h"
#include "..\\rect.h"

#define ONE_PER_255		(1.f/255.f)

#define GetDXApp() ((CDirectX9*)CDirectX9::GetApp())

#define D3DCOLOR_GetB(rgb) ((BYTE)(rgb))
#define D3DCOLOR_GetG(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define D3DCOLOR_GetR(rgb) ((BYTE)((rgb)>>16))
#define D3DCOLOR_GetA(rgb) ((BYTE)((rgb)>>24))

#define DEFAULT_DRAWDIST	600.0f

class CDirectX9 : public IApplication
{
public:
	CDirectX9();
	virtual ~CDirectX9();

	virtual void Release();

	inline LPDIRECT3D9 GetDirect3D() {return m_pD3D;}
	inline LPDIRECT3DDEVICE9 GetDevice() {return m_pD3DDevice;}
	inline LPD3DXSPRITE GetSpriteInterface() {return m_pD3DXSprite;}

	virtual HRESULT Create(int iWidth, int iHeight, int iBPP, bool bWindowed, LPCTSTR strTitle, DWORD dwFlags, bool bVSync=FALSE, bool bZBuffer=TRUE, bool bMaximizeBox=FALSE);

	bool BeginScene(bool bBackBuffer, bool bZBuffer, D3DCOLOR color);
	bool EndScene();
	bool BeginSprite(DWORD dwFlags = 0);
	bool EndSprite();
	UINT BeginShader();
	bool EndShader();

	void SetTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX *pMatrix);

	void SetMaterial(CMaterial &material);
	void SetTexture(CTexture *pTex);

	void SetViewport(RECT &r);
	RECT GetViewport();

	void Draw2DImage(CTexture *tex, D3DXVECTOR2 pos, D3DCOLOR modColor = 0xFFFFFFFF);

	void SetDefaultRenderStates();

	static char GetFormatBPP(D3DFORMAT format);

	void SetAmbientLight(D3DCOLOR color);

	bool EnableAlphaTesting(bool bValue);
	bool EnableZBuffer(bool bValue);
	bool EnableLighting(bool bValue);

	void TakeScreenshot(const char *filename);

	static D3DLIGHT9 SetupDirectionalLight(float r, float g, float b, D3DXVECTOR3 dir);
	static D3DLIGHT9 SetupDirectionalLight(D3DCOLOR color, D3DXVECTOR3 dir);

	virtual HRESULT OnCreate() {return S_OK;}
	virtual void OnRelease() {}
	virtual void OnFlip() {}
	virtual bool OnRestore();
	virtual void OnDeviceLost();

	virtual int GetWidth() {return m_iWidth;}
	virtual int GetHeight() {return m_iHeight;}

	static bool IsUsingShader() {return m_bUsingShader;}
	ID3DXEffect *GetEffect() {return m_pEffect;}

	D3DCOLOR GetCurrentAmbientColor() {return m_ambientColor;}

	void RenderWireframeAABB(aabbox3d &aabb, bool doubleFrame, float dist, bool disableZBuffer, D3DXCOLOR color);

protected:
	bool RestoreDevice();
	bool LoadEffect(const char *effectFilename);

	void RenderWireframeAABB(aabbox3d &aabb);

	bool CheckFormat(D3DFORMAT format, bool bWindowed, bool bHAL);
	void SaveScreenToFile(const char *filename);

	IDirect3D9			*m_pD3D;
	IDirect3DDevice9	*m_pD3DDevice;

	ID3DXSprite			*m_pD3DXSprite;		// D3DX sprite interface

	ID3DXEffect			*m_pEffect;
	LPDIRECT3DVERTEXDECLARATION9    m_pVertDecl;

	D3DCOLOR m_ambientColor;

	D3DPRESENT_PARAMETERS m_d3dpp;			// Presentation parameters, stored in case the device has to be reset

	bool m_bHAL;	// Hardware acceleration
	bool m_bZBuffer; // Use Z-buffering
	bool m_bDeviceLost;

	static bool m_bUsingShader;

	int m_iWidth, m_iHeight;
};

#endif // #ifndef __CDIRECTX9_H__
