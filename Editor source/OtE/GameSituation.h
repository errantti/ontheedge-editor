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

#ifndef __CGAMESITUATION_H__
#define __CGAMESITUATION_H__

#include "Items\\IInventoryItem.h"

#include <vector>

#define GAME_SITUATION_FILE_BEGIN_IDENTIFIER	0x94BF2CAD

#define MAX_MISSION_OBJECTIVES		4

struct SGlobalVariable
{
	float num;
	char name[16];
};

class CGameSituation
{
public:
	CGameSituation();
	~CGameSituation();

	bool StoreCurrentSituation();
	bool SaveStoredSituation(const char *filename);

	bool LoadSituation(const char *filename);

	bool ApplyGadgetsAndItems();

	inline std::vector<SItemTemp> *GetItems() {return &m_items;}
	inline int GetCurrentBlockID() {return m_currentBlockID;}
	inline long GetEquippedGadget(bool slot) {return m_equippedGadgets[(int)slot];}

	void SetEquippedGadget(long ID, bool slot);
	void CopyItems(std::vector<SItemTemp> *items);
	void CopyGadgets(std::vector<long> *gadgets);
	inline void SetShipID(long id) {m_shipID = id;}
	inline void SetCurrentBlockID(int id) {m_currentBlockID = id;}
	inline void SetPlayerStartPosID(long id) {m_startPosID = id;}
	inline void SignalSituationStored() {m_bSituationStored = true;}

	inline long GetShipID() {return m_shipID;}
	inline long GetPlayerStartPosID() {return m_startPosID;}

	const char *GetCampaignFilename() {return m_campaign;}
	void SetCampaignFilename(const char *filename);

	const char *GetSaveName() {return m_name;}
	void SetSaveName(const char *name);

	void SetMissionObjective(int index, const char *objective);
	void GetMissionObjective(int index, char *outObjective);

	void GetGadgetHotkeyIDs(long *outKeys);
	void SetGadgetHotkeyIDs(long *keys);

	inline std::vector<SGlobalVariable> &GetGlobalVariables() {return m_globalVariables;}

	static bool GetSaveNameAndVerify(const char *filename, string &outSaveName);

private:
	char m_campaign[128];
	char m_name[64];
	int m_currentBlockID;
	long m_startPosID;
	std::vector<SItemTemp> m_items;
	long m_equippedGadgets[2];
	long m_shipID;

	bool m_bSituationStored;

	char m_missionObjectives[MAX_MISSION_OBJECTIVES][256];

	// NUM_GADGET_HOTKEYS - 
	long m_gadgetHotkeyIDs[5];

	std::vector<SGlobalVariable> m_globalVariables;
};

#endif // #ifndef __CGAMESITUATION_H__
