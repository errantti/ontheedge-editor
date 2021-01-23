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

#include "InventorySubView.h"
#include "InventoryMenu.h"
#include "..\\OnTheEdge.h"

CInventorySubView *CInventorySubView::m_focusView = NULL;
CInventorySubView *CInventorySubView::m_defaultView = NULL;

CInventorySubView::CInventorySubView(MENU_ID menuId, IMenuManager *mgr, int rows, int columns, CInventoryMenu *invMenu, bool allowHotkeying)
: IMenuHandler(menuId, mgr), m_itemRows(rows), m_itemColumns(columns), m_parentHandler(invMenu), m_items(NULL),
	m_bHasFocus(false), m_bPositionUpdated(false), m_equippedIndex(-1), m_bEnter(false), m_bAllowHotkeying(allowHotkeying)
{
	m_bRequiresUpdate = TRUE;

	m_font = CResourceManager::GetManager().GetFont("DejaVuSansMono12", "DejaVuSansMono", 12, false, false);

	if(m_defaultView == NULL)
		m_defaultView = this;
}

CInventorySubView::~CInventorySubView()
{
	if(m_defaultView == this)
		m_defaultView = NULL;
	if(m_focusView == this)
		m_focusView = NULL;
}

bool CInventorySubView::OnEvent(SEvent event)
{
	IGUIElement *e = (IGUIElement*)event.guiEvent.element;
	switch(event.type)
	{
	case EW_GUI_EVENT:
		switch(event.guiEvent.type)
		{
		case EWGUI_ELEMENT_CREATED:
			m_window = (CGUIWindow*)event.guiEvent.element;
			InitializeMenu();
			return true;
		case EWGUI_ELEMENT_DESTROYED:
			OnClose();
			return true;

		case EWGUI_ELEMENT_HOVERLOST:
			if(e->GetID() >= INVBOX_GADGET0 && e->GetID() < INVBOX_GADGET0 + m_itemRows * m_itemColumns)
			{
				//LostItemHover(e->GetID() - INVBOX_GADGET0);
				return true;
			}
			return false;

		case EWGUI_ELEMENT_HOVERED:
			if(e->GetID() >= INVBOX_GADGET0 && e->GetID() < INVBOX_GADGET0 + m_itemRows * m_itemColumns)
			{
				m_parentHandler->PlayHoverSound();
				HoverItem(e->GetID() - INVBOX_GADGET0);
				return true;
			}
			return false;

		case EWGUI_ELEMENT_PRESSED:
			if(e->GetID() >= INVBOX_GADGET0 && e->GetID() < INVBOX_GADGET0 + m_itemRows * m_itemColumns)
			{
				SelectItem(e->GetID() - INVBOX_GADGET0);
				return true;
			}
			break;
		}
		break;
	}

	return false;
}

bool CInventorySubView::InitializeMenu()
{
	CGUIEnvironment *env = CGUIEnvironment::GetGUIEnvironment();
	CGUIWindow *win = m_window;
	if(win == NULL)
		return false;

	//CResourceManager &r = CResourceManager::GetManager();
	//r.SetTextureColorkey(D3DCOLOR_RGBA(0,0,0,255));

	GetOTE()->GetMenuDatabase()->ApplyMenuData(win, m_ID, this);

	CGUIButton *b;

	// Manually create the item controls
	int x,y;
	int xCoord, yCoord = 39;
	for(y = 0; y < m_itemColumns; ++y)
	{
		xCoord = 12;
		for(x = 0; x < m_itemRows; ++x)
		{
			b = (CGUIButton*)env->AddButton(win, rect<int>(xCoord, xCoord+42, yCoord, yCoord+42), INVBOX_GADGET0 + y * m_itemColumns + x);
			if(b)
				b->SetVisible(false);
			xCoord += 45;
		}
		yCoord += 45;
	}

	for(int i=0; i<5; ++i)
	{
		IGUIElement *e = env->AddLabel(win, rect<int>(0, 15, 0, 15), INVBOX_HOTKEY1 + i, "1");
		if(e)
		{
			e->SetVisible(false);
			e->SetEnabled(false);

			// TODO: replace with more elegant solution or change to get the key string instead
			std::string text;
			CGameSettings &s = *CGameSettings::GetSettingsObject();
			BYTE *keys = s.GetGadgetShortcuts();
			text = s.GetKeyString(keys[i]);

			e->SetCaption(text);
		}
	}

	return true;
}

void CInventorySubView::OnClose()
{
}

void CInventorySubView::Update()
{
	if(m_window == NULL)
		return;

	CGameSettings *s = CGameSettings::GetSettingsObject();

	/**
	 * Always update the key states regardless whether
	 * this view is active so the users don't experience
	 * double jumps when changing box selection
	 **/
	if(!s->IsNavigateLeftDown())
		m_bLeft = false;
	if(!s->IsNavigateRightDown())
		m_bRight = false;
	if(!s->IsNavigateUpDown())
		m_bUp = false;
	if(!s->IsNavigateDownDown())
		m_bDown = false;

	bool up=false, down=false, right=false, left=false;
	if(s->IsNavigateLeftDown() && !m_bLeft)
	{
		m_bLeft = true;
		left = true;
	}
	if(s->IsNavigateRightDown() && !m_bRight)
	{
		m_bRight = true;
		right = true;
	}
	if(s->IsNavigateUpDown() && !m_bUp)
	{
		m_bUp = true;
		up = true;
	}
	if(s->IsNavigateDownDown() && !m_bDown)
	{
		m_bDown = true;
		down = true;
	}

	if(m_focusView == this)
	{
		bool enter = false;
		if(!s->IsGeneralOkDown())
			m_bEnter = false;
		if(s->IsGeneralOkDown() && !m_bEnter)
		{
			enter = true;
			m_bEnter = true;
		}
		if(enter)
		{
			SelectItem(m_selectedIndex);
			return;
		}

		if(m_bPositionUpdated)
		{
			m_bPositionUpdated = false;
			return;
		}

		int column = m_selectedIndex % m_itemColumns;
		int row = (int)(m_selectedIndex / m_itemColumns);
		DIRECTION dir;
		int i = -1;
		int index;
		bool viewChange = false;

		if(up)
		{
			--row;
			dir = DIRECTION_UP;
			i = column;
			if(row < 0)
			{
				viewChange = true;
			}
		}
		else if(down)
		{
			++row;
			index = row * m_itemColumns + column;
			dir = DIRECTION_DOWN;
			i = column;
			if(row >= m_itemRows || index >= (int)m_items->size())
			{
				viewChange = true;
			}
		}
		else if(left)
		{
			--column;
			dir = DIRECTION_LEFT;
			i = row;
			if(column < 0)
			{
				viewChange = true;
			}
		}
		else if(right)
		{
			++column;
			index = row * m_itemColumns + column;
			dir = DIRECTION_RIGHT;
			i = row;
			if(column >= m_itemColumns || index >= (int)m_items->size())
			{
				viewChange = true;
			}
		}
		else
		{
		}

		if(!viewChange)
		{
			//LostItemHover(m_selectedIndex);
			if(HoverItem(row * m_itemColumns + column))
			{
				m_bPositionUpdated = false;
				return;
			}
			viewChange = true;
		}
		else
		{
			//LostItemHover(m_selectedIndex);

			if(!m_parentHandler->NavigateSelection(this, dir, i))
			{
				// Restore previously hovered item if nagivation didn't work
				HoverItem(m_selectedIndex);
			}
		}
	}
	else if(m_focusView == NULL && m_defaultView == this)
	{
		if(up || down || left || right)
		{
			if(m_items->size() > 0)
				HoverItem(0);
			else
				m_parentHandler->NavigateSelection(this, DIRECTION_RIGHT, 0);
		}
	}

	m_bPositionUpdated = false;
}

bool CInventorySubView::KeyboardSelectItem(DIRECTION dir, int rowCol)
{
	if(m_items->size() == 0)
		return false;
	int row, column;
	if(dir == DIRECTION_UP)
	{
		row = m_itemRows-1;
		column = rowCol;
	}
	else if(dir == DIRECTION_DOWN)
	{
		row = 0;
		column = rowCol;
	}
	else if(dir == DIRECTION_LEFT)
	{
		column = m_itemColumns-1;
		row = rowCol;
	}
	else if(dir == DIRECTION_RIGHT)
	{
		column = 0;
		row = rowCol;
	}

	if(column >= m_itemColumns)
		column = m_itemColumns - 1;
	if(row >= m_itemRows)
		row = m_itemRows - 1;

	if(column >= (int)m_items->size())
	{
		HoverItem(m_items->size()-1);
		return true;
	}

	while(!HoverItem(row * m_itemColumns + column))
	{
		--row;
		if(row < 0)
			return false;
	}

	return true;
}

void CInventorySubView::UpdateSubView(std::vector<IInventoryItem*> &items)
{
	if(m_window == NULL)
		return;

	m_items = &items;
	CGUIButton *b;

	std::vector<IInventoryItem*>::iterator it = items.begin();
	int numVisibleItems = m_itemRows * m_itemColumns;

	long hotIDs[5];
	GetOTE()->GetCampaign()->GetGadgetHotkeyIDs(hotIDs);

	for(int i=0; i<numVisibleItems && it != items.end(); ++it, ++i)
	{
		b = (CGUIButton*)m_window->GetChild(INVBOX_GADGET0 + i);
		if(b == NULL)
			continue;
		b->SetVisible(true);
		b->SetImage((*it)->GetIcon());

		for(int i2=0; i2<5; ++i2)
			if((*it)->GetID() == hotIDs[i2])
			{
				//
				SetGadgetHotkey(i2, i);
			}
	}
}

bool CInventorySubView::HoverItem(int index)
{
	if(m_items == NULL || m_parentHandler == NULL)
		return false;
	if(index < 0 || index >= (int)m_items->size())
		return false;

	if(m_focusView != NULL)
		m_focusView->HoverLostCurrent();

	if(m_selectedIndex != index)
		m_parentHandler->PlayHoverSound();

	m_bHasFocus = true;
	m_focusView = this;
	m_selectedIndex = index;

	m_parentHandler->DisplayItemDescription((*m_items)[index]);

	CGUIButton *b;
	if(index != m_equippedIndex)
	{
		b = (CGUIButton*)m_window->GetChild(INVBOX_GADGET0 + index);
		if(b != NULL)
			b->SetModColor(0xFFBBBBFF);
	}

	m_bPositionUpdated = true;

	return true;
}

void CInventorySubView::LostItemHover(int index)
{
	m_bHasFocus = false;
	if(m_focusView == this)
		m_focusView = NULL;

	if(m_window == NULL)
		return;

	CGUIButton *b;
	if(index != m_equippedIndex)
	{
		b = (CGUIButton*)m_window->GetChild(INVBOX_GADGET0 + index);
		if(b != NULL)
			b->SetModColor(0xFFFFFFFF);
	}
}

void CInventorySubView::HoverLostCurrent()
{
	LostItemHover(m_selectedIndex);
}

void CInventorySubView::SelectItem(int index)
{
	if(m_items == NULL || m_parentHandler == NULL)
		return;
	if(index < 0 || index >= (int)m_items->size())
		return;

	m_parentHandler->PlayActivationSound();

	m_parentHandler->UseItem((*m_items)[index]);
}

bool CInventorySubView::SetEquippedItem(int index)
{
	if(index < -1 || index >= (int)m_items->size())
		return false;

	CGUIButton *b;
	b = (CGUIButton*)m_window->GetChild(INVBOX_GADGET0 + m_equippedIndex);
	if(b)
		b->SetModColor(0xFFFFFFFF);

	m_equippedIndex = index;
	if(index == -1)
	{
		return true;
	}

	b = (CGUIButton*)m_window->GetChild(INVBOX_GADGET0 + index);
	if(b == NULL)
		return false;

	b->SetModColor(0xFFFFBBBB);

	return true;
}

bool CInventorySubView::SetEquippedItem(IInventoryItem *item)
{
	CGUIButton *b;
	b = (CGUIButton*)m_window->GetChild(INVBOX_GADGET0 + m_equippedIndex);
	if(item == NULL)
	{
		if(b)
			b->SetModColor(0xFFFFFFFF);
		m_equippedIndex = -1;
		return true;
	}
	if(m_items == NULL)
		return false;

	std::vector<IInventoryItem*>::iterator it = m_items->begin();
	for(int i=0; it != m_items->end(); ++it, ++i)
		if((*it) == item)
		{
			if(b)
				b->SetModColor(0xFFFFFFFF);
			b = (CGUIButton*)m_window->GetChild(INVBOX_GADGET0 + i);
			if(b == NULL)
				return false;
			b->SetModColor(0xFFFFBBBB);
			m_equippedIndex = i;
			return true;
		}
	return false;
}

void CInventorySubView::RemoveGadgetHotkey(int index)
{
	if(m_window == NULL)
		return;
	IGUIElement *e = m_window->GetChild(INVBOX_HOTKEY1 + index);
	if(e)
		e->SetVisible(false);
}

void CInventorySubView::SetGadgetHotkey(int hotkeyIndex, int gadgetControlIndex)
{
	if(!m_bAllowHotkeying)
		return;

	CGUIButton *b = (CGUIButton*)m_window->GetChild(INVBOX_GADGET0 + gadgetControlIndex);
	IGUIElement *e = m_window->GetChild(INVBOX_HOTKEY1 + hotkeyIndex);
	rect<int> area;
	if(b != NULL && e != NULL)
	{
		e->SetVisible(true);
		area = b->GetElementRect();
		area.left += 25;
		area.top += 25;
		area.bottom = area.top + 15;
		area.right = area.left + 15;
		e->SetElementRect(area);
	}
	else
		return;

	for(int i=0;i<5;++i)
		if(i != hotkeyIndex)
		{
			e = m_window->GetChild(INVBOX_HOTKEY1 + i);
			if(e!=NULL)
			{
				rect<int> r = e->GetElementRect();
				if(r.left == area.left && r.top == area.top)
					e->SetVisible(FALSE);
			}
		}
}

long CInventorySubView::GetCurrentlyHoveredGadgetID()
{
	if(m_focusView == NULL)
		return -1;
	return (*m_focusView->m_items)[m_focusView->m_selectedIndex]->GetID();
}

void CInventorySubView::SetSelectedGadgetHotkey(int index)
{
	if(m_focusView == NULL)
		return;

	m_focusView->SetGadgetHotkey(index, m_focusView->m_selectedIndex);
}

void CInventorySubView::ResetFocus()
{
	m_focusView = NULL;
}