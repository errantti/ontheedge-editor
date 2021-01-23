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

#ifndef __CFONT_H__
#define __CFONT_H__

#include "DirectX9.h"
#include "IDirectXResource.h"
#include "..\\rect.h"

class CFont : public IDirectXResource
{
public:
	CFont();
	virtual ~CFont();

	bool Create(const char *name, int size, LPDIRECT3DDEVICE9 d3dd, bool bold = FALSE, bool italic = FALSE, bool strikeout = FALSE, bool underline = FALSE);
	void Release();
	bool Print(const char *text, rect<int> &r, D3DCOLOR color = D3DCOLOR_RGBA(255,255,255,255), DWORD format = 0);

	virtual void OnDeviceLost();
	virtual void OnDeviceReset();

private:
	ID3DXFont *m_Font;
};

#endif // #ifndef __CFONT_H__
