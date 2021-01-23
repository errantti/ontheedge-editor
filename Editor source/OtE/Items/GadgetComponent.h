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

#ifndef __CGADGETCOMPONENT_H__
#define __CGADGETCOMPONENT_H__

#include "IInventoryItem.h"
//#include "..\\Gadgets\\IGadget.h"

struct SGadgetComponentTemplate
{
	long id;
	char name[32];
	char iconTexture[128];
	char description[INVENTORY_ITEM_DESCRIPTION_LENGTH];

	long targetGadgetID, upgradedGadgetID;

	void FillDefaults()
	{
		id = -1;
		name[0] = '\0';
		iconTexture[0] = '\0';
		description[0] = '\0';

		targetGadgetID = upgradedGadgetID = -1;
	}
};

/**
 * Defines a gadget component which can be used to upgrade
 * a specific gadget into another
 */
class CGadgetComponent : public IInventoryItem
{
public:
	CGadgetComponent(SGadgetComponentTemplate &t);

	long GetTargetGadgetID() {return m_targetGadgetID;}
	long GetUpgradedGadgetID() {return m_upgradedGadgetID;}

	void ApplyTemplate(SGadgetComponentTemplate &t);

	virtual INVENTORY_ITEM_TYPE GetItemType() {return INVENTORY_ITEM_COMPONENT;}

protected:
	long m_targetGadgetID, m_upgradedGadgetID;
};

#endif // #ifndef __CGADGETCOMPONENT_H__