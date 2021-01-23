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

#ifndef __IINVENTORYITEM_H__
#define __IINVENTORYITEM_H__

#include "..\\Edgewalker\\DirectX9\\Texture.h"
#include <string>

enum INVENTORY_ITEM_TYPE {
	INVENTORY_ITEM_GADGET = 0,
	INVENTORY_ITEM_PATCH,
	INVENTORY_ITEM_COMPONENT,
	INVENTORY_ITEM_QUESTITEM,
	INVENTORY_ITEM_INVISIBLE,
	INVENTORY_ITEM_FORCEDWORD = 0xFFFFFFFF
};

/**
 * A struct which can be easily used to identify a certain
 * inventory item type
 */
struct SItemTemp
{
	INVENTORY_ITEM_TYPE type;
	long id;

	SItemTemp() {}
	SItemTemp(INVENTORY_ITEM_TYPE pType, long pID) : type(pType), id(pID) {}
};

#define INVENTORY_ITEM_DESCRIPTION_LENGTH	256

/**
 * Interface for inventory items (gadgets, patches, components, quest items)
 * Provides the essential properties required to be graphically displayed
 * in inventory view. Inventory item can also be an invisible item used to
 * store global variables for the player
 */
class IInventoryItem
{
public:
	virtual INVENTORY_ITEM_TYPE GetItemType() = 0;

	std::string &GetDescription() {return m_description;}
	std::string &GetName() {return m_name;}

	void SetName(std::string name) {m_name = name;}
	void SetDescription(std::string desc) {m_description = desc;}

	inline CTexture *GetIcon() {return m_iconTexture;}
	inline void SetIcon(CTexture *texture) {m_iconTexture = texture;}

	inline const long GetID() const {return m_ID;}

protected:
	CTexture *m_iconTexture;

	std::string m_name, m_description;

	long m_ID;
};

#endif // #ifndef __IINVENTORYITEM_H__