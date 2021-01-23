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

#include "DirectX9.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexture::CTexture()
: m_texture(NULL), m_filename("")
{
}

CTexture::~CTexture()
{
	Release();
}

void CTexture::Release()
{
	if(m_texture)
	{
		m_texture->Release();
		m_texture = NULL;
	}
}

bool CTexture::LoadTexture(LPCTSTR filename, D3DCOLOR colorkey)
{
	//IDirect3DDevice9 *p = ((CDirectX9*)CDirectX9::GetApp())->GetDevice();
	IDirect3DDevice9 *p = GetDXApp()->GetDevice();

	//if(FAILED(D3DXCreateTextureFromFile(p, filename, &m_texture)))
	//	return FALSE;

	D3DXIMAGE_INFO info;

	if(FAILED(D3DXCreateTextureFromFileEx(	p, filename, D3DX_DEFAULT, D3DX_DEFAULT,
											1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
											D3DX_DEFAULT, D3DX_DEFAULT, colorkey,
											&info, NULL, &m_texture)))
		return FALSE;

	m_filename = filename;

	m_iWidth = info.Width;
	m_iHeight = info.Height;

	return TRUE;
}