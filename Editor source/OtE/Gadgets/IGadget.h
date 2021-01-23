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

#ifndef __IGADGET_H__
#define __IGADGET_H__

#include "..\\Edgewalker\\DirectX9\\Texture.h"
#include "..\\Edgewalker\\EWScript\\Script.h"
#include "..\\Sound\\Sound.h"
#include "..\\Objects\\ObjectTypes.h"
#include "..\\Items\\IInventoryItem.h"

enum GADGET_TYPE {
	// Activateable
	GADGET_TYPE_WEAPON=0,
	GADGET_TYPE_KINETIC,
	GADGET_TYPE_HOVER,
	GADGET_TYPE_CUSTOM,

	// Toggle
	GADGET_TYPE_SHIELD,
	GADGET_TYPE_STEALTH,
	GADGET_TYPE_POSSESS,
	GADGET_TYPE_PULLRAY,

	//
	GADGET_TYPE_COUNT,
	GADGET_TYPE_FORCEDWORD=0xFFFFFFFF
};

#define GADGET_USABLE_MULTIPLAYER	1
#define GADGET_USABLE_SINGLEPLAYER	2
#define GADGET_USABLE_ENEMY			4

#define GADGET_NAME_LENGTH		128
#define GADGET_DESCRIPTION_LENGTH	256

class CVessel;

struct SCustomGadgetTemplate 
{
	virtual void FillDefaults() = 0;
};

struct SGadgetTemplate
{
	long ID;
	GADGET_TYPE gadgetType;

	char name[GADGET_NAME_LENGTH];
	char icon[128];
	char description[GADGET_DESCRIPTION_LENGTH];

	bool bMultiplayer, bActivateable, bOneUse;

	float capacityRequirement;
	long activationSoundID;
	char activationScript[32];

	int useFlags, customBlockSize;

	void FillDefaults()
	{
		ID = -1;
		gadgetType = GADGET_TYPE_HOVER;
		name[0] = '\0';
		icon[0] = '\0';
		description[0] = '\0';
		bMultiplayer = false;
		bActivateable = true;
		capacityRequirement = 0.f;
		activationSoundID = -1;
		activationScript[0] = '\0';
		useFlags = GADGET_USABLE_SINGLEPLAYER;
		customBlockSize = 0;
		bOneUse = false;
	}
};

/**
 * Interface for gadget classes. Provides the very basic functionality
 * and essential pure virtual methods
 */
class IGadget : public IInventoryItem
{
public:
	IGadget();
	virtual ~IGadget() {}

	virtual bool Equip(CVessel *vessel);
	virtual void UnEquip();
	virtual void Update(float frameTime) = 0;

	virtual bool Activate() = 0;

	void ApplyTemplate(SGadgetTemplate &t);

	inline bool IsEquipped() {return ((m_vessel)?true:false);}
	inline bool IsActivateable() {return m_bActivateable;}
	virtual bool IsActive() {return false;}

	inline GADGET_TYPE GetGadgetType() {return m_gadgetType;}

	inline float GetCapacityRequirement() {return m_capacityRequirement;}

	inline bool IsEnabledInMultiplayer() {return m_bEnabledInMultiplayer;}

	virtual INVENTORY_ITEM_TYPE GetItemType() {return INVENTORY_ITEM_GADGET;}

	CVessel *GetVessel() {return m_vessel;}

protected:
	virtual void SetDescription() = 0;

	CVessel *m_vessel;

	bool m_bActivateable;
	bool m_bEnabledInMultiplayer;
	float m_capacityRequirement;

	float *m_energy;

	GADGET_TYPE m_gadgetType;

	int m_useFlags;

	CSound *m_activationSound;
	CScript *m_activationScript;
};

#endif // #ifndef __IGADGET_H__
