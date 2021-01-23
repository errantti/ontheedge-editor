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

#ifndef __CGUILABEL_H__
#define __CGUILABEL_H__

#include "IGUIElement.h"
#include "..\\DirectX9\\Font.h"
#include <d3d9.h>

class CGUILabel : public IGUIElement  
{
public:
	CGUILabel(IGUIEnvironment *env, IGUIElement *parent, rect<int> position, long id, string text);
	virtual ~CGUILabel();

	virtual bool OnEvent(SEvent event);

	virtual void SetText(char *text) {m_text = text;}
	virtual void SetText(string text) {m_text = text;}

	virtual void SetFont(CFont *font) {if(font) m_font = font;}
	virtual CFont *GetFont() {return m_font;}

	void SetTextAlign(DWORD align) {m_textAlign = align;}
	DWORD GetTextAlign() {return m_textAlign;}

protected:
	string m_text;
	D3DCOLOR m_textColor;
	DWORD m_textAlign;
	CFont *m_font;

	virtual void RenderElement();
};

#endif // #ifndef __CGUILABEL_H__
