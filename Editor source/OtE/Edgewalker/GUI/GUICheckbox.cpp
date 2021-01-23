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

#include "GUIEnvironment.h"
#include "GUICheckbox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUICheckbox::CGUICheckbox(IGUIEnvironment *env, IGUIElement *parent, rect<int> position, long id)
: IGUIElement(env, parent, position, id), m_image(NULL), m_imageHover(NULL), m_imagePressed(NULL), m_imagePressedHover(NULL)
{

}

CGUICheckbox::~CGUICheckbox()
{

}


void CGUICheckbox::RenderElement()
{
	CDirectX9 *dx = GetDXApp();

	CTexture *i = NULL;

	if(m_bHovering)
	{
		if(m_imageHover)
			i = m_imageHover;
		else if(m_image)
			i = m_image;
	}
	else
	{
		if(m_image)
			i = m_image;
	}

	if(i)
		dx->Draw2DImage(i, D3DXVECTOR2((float)m_absoluteRect.left, (float)m_absoluteRect.top), m_modColor);

}


bool CGUICheckbox::OnEvent(SEvent event)
{
	switch(event.type)
	{
	case EW_GUI_EVENT:
		switch(event.guiEvent.type)
		{
		case EWGUI_ELEMENT_HOVERED:
			m_bHovering = TRUE;
			break;
		case EWGUI_ELEMENT_HOVERLOST:
			m_bHovering = FALSE;
			break;
		}
		break;
	}

	if(m_externalEventHandler)
		if(m_externalEventHandler->OnEvent(event))
			return true;

	return m_parent->OnEvent(event);
}

void CGUICheckbox::SetImage(CTexture *image)
{
	m_image = image;
}

void CGUICheckbox::SetImageHover(CTexture *image)
{
	m_imageHover = image;
}

void CGUICheckbox::SetImagePressed(CTexture *image)
{
	m_imagePressed = image;
}

void CGUICheckbox::SetImagePressedHover(CTexture *image)
{
	m_imagePressed = image;
}
