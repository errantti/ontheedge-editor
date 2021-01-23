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

#include "SaveGameManager.h"
#include "OnTheEdge.h"

using namespace std;

std::string CSaveGameManager::m_coreSaveFolder = "saves\\";

CSaveGameManager::CSaveGameManager() : m_saveFolder("saves\\"), m_latestAutosaveIndex(-1)
{
	ResetSlotList();
}

CSaveGameManager::~CSaveGameManager()
{
}

void CSaveGameManager::SetSaveFolder(std::string folder)
{
	m_saveFolder = folder;
	sprintf_s(m_autosaveFolder, 255, "%sautosaves\\", folder.c_str());
	m_latestAutosaveIndex = -1;
}

void CSaveGameManager::ResetSlotList()
{
	for(int i=0;i<NUM_SAVE_SLOTS;++i)
		m_slotSaves[i] = NULL;
}

void CSaveGameManager::ClearSavedGameList()
{
	ResetSlotList();
	m_saves.clear();
}

bool CSaveGameManager::UpdateSavedGameList()
{
	ClearSavedGameList();

	HANDLE h;
	WIN32_FIND_DATA filedata;

	char szFolder[256];
	sprintf_s(szFolder, 255, "%s*.svg", m_saveFolder.c_str());

	// TODO: Check whether the save folder exists

	h = FindFirstFile((char*)szFolder,&filedata);

	if(h == INVALID_HANDLE_VALUE)
	{
		// No saved games found
		return true;
	}

	do
	{
		SSaveGameInfo info;
		info.filename = m_saveFolder;
		info.filename += filedata.cFileName;

		// read save name from the file (and verify the version at the same time)
		if(CGameSituation::GetSaveNameAndVerify(info.filename.c_str(), info.saveName))
			m_saves.push_back(info);
	} while(FindNextFile(h,&filedata));

	FindClose(h);

	ParseSlotSaves();

	return true;
}

void CSaveGameManager::ClearLevelSavePointList()
{
	m_autosaves.clear();
}

bool CSaveGameManager::UpdateLevelSavePointList()
{
	ClearLevelSavePointList();

	HANDLE h;
	WIN32_FIND_DATA filedata;

	char szFolder[256];
	sprintf_s(szFolder, 255, "%s*.svg", m_autosaveFolder);

	h = FindFirstFile((char*)szFolder,&filedata);

	if(h == INVALID_HANDLE_VALUE)
	{
		// No saved games found
		return true;
	}

	do
	{
		SSaveGameInfo info;
		info.filename = m_autosaveFolder;
		info.filename += filedata.cFileName;

		// read save name from the file (and verify the version at the same time)
		if(CGameSituation::GetSaveNameAndVerify(info.filename.c_str(), info.saveName))
			m_autosaves.push_back(info);
	} while(FindNextFile(h,&filedata));

	FindClose(h);

	return true;
}

void CSaveGameManager::ParseSlotSaves()
{
	vector<SSaveGameInfo>::iterator it;
	string filename;
	char buffer[32];
	for(int i=0; i<NUM_SAVE_SLOTS; ++i)
	{
		sprintf_s(buffer, 31, "savegame%i.svg", i+1);
		filename = buffer;
		for(it = m_saves.begin(); it != m_saves.end(); ++it)
		{
			if(it->filename.find(filename) != string::npos)
			{
				m_slotSaves[i] = &*it;
				break;
			}
		}
	}
}

bool CSaveGameManager::SaveCurrentSituation(const string filename)
{
	if(filename.length() < 2)
	{
		CPrinter::logError("Failed saving game: passed too short filename parameter");
		return false;
	}

	string fullPath(m_saveFolder);
	fullPath += filename;

	if(filename.rfind(".svg") == string::npos)
	{
		fullPath += ".svg";
	}

	// TODO: Check whether the save folder exists and if it does not, create it

	return GetOTE()->GetCampaign()->SaveGame(fullPath.c_str());
}

bool CSaveGameManager::SaveCurrentSituation(int slot)
{
	if(slot < 0 || slot >= NUM_SAVE_SLOTS)
		return false;

	char buffer[16];
	sprintf_s(buffer, 15, "savegame%i.svg", slot+1);
	return SaveCurrentSituation(string(buffer));
}

bool CSaveGameManager::Autosave()
{
	//return SaveCurrentSituation("autosave.svg");

	CCampaign *c = GetOTE()->GetCampaign();

	char buf[256];
	sprintf_s(buf, 255, "%slevel%i.svg", m_autosaveFolder, c->GetCurrentBlockID());
	strcpy_s(m_latestAutosaveFilename, 511, buf);

	return c->SaveGame(buf);
}

bool CSaveGameManager::LevelSavePoint(bool bOverride)
{
	GetOTE()->GetCurrentLevel()->GetFilename();
	return false;
}

bool CSaveGameManager::LoadGame(const string filename)
{
	return GetOTE()->GetCampaign()->LoadGame(filename.c_str());
}

bool CSaveGameManager::LoadGame(int slot)
{
	if(slot < 0 || slot >= NUM_SAVE_SLOTS)
		return false;

	if(m_slotSaves[slot] == NULL)
		return false;

	return LoadGame(m_slotSaves[slot]->filename);
}

bool CSaveGameManager::LoadAutosave()
{
	//if(m_latestAutosaveIndex < 0 || m_latestAutosaveIndex >= (int)m_autosaves.size())
	//	return false;

	return GetOTE()->GetCampaign()->LoadGame(m_latestAutosaveFilename);
}