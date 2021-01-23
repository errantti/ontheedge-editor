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

#include "Font.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFont::CFont()
: m_Font(NULL)
{
}

CFont::~CFont()
{
	Release();
}

void CFont::Release()
{
	if(m_Font)
	{
		m_Font->Release();
		m_Font = NULL;
	}
}

bool CFont::Create(const char *name, int size, LPDIRECT3DDEVICE9 d3dd, bool bold, bool italic, bool strikeout, bool underline)
{
	if(d3dd == NULL)
		return FALSE;

	/*LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));

	strcpy(lf.lfFaceName, name);
	lf.lfHeight = -size;
	lf.lfWeight = (bold == TRUE) ? 700 : 0;
	lf.lfItalic = italic;
	lf.lfUnderline = underline;
	lf.lfStrikeOut = strikeout;

	D3DXFONT_DESC d;
	ZeroMemory(&d, sizeof(D3DXFONT_DESC));
	d.Height = -size;
	//d.Width = size;
	d.Weight = (bold == TRUE) ? 700 : 0;
	d.Italic = italic;
	d.MipLevels = D3DX_DEFAULT;
	d.MipLevels = 1;
	strcpy(d.FaceName, _T(""));*/

	//if(FAILED(D3DXCreateFont(d3dd, -size, 0, bold ? FW_BOLD : FW_NORMAL, 0, italic, DEFAULT_CHARSET, 
      //                       OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Arial"), &m_Font)))
	if(FAILED(D3DXCreateFont(d3dd, -size, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, 
                             OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, name, &m_Font)))
	{
		CPrinter::log("Failed loading font", LOG_ERROR);
		return FALSE;
	}

	/*if(FAILED(D3DXCreateFontIndirect(d3dd, &d, &m_Font)))
	{
		return FALSE;
	}*/

	return TRUE;
}

bool CFont::Print(const char *text, rect<int> &r, D3DCOLOR color, DWORD dwFlags)
{
	static RECT rect;

	rect.top = r.top;
	rect.bottom = r.bottom;
	rect.left = r.left;
	rect.right = r.right;

	if(m_Font == NULL)
		return FALSE;

	if(FAILED(m_Font->DrawText(GetDXApp()->GetSpriteInterface(), text, -1, &rect, dwFlags, color)))
	//if(FAILED(m_Font->DrawText(NULL, text, -1, &rect, dwFlags, color)))
		return FALSE;

	return TRUE;
}

void CFont::OnDeviceLost()
{
	if(m_Font)
		m_Font->OnLostDevice();
}

void CFont::OnDeviceReset()
{
	if(m_Font)
		m_Font->OnResetDevice();
}