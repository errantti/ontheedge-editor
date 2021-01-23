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

#ifndef __CSHIPPATCH_H__
#define __CSHIPPATCH_H__

#include <string>
using std::string;

#include "IInventoryItem.h"

enum PATCH_TYPE {
	PATCH_TYPE_HEALTH=0,
	PATCH_TYPE_ENERGY,
	PATCH_TYPE_ENERGY_REGENERATION,
	PATCH_TYPE_CARRY_CAPACITY,
	PATCH_TYPE_CONTROL_SYSTEM,
	PATCH_TYPE_SPEED,
	PATCH_TYPE_BOOST_SPEED,
	PATCH_TYPE_TURN_SPEED,

	PATCH_TYPE_FORCEDWORD = 0xFFFFFFFF
};

struct SShipPatchTemplate
{
	long id;
	char name[32];
	char iconTexture[128];
	char description[INVENTORY_ITEM_DESCRIPTION_LENGTH];
	PATCH_TYPE type;
	float params[2];

	void FillDefaults()
	{
		id = -1;
		name[0] = '\0';
		iconTexture[0] = '\0';
		description[0] = '\0';

		type = PATCH_TYPE_HEALTH;
		params[0] = params[1] = 1.f;
	}
};

class CShip;

/**
 * Patch is a small and permanent boost that can be
 * attached to a ship
 */
class CShipPatch : public IInventoryItem
{
public:
	CShipPatch(SShipPatchTemplate &t);

	bool Attach(CShip &ship);
	bool Detach(CShip &ship);

	void ApplyTemplate(SShipPatchTemplate &t);

	virtual INVENTORY_ITEM_TYPE GetItemType() {return INVENTORY_ITEM_PATCH;}

private:
	PATCH_TYPE m_type;
	float m_param1, m_param2;

	bool m_bAttached;
};

#endif // #ifndef __CSHIPPATCH_H__