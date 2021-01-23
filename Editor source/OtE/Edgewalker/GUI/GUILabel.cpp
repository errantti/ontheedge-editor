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

#include "GUILabel.h"
#include "GUIEnvironment.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUILabel::CGUILabel(IGUIEnvironment *env, IGUIElement *parent, rect<int> position, long id, string text)
: IGUIElement(env, parent, position, id), m_textColor(D3DCOLOR_RGBA(255,255,255,255)), m_font(NULL)
{
	m_caption = text;
	m_textAlign = DT_CENTER | DT_VCENTER | DT_WORDBREAK;
}

CGUILabel::~CGUILabel()
{
}

void CGUILabel::RenderElement()
{
	if(m_font == NULL)
		m_font = ((CGUIEnvironment*)m_environment)->GetDefaultFont();
	if(m_font == NULL)
		return;

	if(m_caption.size() != 0)
	{
		m_font->Print(m_caption.c_str(), m_absoluteRect, m_modColor, m_textAlign);
	}
}


bool CGUILabel::OnEvent(SEvent event)
{
	return FALSE;
}