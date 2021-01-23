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

#ifndef __CPLAYER_H__
#define __CPLAYER_H__

#include "..\\Objects\\Ship.h"
#include "..\\Objects\\Enemy.h"
#include "..\\Edgewalker\\Input\\InputKeyboard.h"
#include "..\\Items\\ItemManager.h"

#define GADGET_CHANGE_DELAY		2.f

struct SPlayerControlSet
{
	// Movement
	BYTE forward, left, right, back;
	BYTE thrustLeft, thrustRight;
	// Actions
	BYTE gadget1, gadget2, boost;
	bool keyboard;

	void FillDefaulKeyboard1()
	{
		forward = DIK_UP;
		left = DIK_LEFT;
		right = DIK_RIGHT;
		back = DIK_DOWN;
		thrustLeft = DIK_NUMPAD7;
		thrustRight = DIK_NUMPAD9;
		gadget1 = DIK_L;
		gadget2 = DIK_K;
		boost = DIK_I;
		keyboard = TRUE;
	}

	void FillDefaultKeyboard2()
	{
		forward = DIK_T;
		left = DIK_F;
		right = DIK_H;
		back = DIK_G;
		thrustLeft = DIK_R;
		thrustRight = DIK_Y;
		gadget1 = DIK_S;
		gadget2 = DIK_A;
		boost = DIK_Q;
		keyboard = TRUE;
	}

	void FillDefaultGamepad()
	{
		keyboard = FALSE;
		forward = 0;
		gadget1 = 1;
		gadget2 = 3;
		boost = 2;

		// These are not used
		back = thrustLeft = thrustRight = left = right = 100;
	}

	void FillDefaultKeybMouse()
	{
		keyboard = TRUE;
		forward = DIK_W;
		back = DIK_S;
		thrustLeft = DIK_A;
		thrustRight = DIK_D;
		boost = DIK_LSHIFT;

		// These are not used
		gadget1 = gadget2 = left = right = 255;
	}
};

class CPlayer : public IGameObjectController
{
public:
	CPlayer();
	~CPlayer();

	virtual void Update(IGameObject *object, float frameTime);
	virtual void RegisterCollision(IGameObject *object, IGeometricObject *other);
	virtual void OnDeath(IGameObject *object);

	void SetControlSet(SPlayerControlSet s) {m_controls = s;}

	bool AddGadget(long ID, bool bNotify = FALSE, bool bSkipExtraChecks = FALSE);
	//void EquipNextGadget(int slot);

	IInventoryItem *AddQuestItem(long ID, bool bNotify = FALSE);
	bool AddComponent(long ID, bool bNotify = FALSE);
	bool AddPatch(long ID, bool bNotify = FALSE);

	/// Returns the point at which the viwe of the player is focused. Used in optimizations
	D3DXVECTOR3 GetFocusPoint();

	/// Returns possessed enemy if possessing and damaged another enemy, otherwise player ship
	CVessel *GetPrimaryAggroTarget();
	/// Returns player ship if primary aggro target is other than that
	CVessel *GetSecondaryAggroTarget();

	void SetShip(CShip *ship);
	inline CShip *GetShip() {return m_ship;}

	void RemoveGadgets();
	void Reset();
	void ResetGadgets();
	void RemoveItems();

	void PossessEnemy(CEnemy *target);
	void UnPosses();
	bool IsPossessing() {return m_bPossessing;}
	CVessel *GetPossessedEnemy();

	/// Called when possessed enemy has aggroed another enemy
	void ReportPossessedAggro();

	void GetGadgetIDsInInventory(vector<long> &gadgetIDs);
	void GetEquippedGadgetIDs(long &gadget1, long &gadget2);
	IGadget *GetGadgetInInventory(long gadgetID);
	IGadget *GetEquippedGadget(int slot);
	IGadget *GetFirstGadgetInInventory();
	CGadgetComponent *GetGadgetComponentInInventory(long sourceGadgetID);
	IInventoryItem *GetItemInInventory(INVENTORY_ITEM_TYPE type, long id);
	void RemoveGadget(long gadgetID);

	/// Copies ids and types of all items in inventory (including gadgets)
	void GetAllItemsInInventory(std::vector<SItemTemp> &items);

	/// Adds all items and gadgets in the vector to the player's inventory
	void AddItems(std::vector<SItemTemp> &items);

	void ClearItemsAndGadgets();

	std::vector<IGadget*> *GetGadgetsInInventory() {return &m_gadgets;}
	std::vector<IInventoryItem*> *GetItemsInInventory() {return &m_items;}

	IGadget *GetShipGadget(int slot, bool &outIsBeingEquipped);
	void UnEquipGadget(int slot);
	bool EquipGadget(long ID, bool bNotify = true);
	bool InstantEquipGadget(long ID, bool bNotify = FALSE, bool bCheckLoad = FALSE);
	//bool InstantEquipExternalGadget(IGadget *gadget, bool bNotify = FALSE);

	void StopChangingGadget(int slot);
	void StopChangingGadgets();
	bool IsChangingGadget(int slot) {return m_bChangingGadget[slot];}
	long GetNewGadgetID(int slot) {return m_newGadgetID[slot];}

	float GetGadgetChangeCounter(int slot) {return m_gadgetChangeCounter[slot];}

	virtual bool RequiresInstantiation() {return true;}

	virtual IGameObjectController *Clone() 
	{
		CPlayer *p = new CPlayer(*this);
		return (IGameObjectController*)p;
	}

	void RemoveItem(IInventoryItem *corpse);
private:
	void ForceRemoveGadget(IGadget *corpse);
	bool ReplaceGadget(long sourceID, long upgradedID);
	void AssignKeyShortcut(IGadget *gadget, bool bNotify);
	int CountSecondaryGadgets();

	void NextWeapon();
	void PrevWeapon();

	CShip *m_ship;
	CVessel *m_possessed;

	std::vector<IGadget*> m_gadgets;
	std::vector<IInventoryItem*> m_items;

	SPlayerControlSet m_controls;

	/**
	 * True while the player is possessing ("mind controlling") an enemy
	 * Possessed enemy can't trigger player triggered triggers and can
	 * only interact with repair and energy pods (not teleports and gadgets)
	 */
	bool m_bPossessing;
	/**
	 * Must be true when the player is possessing an enemy by gadget.
	 * False means that the possession is done through scripts and
	 * is not subject to the restrictions when using gadget possess
	 */
	bool m_bPossessingByGadget;

	/**
	 * Possessed enemy doesn't automatically cause other nearby enemies
	 * to aggro unless it has already damaged another enemy. When this
	 * happens the enemy reports aggro and m_bPossessedAggro is set to
	 * true.
	 */
	bool m_bPossessedAggro;

	/// True while a gadget is being equipped
	bool m_bChangingGadget[2];
	/// IDs of the gadgets that are being equipped to respective slots
	long m_newGadgetID[2];
	/// Counters to count how long till the gadgets are finally equipped
	float m_gadgetChangeCounter[2];

	float m_timeFromMouseChange;
	float m_mouseMovementBuffer;
};

#endif // #ifndef __CPLAYER_H__
