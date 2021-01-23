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

#ifndef __CINVENTORYSUBVIEW_H__
#define __CINVENTORYSUBVIEW_H__

#include "IMenuHandler.h"
#include "..\\Items\\IInventoryItem.h"

enum INVBOX_ELEMENT
{
	INVBOX_GADGET0 = 8500,
	INVBOX_GADGET1,
	INVBOX_GADGET2,
	INVBOX_GADGET3,
	INVBOX_GADGET4,
	INVBOX_GADGET5,
	INVBOX_GADGET6,
	INVBOX_GADGET7,
	INVBOX_GADGET8,

	INVBOX_HOTKEY1,
	INVBOX_HOTKEY2,
	INVBOX_HOTKEY3,
	INVBOX_HOTKEY4,
	INVBOX_HOTKEY5
};

enum DIRECTION {
	DIRECTION_LEFT=0,
	DIRECTION_RIGHT,
	DIRECTION_UP,
	DIRECTION_DOWN
};

class CInventoryMenu;

class CInventorySubView : public IMenuHandler
{
public:
	CInventorySubView(MENU_ID menuId, IMenuManager *mgr, int rows, int columns, CInventoryMenu *invMenu, bool allowHotkeying = true);
	~CInventorySubView();

	virtual bool InitializeMenu();
	virtual void OnClose();

	virtual bool OnEvent(SEvent event);

	virtual void Update();

	void UpdateSubView(std::vector<IInventoryItem*> &items);

	void MakeDefaultView() {m_defaultView = this;}

	bool KeyboardSelectItem(DIRECTION dir, int rowCol);

	bool SetEquippedItem(int index);
	bool SetEquippedItem(IInventoryItem *item);

	void RemoveGadgetHotkey(int index);

	static void SetSelectedGadgetHotkey(int index);

	static void ResetFocus();

	static long GetCurrentlyHoveredGadgetID();

private:
	void SelectItem(int index);
	bool HoverItem(int index);
	void LostItemHover(int index);
	void HoverLostCurrent();
	void SetGadgetHotkey(int hotkeyIndex, int gadgetControlIndex);

	CFont *m_font;

	CInventoryMenu *m_parentHandler;

	int m_itemRows, m_itemColumns;
	bool m_bHasFocus;
	int m_selectedIndex;

	bool m_bUp, m_bDown, m_bRight, m_bLeft, m_bEnter;
	bool m_bPositionUpdated;
	int m_equippedIndex;

	bool m_bAllowHotkeying;

	std::vector<IInventoryItem*> *m_items;

	static CInventorySubView *m_focusView, *m_defaultView;
};

#endif // #ifndef __CINVENTORYSUBVIEW_H__