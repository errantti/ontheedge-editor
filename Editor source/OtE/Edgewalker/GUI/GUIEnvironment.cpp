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
#include "..\\Edgewalker.h"

CGUIEnvironment *CGUIEnvironment::m_this = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUIEnvironment::CGUIEnvironment() : IGUIElement(NULL, NULL, rect<int>(0, 800, 0, 600), 0),
	m_cursorImage(NULL), m_bCursorVisible(FALSE), m_hover(NULL), m_focus(NULL)
{
	m_environment = this;
	if(m_this == NULL)
		m_this = this;
	//m_bEnabled = false;
}

CGUIEnvironment::~CGUIEnvironment()
{
	/*if(m_cursorImage)
	{
		delete m_cursorImage;
		m_cursorImage = NULL;
	}
	RemoveAllElements();*/
	if(m_this == this)
		m_this = NULL;
}

void CGUIEnvironment::SetCursorImage(CTexture *image)
{
	if(image == NULL)
		return;

	m_cursorImage = image;
}

void CGUIEnvironment::SetCursorImage(string filename)
{
	m_cursorImage = CResourceManager::GetManager().GetTexture(filename);
}

/*IGUIElement *CGUIEnvironment::GetElement(POINT pos)
{
	list<IGUIElement*>::iterator it = m_children.begin();

	for(;it!=m_children.end();it++)
	{
		if((*it)->IsPointInsideElement(pos))
			return (*it)->GetActiveElementAt(pos);
	}

	return NULL;
}*/

/*IGUIElement *CGUIEnvironment::GetElementOnTop(POINT pos)
{
	list<IGUIElement*>::iterator it = m_elements.end();
	--it;

	for(;it!=m_elements.begin();--it)
	{
		if((*it)->IsPointInsideElement(pos))
			return (*it);
	}
	if((*it)->IsPointInsideElement(pos))
		return (*it);

	return NULL;
}*/

/*IGUIElement *CGUIEnvironment::GetTopmostActiveElement(POINT pos)
{
	list<IGUIElement*>::iterator it = m_elements.end();
	--it;

	for(;it!=m_elements.begin();--it)
	{
		if((*it)->IsPointInsideElement(pos))
			if((*it)->GetEnabled())
				return (*it);
	}
	if((*it)->IsPointInsideElement(pos))
		return (*it);

	return NULL;
}*/

/*IGUIElement *CGUIEnvironment::GetElement(long id)
{
	list<IGUIElement*>::iterator it = m_children.begin();

	for(;it!=m_children.end();it++)
	{
		if((*it)->GetID() == id)
			return (*it);
	}

	return NULL;
}*/

bool CGUIEnvironment::RemoveChild(IGUIElement *child)
{
	if(child == NULL)
		return true;

	if(m_focus != NULL)
	{
		IGUIElement *parent1 = m_focus->GetParent();
		IGUIElement *parent = parent1;
		while(parent != this && parent != NULL)
		{
			if(parent == child)
			{
				m_focus = NULL;
				break;
			}
			parent = parent->GetParent();
		}
	}

	m_hover = NULL;
	if(m_bCriticalArea)
		m_waitingToBeRemoved.push_back(child);
	else
		return IGUIElement::RemoveChild(child);
	return true;
}

void CGUIEnvironment::RemovePending()
{
	vector<IGUIElement*>::iterator it = m_waitingToBeRemoved.begin();
	list<IGUIElement*>::iterator lit;
	for(; it != m_waitingToBeRemoved.end(); ++it)
	{
		for(lit = m_children.begin(); lit != m_children.end(); ++lit)
			if((*lit) == (*it))
			{
				delete *lit;
				m_children.erase(lit);
				break;
			}
	}
	m_waitingToBeRemoved.clear();
}

void CGUIEnvironment::Update()
{
	//CDirectX9 *dx = GetDXApp();
	CEdgewalker *ew = GetEdgewalkerApp();
	CInputMouse *mouse = ew->GetMouse();
	POINT pos = mouse->GetCursorPosition();
	SEvent event;
	event.type = EW_GUI_EVENT;

	if(m_children.empty())
		m_focus = NULL;

	/*if(m_externalEventHandler)
		if(m_externalEventHandler->GetRequiresConstantUpdate())
			m_externalEventHandler->Update();*/
	list<IGUIElement*>::iterator it = m_children.begin();

	// Don't let root elements try to remove themselves
	// while they're being updated
	m_bCriticalArea = true;
	for(; it != m_children.end(); ++it)
		(*it)->Update();
	m_bCriticalArea = false;

	// If they did try to remove themselves, do it afterwards
	RemovePending();

	if(m_children.size() == 0)
		return;

	IGUIElement *e = GetActiveElementAt(pos);

	if(e != NULL && e != this)
	{
		if(e->GetEnabled() == FALSE)
		{
			SetHoveringElement(NULL);
			if(mouse->GetMouseLeftState() == MOUSE_BUTTON_PRESSED)
				SetFocusElement(NULL);
			return;
		}

		SetHoveringElement(e);

		if(mouse->GetMouseLeftState() == MOUSE_BUTTON_PRESSED)
		{
			event.guiEvent.type = EWGUI_ELEMENT_PRESSED;
			event.guiEvent.x = pos.x - e->GetElementRect().left;
			event.guiEvent.y = pos.y - e->GetElementRect().top;
			event.guiEvent.element = (void*)e;

			SetFocusElement(e);

			if(e->OnEvent(event))
				return;
			/*if(m_externalEventHandler)
				if(m_externalEventHandler->OnEvent(event))
					return;*/
		}
	}
	else
	{
		SetHoveringElement(NULL);
	}
}

void CGUIEnvironment::DrawAll()
{
	CEdgewalker *ew = GetEdgewalkerApp();
	CInputMouse *mouse = ew->GetMouse();

	Draw();

	// Draw cursor
	if(m_bCursorVisible && m_cursorImage)
	{
		ew->Draw2DImage(m_cursorImage, D3DXVECTOR2((float)(mouse->GetX()-16), (float)(mouse->GetY()-16)));
	}
}

void CGUIEnvironment::SetHoveringElement(IGUIElement *element)
{
	if(element == m_hover)
		return;

	SEvent event;
	event.type = EW_GUI_EVENT;

	if(m_hover)
	{
		event.guiEvent.type = EWGUI_ELEMENT_HOVERLOST;
		event.guiEvent.element = (void*)m_hover;
		m_hover->OnEvent(event);
		/*if(m_externalEventHandler)
			if(m_externalEventHandler->OnEvent(event))
				return;*/
	}

	m_hover = element;
	if(element)
	{
		event.guiEvent.type = EWGUI_ELEMENT_HOVERED;
		event.guiEvent.element = (void*)m_hover;
		if(m_hover->OnEvent(event))
			return;
		/*if(m_externalEventHandler)
			if(m_externalEventHandler->OnEvent(event))
				return;*/
	}
}

void CGUIEnvironment::ForceHoverElement(IGUIElement *element)
{
	if(element == m_hover)
		return;

	m_hover = element;
}

void CGUIEnvironment::SetFocusElement(IGUIElement *element)
{
	if(element == m_focus)
		return;
	SEvent event;
	event.type = EW_GUI_EVENT;

	if(m_focus)
	{
		event.guiEvent.type = EWGUI_ELEMENT_FOCUSLOST;
		event.guiEvent.element = (void*)m_focus;
		if(m_focus->OnEvent(event))
			return;
		/*if(m_externalEventHandler)
			if(m_externalEventHandler->OnEvent(event))
				return;*/
	}

	m_focus = element;
	if(element)
	{
		event.guiEvent.type = EWGUI_ELEMENT_FOCUS;
		event.guiEvent.element = (void*)m_focus;
		if(m_focus->OnEvent(event))
			return;
		/*if(m_externalEventHandler)
			if(m_externalEventHandler->OnEvent(event))
				return;*/
	}
}

void CGUIEnvironment::ForceFocusElement(IGUIElement *element)
{
	m_focus = element;
}

bool CGUIEnvironment::LoadDefaultFont()
{
	m_defaultFont = CResourceManager::GetManager().GetFont("DejaVuSansMono14", "DejaVu Sans Mono", 14, false);
	if(m_defaultFont == NULL)
		return false;
	return true;
}

CFont *CGUIEnvironment::GetDefaultFont()
{
	return m_defaultFont;
}

bool CGUIEnvironment::LoadCursorImage(string filename)
{
	CResourceManager &r = CResourceManager::GetManager();
	//r.SetTextureColorkey(D3DCOLOR_RGBA(0,0,0,255));
	r.SetTextureColorkey(0);
	m_cursorImage = CResourceManager::GetManager().GetTexture(filename);
	r.RestoreDefaultColorkey();
	if(m_cursorImage != NULL)
		return true;
	return false;
}


//////////////////////////////////////////////////

IGUIElement *CGUIEnvironment::AddWindow(IGUIElement *parent, rect<int> position, long id, IEventReceiver *handler)
{
	IGUIElement *p = parent;
	if(p == NULL)
		p = this;

	CGUIWindow *element = new CGUIWindow(this, p, position, id, handler);

	return element;
}

IGUIElement *CGUIEnvironment::AddButton(IGUIElement *parent, rect<int> position, long id, string text)
{
	IGUIElement *p = parent;
	if(p == NULL)
		p = this;

	CGUIButton *element = new CGUIButton(this, p, position, id, text);

	return element;
}

IGUIElement *CGUIEnvironment::AddLabel(IGUIElement *parent, rect<int> position, long id, string text)
{
	IGUIElement *p = parent;
	if(p == NULL)
		p = this;

	CGUILabel *element = new CGUILabel(this, p, position, id, text);

	return element;
}

IGUIElement *CGUIEnvironment::AddImage(IGUIElement *parent, rect<int> position, long id, string image)
{
	IGUIElement *p = parent;
	if(p == NULL)
		p = this;

	CGUIImage *element = new CGUIImage(this, p, position, id, CResourceManager::GetManager().GetTexture(image));

	return element;
}

IGUIElement *CGUIEnvironment::AddScrollbar(IGUIElement *parent, rect<int> position, long id)
{
	IGUIElement *p = parent;
	if(p == NULL)
		p = this;

	CGUIScrollbar *element = new CGUIScrollbar(this, p, position, id);

	return element;
}