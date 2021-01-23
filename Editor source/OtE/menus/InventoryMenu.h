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

#ifndef __CINVENTORYMENU_H__
#define __CINVENTORYMENU_H__

#include "IMenuHandler.h"
#include "..\\Gadgets\\IGadget.h"
#include "InventorySubView.h"

enum INV_ELEMENTS
{
	/*INV_LIST1=150,
	INV_LIST2,
	INV_LIST3,
	INV_GADGET1,
	INV_GADGET2,
	INV_EQUIP1,
	INV_EQUIP2,
	INV_UNEQUIP1,
	INV_UNEQUIP2,
	INV_EXIT,
	INV_LISTUP,
	INV_LISTDOWN,
	INV_GDETAILS,
	INV_BACKGROUND,
	INV_SHIPDETAILS,
	INV_GADGETSELECTION,
	INV_GADGETCHANGE*/

	INV_DETAILIMG=150,
	INV_GADGETNAME,
	INV_GADGETDETAILS,
	INV_GADGET1,
	INV_GADGET2,
	INV_SELECTION,
	INV_EXIT,

	INV_GADGETBOX1,
	INV_BOX1CAPTION,
	INV_BOX11 = 160,
	INV_BOX12,
	INV_BOX13,
	INV_BOX14,
	INV_BOX15,
	INV_BOX16,
	INV_BOX17,
	INV_BOX18,
	INV_BOX19,

	INV_GADGETBOX2,
	INV_BOX2CAPTION,
	INV_BOX21 = 180,
	INV_BOX22,
	INV_BOX23,
	INV_BOX24,
	INV_BOX25,
	INV_BOX26,
	INV_BOX27,
	INV_BOX28,
	INV_BOX29,

	INV_PATCHBOX3,
	INV_BOX3CAPTION,
	INV_BOX31 = 200,
	INV_BOX32,
	INV_BOX33,
	INV_BOX34,
	INV_BOX35,
	INV_BOX36,
	INV_BOX37,
	INV_BOX38,
	INV_BOX39,

	INV_ITEMBOX4,
	INV_BOX4CAPTION,
	INV_BOX41 = 220,
	INV_BOX42,
	INV_BOX43,
	INV_BOX44,
	INV_BOX45,
	INV_BOX46,
	INV_BOX47,
	INV_BOX48,
	INV_BOX49,

	INV_BOX5,
	INV_BOX5CAPTION,
	INV_BOX51 = 240,
	INV_BOX52,
	INV_BOX53,
	INV_BOX54,
	INV_BOX55,
	INV_BOX56,
	INV_BOX57,
	INV_BOX58,
	INV_BOX59,
};

class CInventoryMenu : public IMenuHandler  
{
	friend class CInventorySubView;
public:
	CInventoryMenu(IMenuManager *mgr);
	virtual ~CInventoryMenu();

	virtual bool InitializeMenu();
	virtual void OnClose();

	virtual bool OnEvent(SEvent event);

	void SetModColor(D3DCOLOR color) {m_modColor = color;}

	virtual void Update();

protected:
	void DisplayItemDescription(IInventoryItem *item);
	void UseItem(IInventoryItem *item);
	bool NavigateSelection(CInventorySubView *view, DIRECTION dir, int rowCol);

	virtual bool OnMenuBackPressed();

private:
	class CStatusDisplayRendering : public IExternalGUIWindowRendering
	{
	public:
		virtual void Render();
	};

	void UpdateItemLists();
	void UpdateInventoryView();
	void UpdateEquippedGadgetSelection();

	/*void UpdateGadgetSelection();
	void SelectGadget(int listItemIndex);
	void SelectGadgetFromWholeList(int gadgetIndex);
	void UpdateShipDetails();
	void UpdateEquippedGadgets();*/

	//void EquipSelected(bool slot);

	D3DCOLOR m_modColor;
	IGadget* m_startGadgets[2];
	IGadget* m_equippedGadgets[2];
	CFont *m_fontDejaVuSansMono12;

	bool m_bUpArrowDown, m_bDownArrowDown, m_b1Down, m_b2Down;
	bool m_openDown;

	CInventorySubView m_box1, m_box2, m_box3, m_box4, m_box5;

	IInventoryItem *m_selectedItem;

	std::vector<IInventoryItem*> m_weaponGadgets;
	std::vector<IInventoryItem*> m_secondaryGadgets;
	std::vector<IInventoryItem*> m_items;
	std::vector<IInventoryItem*> m_patches;

	CStatusDisplayRendering m_statusRender;
};

#endif // #ifndef __CINVENTORYMENU_H__
