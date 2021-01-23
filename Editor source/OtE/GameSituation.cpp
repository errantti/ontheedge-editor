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

#include "GameSituation.h"
#include "OnTheEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameSituation::CGameSituation() : m_bSituationStored(FALSE)
{
	m_name[0] = '\0';
	m_campaign[0] = '\0';

	int i;
	for(i=0;i<MAX_MISSION_OBJECTIVES;++i)
		m_missionObjectives[i][0] = '\0';
	for(i=0;i<5;++i)
		m_gadgetHotkeyIDs[i] = -1;
}

CGameSituation::~CGameSituation()
{
}

bool CGameSituation::StoreCurrentSituation()
{
	CPlayer *p1;
	if((p1 = GetOTE()->GetPlayer(0)) == NULL)
		return FALSE;

	m_items.clear();
	//p1->GetGadgetIDsInInventory(m_gadgets);
	p1->GetAllItemsInInventory(m_items);

	p1->GetEquippedGadgetIDs(m_equippedGadgets[0], m_equippedGadgets[1]);

	CCampaign *campaign = GetOTE()->GetCampaign();

	sprintf_s(m_name, 63, GetOTE()->GetCurrentLevel()->GetName().c_str());

	m_currentBlockID = campaign->GetCurrentBlockID();
	m_startPosID = campaign->GetPlayerStartposID();
	strcpy_s(m_campaign, 127, campaign->GetCampaignFilename().c_str());

	for(int i=0;i<MAX_MISSION_OBJECTIVES;++i)
		SetMissionObjective(i, campaign->GetMissionObjective(i).c_str());

	m_globalVariables.clear();
	m_globalVariables = campaign->GetGlobalVariableList();

	campaign->GetGadgetHotkeyIDs(m_gadgetHotkeyIDs);

	m_bSituationStored = TRUE;

	return TRUE;
}

void CGameSituation::SetCampaignFilename(const char *filename)
{
	strcpy_s(m_campaign, 127, filename);
}

void CGameSituation::SetSaveName(const char *name)
{
	strcpy_s(m_name, 63, name);
}

void CGameSituation::SetMissionObjective(int index, const char *objective)
{
	if(index < 0 || index >= MAX_MISSION_OBJECTIVES)
		return;
	strcpy_s(m_missionObjectives[index], 255, objective);
}

void CGameSituation::GetMissionObjective(int index, char *outObjective)
{
	if(outObjective == NULL)
		return;
	if(index < 0 || index >= MAX_MISSION_OBJECTIVES)
		return;

	strcpy_s(outObjective, 255, m_missionObjectives[index]);
}

bool CGameSituation::SaveStoredSituation(const char *filename)
{
	if(filename == NULL || !m_bSituationStored)
		return FALSE;

	fstream f;
	f.open(filename, ios::out | ios::binary);
	if(!f.is_open())
	{
		CPrinter::log("Failed opening game situation file for writing", LOG_ERROR);
		//::MessageBox(NULL, "Failed opening game situation file for writing", "OtE Editor error!", MB_OK);
		GetOTE()->AddScreenMessage("Failed opening game situation file for writing");
		return FALSE;
	}

	DWORD identifier = GAME_SITUATION_FILE_BEGIN_IDENTIFIER;

	// Write the file begin identifier
	f.write((const char*)&identifier, sizeof(DWORD));

	/***
	 * Write campaign data
	 ***/
	f.write((const char*)m_name, sizeof(char)*64);
	f.write((const char*)m_campaign, sizeof(char)*128);
	f.write((const char*)&m_currentBlockID, sizeof(int));
	f.write((const char*)&m_shipID, sizeof(long));
	f.write((const char*)&m_startPosID, sizeof(long));
	f.write((const char*)&m_equippedGadgets[0], sizeof(long));
	f.write((const char*)&m_equippedGadgets[1], sizeof(long));

	f.write((const char*)m_gadgetHotkeyIDs, sizeof(long)*5);

	for(int i=0; i<MAX_MISSION_OBJECTIVES; ++i)
		f.write((const char*)m_missionObjectives[i], sizeof(char)*256);

	int size = (int)m_items.size();
	f.write((const char*)&size, sizeof(int));

	size = (int)m_globalVariables.size();
	f.write((const char*)&size, sizeof(int));

	SItemTemp itemTemp;
	vector<SItemTemp>::iterator it = m_items.begin();
	for(; it != m_items.end(); ++it)
	{
		itemTemp = (*it);
		f.write((const char*)&itemTemp, sizeof(SItemTemp));
	}

	SGlobalVariable varTemp;
	vector<SGlobalVariable>::iterator vIt = m_globalVariables.begin();
	for(; vIt != m_globalVariables.end(); ++vIt)
	{
		varTemp = *vIt;
		f.write((const char*)&varTemp, sizeof(SGlobalVariable));
	}

	f.close();

	return TRUE;
}

bool CGameSituation::LoadSituation(const char *filename)
{
	if(filename == NULL)
		return FALSE;

	fstream f;
	f.open(filename, ios::in | ios::binary);
	if(!f.is_open())
	{
		CPrinter::log("Failed opening game situation file. File is being used by another program or doesn't exist.", LOG_ERROR);
		GetOTE()->AddScreenMessage("Couldn't load game");
		return FALSE;
	}

	// Read the identifier
	DWORD identifier;
	f.read((char*)&identifier, sizeof(DWORD));

	if(identifier != GAME_SITUATION_FILE_BEGIN_IDENTIFIER)
	{
		CPrinter::log("Game situation file begin identifier mismatch, file may be corrupted", LOG_ERROR);
		f.close();
		return FALSE;
	}

	/***
	 * Read the game data
	 ***/
	f.read((char*)m_name, sizeof(char)*64);
	m_name[63] = '\0';
	f.read((char*)m_campaign, sizeof(char)*128);
	m_campaign[127] = '\0';
	f.read((char*)&m_currentBlockID, sizeof(int));
	f.read((char*)&m_shipID, sizeof(long));
	f.read((char*)&m_startPosID, sizeof(long));
	f.read((char*)&m_equippedGadgets[0], sizeof(long));
	f.read((char*)&m_equippedGadgets[1], sizeof(long));

	f.read((char*)m_gadgetHotkeyIDs, sizeof(long)*5);

	int i;
	for(i=0; i<MAX_MISSION_OBJECTIVES; ++i)
		f.read((char*)m_missionObjectives[i], sizeof(char)*256);

	int itemsSize;
	int varsSize;

	f.read((char*)&itemsSize, sizeof(int));
	if(itemsSize < 0 || itemsSize > 100)
	{
		CPrinter::log("Gadget amount beyond boundaries, file may be corrupted", LOG_ERROR);
		f.close();
		return FALSE;
	}

	f.read((char*)&varsSize, sizeof(int));
	if(varsSize < 0 || varsSize > 1000)
	{
		CPrinter::log("Global variable amount beyond boundaries, file may be corrupted", LOG_ERROR);
		f.close();
		return FALSE;
	}

	m_items.clear();
	SItemTemp iTemp;
	for(i=0; i<itemsSize; ++i)
	{
		f.read((char*)&iTemp, sizeof(SItemTemp));
		m_items.push_back(iTemp);
	}

	m_globalVariables.clear();
	SGlobalVariable varTemp;
	for(i=0; i<varsSize; ++i)
	{
		f.read((char*)&varTemp, sizeof(SGlobalVariable));
		m_globalVariables.push_back(varTemp);
	}

	f.close();
	m_bSituationStored = TRUE;

	return TRUE;
}

bool CGameSituation::GetSaveNameAndVerify(const char *filename, string &outSaveName)
{
	fstream f;
	f.open(filename, ios::in | ios::binary);
	if(!f.is_open())
		return false;

	DWORD identifier;
	f.read((char*)&identifier, sizeof(DWORD));

	if(identifier != GAME_SITUATION_FILE_BEGIN_IDENTIFIER)
	{
		f.close();
		return false;
	}

	char buffer[64];
	f.read((char*)buffer, sizeof(char)*64);

	if(f.peek() == EOF)
	{
		f.close();
		return false;
	}

	buffer[63] = '\0';
	outSaveName = buffer;

	f.close();

	return true;
}

bool CGameSituation::ApplyGadgetsAndItems()
{
	if(!m_bSituationStored)
		return FALSE;

	// Gadgets and items
	CPlayer *p1 = GetOTE()->GetPlayer(0);
	p1->ResetGadgets();
	p1->ClearItemsAndGadgets();
	p1->AddItems(m_items);

	// TODO: remove
	//p1->AddGadget(1);
	p1->InstantEquipGadget(m_equippedGadgets[0], false);
	p1->InstantEquipGadget(m_equippedGadgets[1], false);
	//p1->InstantEquipGadget(1, 0);

	return TRUE;
}

void CGameSituation::CopyItems(vector<SItemTemp> *items)
{
	if(items == NULL)
		return;
	vector<SItemTemp>::iterator it = items->begin();
	m_items.clear();
	for(; it != items->end(); ++it)
		m_items.push_back(*it);
}

void CGameSituation::CopyGadgets(std::vector<long> *gadgets)
{
	if(gadgets == NULL)
		return;
	std::vector<long>::iterator it = gadgets->begin();
	for(; it != gadgets->end(); ++it)
		m_items.push_back(SItemTemp(INVENTORY_ITEM_GADGET, *it));
}

void CGameSituation::GetGadgetHotkeyIDs(long *outKeys)
{
	if(outKeys == NULL)
		return;
	for(int i=0;i<5;++i)
		outKeys[i] = m_gadgetHotkeyIDs[i];
}

void CGameSituation::SetGadgetHotkeyIDs(long *keys)
{
	if(keys == NULL)
		return;
	for(int i=0;i<5;++i)
		m_gadgetHotkeyIDs[i] = keys[i];
}

void CGameSituation::SetEquippedGadget(long ID, bool slot)
{
	// first check whether the gadget exists in the saved situation
	/*bool found = false;

	std::vector<SItemTemp>::iterator it = m_items.begin();
	for (; it != m_items.end(); ++it)
		if ((*it).id == ID)
		{
			found = true;
			break;
		}

	// There's no point in saving equipped gadget that the player doesn't have
	// (gadget might've been upgraded or it may've been one-time use only)
	if (found)*/
		m_equippedGadgets[(int)slot] = ID;
}