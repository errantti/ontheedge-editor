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

#include "InventoryMenu.h"
#include "..\\OnTheEdge.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInventoryMenu::CInventoryMenu(IMenuManager *mgr)
: IMenuHandler(MENU_ID_INVENTORY, mgr), m_modColor(0xBBFFFFFF),
	m_fontDejaVuSansMono12(NULL), m_selectedItem(NULL),
	m_box1(MENU_ID_INVENTORY_BOX1, mgr, 3, 3, this),
	m_box2(MENU_ID_INVENTORY_BOX2, mgr, 3, 3, this),
	m_box3(MENU_ID_INVENTORY_BOX3, mgr, 3, 3, this, false),
	m_box4(MENU_ID_INVENTORY_BOX4, mgr, 3, 3, this, false),
	m_box5(MENU_ID_INVENTORY_BOX5, mgr, 3, 3, this, false)
{
	m_bRequiresUpdate = TRUE;
	//CGUIEnvironment *e = CGUIEnvironment::GetGUIEnvironment();

	m_fontDejaVuSansMono12 = CResourceManager::GetManager().GetFont("DejaVuSansMono12", "DejaVu Sans Mono", 12, false, false);
}

CInventoryMenu::~CInventoryMenu()
{
}

bool CInventoryMenu::OnEvent(SEvent event)
{
	IGUIElement *e = (IGUIElement*)event.guiEvent.element;
	//CGUILabel *i;
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

		case EWGUI_ELEMENT_PRESSED:
			switch(e->GetID())
			{
			case INV_EXIT:
				GetOTE()->UnpauseGame();
				m_manager->CloseMenu();
				return TRUE;
			}
			break;
		}
		break;
	}

	return false;
}

bool CInventoryMenu::InitializeMenu()
{
	CGUIEnvironment *env = CGUIEnvironment::GetGUIEnvironment();
	CGUIWindow *win = m_window;
	if(win == NULL)
		return false;

	win->SetExternalRendering(&m_statusRender);

	CResourceManager &r = CResourceManager::GetManager();
	r.SetTextureColorkey(D3DCOLOR_RGBA(0,0,0,255));

	GetOTE()->GetMenuDatabase()->ApplyMenuData(win, MENU_ID_INVENTORY, this);

	m_openDown = true;

	r.RestoreDefaultColorkey();

	CInventorySubView::ResetFocus();

	// Add box windows as whild controls to inventory view window
	env->AddWindow(win, rect<int>(0, 0, 512, 512), MENU_ID_INVENTORY_BOX1, &m_box1);
	env->AddWindow(win, rect<int>(200, 200, 512, 512), MENU_ID_INVENTORY_BOX2, &m_box2);
	env->AddWindow(win, rect<int>(0, 0, 512, 512), MENU_ID_INVENTORY_BOX3, &m_box3);
	env->AddWindow(win, rect<int>(0, 0, 512, 512), MENU_ID_INVENTORY_BOX4, &m_box4);


	/*************
	 *
	 *************/

	UpdateItemLists();
	UpdateInventoryView();
	UpdateEquippedGadgetSelection();
	env->SetCursorVisibility(true);

	/*CPlayer *p1 = GetOTE()->GetPlayer(0);
	CShip *s = p1->GetShip();
	m_firstGadget = 0;
	if(s == NULL)
		return TRUE;	// Player has died

	m_gadgets = p1->GetGadgetsInInventory();
	UpdateList();
	UpdateShipDetails();
	m_selectedGadget = -1;
	m_firstGadget = 0;
	m_equippedGadgets[0] = m_startGadgets[0] = s->GetEquippedGadget(0);
	m_equippedGadgets[1] = m_startGadgets[1] = s->GetEquippedGadget(1);

	GetOTE()->GetGUIEnvironment()->SetCursorVisibility(TRUE);

	if(p1->IsChangingGadget())
	{
		vector<IGadget*>::iterator it = m_gadgets->begin();
		for(; it != m_gadgets->end(); ++it)
			if((*it)->GetID() == p1->GetNewGadgetID())
			{
				m_equippedGadgets[p1->GetNewGadgetSlot()] = *it;
				break;
			}
	}

	UpdateEquippedGadgets();*/

	PlayMusic();

	return true;
}

void CInventoryMenu::OnClose()
{
	// Closing menu so if changes to equipments were made, update them
	/*CPlayer *p = GetOTE()->GetPlayer(0);
	if(m_startGadgets[0] != m_equippedGadgets[0])
	{
		if(m_equippedGadgets[0])
			p->EquipGadget(m_equippedGadgets[0]->GetID());
		else
			p->UnEquipGadget(0);
	}
	else if(m_startGadgets[1] != m_equippedGadgets[1])
	{
		if(m_equippedGadgets[1])
			p->EquipGadget(m_equippedGadgets[1]->GetID());
		else
			p->UnEquipGadget(1);
	}*/

	GetOTE()->GetGUIEnvironment()->SetCursorVisibility(FALSE);
}


/***
 * Menu functionalities
 ***/

/*void CInventoryMenu::UpdateShipDetails()
{
	string str = "Max load: ", strTemp;
	stringstream s1, s2;
	CShip *s = GetOTE()->GetPlayer(0)->GetShip();
	if(s == NULL)
		return;
	s1 << (int)s->GetMaxLoad();
	s1 >> strTemp;
	str+=strTemp;
	str+="\nLoad: ";
	s2 << (int)s->GetLoad();
	s2 >> strTemp;
	str+=strTemp;

	CGUILabel *l;
	l = (CGUILabel*)m_window->GetChild(INV_SHIPDETAILS);
	if(l != NULL)
		l->SetCaption(str.c_str());
}*/

void CInventoryMenu::UpdateItemLists()
{
	CPlayer *p = GetOTE()->GetPlayer(0);
	std::vector<IGadget*> &g = *(p->GetGadgetsInInventory());
	std::vector<IGadget*>::iterator it = g.begin();

	m_weaponGadgets.clear();
	m_secondaryGadgets.clear();
	GADGET_TYPE type;
	for(; it != g.end(); ++it)
	{
		type = (*it)->GetGadgetType();
		if(type == GADGET_TYPE_WEAPON || type == GADGET_TYPE_KINETIC)
			m_weaponGadgets.push_back(*it);
		else
			m_secondaryGadgets.push_back(*it);
	}

	m_items.clear();
	std::vector<IInventoryItem*> &i = *(p->GetItemsInInventory());
	std::vector<IInventoryItem*>::iterator iti = i.begin();
	for(; iti != i.end(); ++iti)
	{
		if((*iti)->GetItemType() != INVENTORY_ITEM_COMPONENT)
			m_items.push_back(*iti);
	}

	m_patches.clear();
	CShip *ship = p->GetShip();
	if(ship != NULL)
	{
		std::vector<CShipPatch> &sp = *(ship->GetShipPatches());
		std::vector<CShipPatch>::iterator pit = sp.begin();
		for(; pit != sp.end(); ++pit)
			m_patches.push_back(&(*pit));
	}
}

void CInventoryMenu::UpdateInventoryView()
{
	m_box1.UpdateSubView(m_secondaryGadgets);
	m_box2.UpdateSubView(m_weaponGadgets);
	m_box3.UpdateSubView(m_items);
	m_box4.UpdateSubView(m_patches);
}

void CInventoryMenu::UpdateEquippedGadgetSelection()
{
	CPlayer *p = GetOTE()->GetPlayer(0);
	CShip *s = p->GetShip();
	if(s != NULL)
	{
		bool changing1, changing2;
		IGadget *g1 = p->GetShipGadget(0, changing1);
		IGadget *g2 = p->GetShipGadget(1, changing2);

		m_box2.SetEquippedItem(g1);
		m_box1.SetEquippedItem(g2);
	}
	else
		GetOTE()->AddScreenMessage("The gadget requires too much capacity to equip");
}

void CInventoryMenu::DisplayItemDescription(IInventoryItem *item)
{
	if(item == NULL)
		return;

	CGUILabel *l = (CGUILabel*)m_window->GetChild(INV_GADGETDETAILS);
	if(l)
		l->SetCaption(item->GetDescription());
	l = (CGUILabel*)m_window->GetChild(INV_GADGETNAME);
	if(l)
		l->SetCaption(item->GetName());

}

void CInventoryMenu::UseItem(IInventoryItem *item)
{
	if(item == NULL)
		return;

	if(item->GetItemType() != INVENTORY_ITEM_GADGET)
		return;

	// Try to equip the gadget
	IGadget *g = (IGadget*)item;
	CPlayer *p = GetOTE()->GetPlayer(0);

	if(p->EquipGadget(g->GetID()))
	{
		/*if(g->IsActivateable())
			m_box2.SetEquippedItem(item);
		else
			m_box1.SetEquippedItem(item);*/
		UpdateEquippedGadgetSelection();
	}
	// TODO
}

bool CInventoryMenu::NavigateSelection(CInventorySubView *view, DIRECTION dir, int rowCol)
{
	if(view == NULL)
		return false;

	CInventorySubView *grid[2][2] = {	
		{&m_box1, &m_box2},
		{&m_box3, &m_box4}
	};

	PlayHoverSound();

	int y,x;
	for(y=0;y<2;++y)
		for(x=0;x<2;++x)
			if(grid[y][x] == view)
			{
				if(dir == DIRECTION_LEFT)
				{
					--x;
					if(x < 0)
						return false;
				}
				else if(dir == DIRECTION_RIGHT)
				{
					++x;
					if(x>=2)
						return false;
				}
				else if(dir == DIRECTION_UP)
				{
					--y;
					if(y<0)
						return false;
				}
				else if(dir == DIRECTION_DOWN)
				{
					++y;
					if(y>=2)
						return false;
				}
				return grid[y][x]->KeyboardSelectItem(dir, rowCol);
			}
	return false;
}

void CInventoryMenu::Update()
{
	m_box1.Update();
	m_box2.Update();
	m_box3.Update();
	m_box4.Update();
	m_box5.Update();

	CGameSettings *s = CGameSettings::GetSettingsObject();

	BYTE *hotkeys = s->GetGadgetShortcuts();
	CInputKeyboard *k = GetOTE()->GetKeyboard();
	long id;
	if((id = CInventorySubView::GetCurrentlyHoveredGadgetID()) != -1)
	{
		for(int i=0; i<NUM_GADGET_HOTKEYS; ++i)
		{
			if(k->GetButtonDown(hotkeys[i]))
			{
				GetOTE()->GetCampaign()->SetGadgetHotkeyID(i, id);

				m_box1.RemoveGadgetHotkey(i);
				m_box2.RemoveGadgetHotkey(i);
				m_box3.RemoveGadgetHotkey(i);
				m_box4.RemoveGadgetHotkey(i);
				m_box5.RemoveGadgetHotkey(i);
				CInventorySubView::SetSelectedGadgetHotkey(i);
				break;
			}
		}
	}

	//bool openDownNow = GetOTE()->GetKeyboard()->GetButtonDown(s->GetGeneralKeyboardControlSet().openInventory);
	bool openDownNow = s->IsOpenInventoryDown();

	if(!openDownNow)
		m_openDown = false;

	if(s->IsMenuBackDown() || (openDownNow && !m_openDown))
	{
		m_manager->CloseMenu();
		GetOTE()->UnpauseGame();
	}
}

bool CInventoryMenu::OnMenuBackPressed()
{
	return false;
}

void CInventoryMenu::CStatusDisplayRendering::Render()
{
	GetOTE()->GetStatusDisplay()->Render();
}