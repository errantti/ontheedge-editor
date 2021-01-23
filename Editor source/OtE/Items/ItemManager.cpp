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

#include "ItemManager.h"
#include "..\\Edgewalker\\Utilities.h"
#include "..\\Edgewalker\\Printer.h"

#include <iostream>
using namespace std;

CItemManager *CItemManager::m_this = 0;

CItemManager::CItemManager()
{
	if(m_this == 0)
		m_this = this;
}

CItemManager::~CItemManager()
{
	if(m_this == this)
		m_this = 0;
}

CShipPatch *CItemManager::CreateShipPatch(long id)
{
	SShipPatchTemplate *t = GetPatchTemplate(id);
	if(t != NULL)
		return new CShipPatch(*t);
	return NULL;
}

CQuestItem *CItemManager::CreateQuestItem(long id)
{
	SQuestItemTemplate *t = GetQuestItemTemplate(id);
	if(t != NULL)
		return new CQuestItem(*t);
	return NULL;
}

CGadgetComponent *CItemManager::CreateGadgetComponent(long id)
{
	SGadgetComponentTemplate *t = GetComponentTemplate(id);
	if(t != NULL)
		return new CGadgetComponent(*t);
	return NULL;
}

SShipPatchTemplate *CItemManager::GetPatchTemplate(long id)
{
	vector<SShipPatchTemplate>::iterator it = m_patchTemplates.begin();
	for(; it != m_patchTemplates.end(); ++it)
		if(it->id == id)
			return &(*it);
	return NULL;
}

SQuestItemTemplate *CItemManager::GetQuestItemTemplate(long id)
{
	vector<SQuestItemTemplate>::iterator it = m_questTemplates.begin();
	for(; it != m_questTemplates.end(); ++it)
		if(it->id == id)
			return &(*it);
	return NULL;
}

SGadgetComponentTemplate *CItemManager::GetComponentTemplate(long id)
{
	vector<SGadgetComponentTemplate>::iterator it = m_componentTemplates.begin();
	for(; it != m_componentTemplates.end(); ++it)
		if(it->id == id)
			return &(*it);
	return NULL;
}

bool CItemManager::LoadDatabase(const char *filename)
{
	fstream f;
	f.open(filename, ios::in | ios::binary);
	if(!f.is_open())
	{
		CPrinter::logError("Couldn't open item database file");
		return false;
	}

	DWORD id = ITEM_FILE_BEGIN_IDENTIFIER;
	try {
		f.read((char*)&id, sizeof(DWORD));
		if(id != ITEM_FILE_BEGIN_IDENTIFIER)
		{
			f.close();
			CPrinter::logError("CItemManager::LoadDatabase: File begin identifier mismatch");
			return false;
		}

		LoadVectorFromBinaryStream<SShipPatchTemplate>(f, m_patchTemplates, 500);
		LoadVectorFromBinaryStream<SQuestItemTemplate>(f, m_questTemplates, 500);
		LoadVectorFromBinaryStream<SGadgetComponentTemplate>(f, m_componentTemplates, 500);
	} catch (exception) {
		return false;
	}

	return true;
}

bool CItemManager::SaveDatabase(const char *filename)
{
	if(filename == NULL)
		return false;

	fstream f;
	f.open(filename, ios::out | ios::binary);
	if(!f.is_open())
	{
		CPrinter::logError("Couldn't open item database file for saving");
		return false;
	}

	DWORD identifier = ITEM_FILE_BEGIN_IDENTIFIER;

	try {
		f.write((const char*)&identifier, sizeof(DWORD));

		SaveVectorToBinaryStream<SShipPatchTemplate>(f, m_patchTemplates);
		SaveVectorToBinaryStream<SQuestItemTemplate>(f, m_questTemplates);
		SaveVectorToBinaryStream<SGadgetComponentTemplate>(f, m_componentTemplates);
	} catch (exception) {
		return false;
	}

	return true;
}

void CItemManager::ClearDatabase()
{
	m_patchTemplates.clear();
	m_questTemplates.clear();
	m_componentTemplates.clear();
}

bool CItemManager::ExportQuestItemTemplate(long id, std::string fullPath)
{
	vector<SQuestItemTemplate>::iterator it = m_questTemplates.begin();
	for(;it != m_questTemplates.end(); it++)
		if(it->id == id)
		{
			return ExportStructToFile(fullPath, *it, 4110);
		}
	return false;
}

bool CItemManager::ImportQuestItemTemplate(long id, std::string fullPath)
{
	SQuestItemTemplate t;
	if(!ImportStructFromFile(fullPath, t, 4110))
		return false;

	if(id == -1)
		t.id = GetNewQuestItemID();
	else
	{
		t.id = id;
		std::vector<SQuestItemTemplate>::iterator it = m_questTemplates.begin();
		for(; it != m_questTemplates.end(); ++it)
			if(it->id == id)
			{
				*it = t;
				return true;
			}
	}

	m_questTemplates.push_back(t);

	return true;
}

bool CItemManager::ExportShipPatchTemplate(long id, std::string fullPath)
{
	vector<SShipPatchTemplate>::iterator it = m_patchTemplates.begin();
	for(;it != m_patchTemplates.end(); it++)
		if(it->id == id)
		{
			return ExportStructToFile(fullPath, *it, 4120);
		}
	return false;
}

bool CItemManager::ImportShipPatchTemplate(long id, std::string fullPath)
{
	SShipPatchTemplate t;
	if(!ImportStructFromFile(fullPath, t, 4120))
		return false;

	if(id == -1)
		t.id = GetNewShipPatchID();
	else
	{
		t.id = id;
		std::vector<SShipPatchTemplate>::iterator it = m_patchTemplates.begin();
		for(; it != m_patchTemplates.end(); ++it)
			if(it->id == id)
			{
				*it = t;
				return true;
			}
	}

	m_patchTemplates.push_back(t);

	return true;
}

bool CItemManager::ExportGadgetComponentTemplate(long id, std::string fullPath)
{
	vector<SGadgetComponentTemplate>::iterator it = m_componentTemplates.begin();
	for(;it != m_componentTemplates.end(); it++)
		if(it->id == id)
		{
			return ExportStructToFile(fullPath, *it, 4130);
		}
	return false;
}

bool CItemManager::ImportGadgetComponentTemplate(long id, std::string fullPath)
{
	SGadgetComponentTemplate t;
	if(!ImportStructFromFile(fullPath, t, 4130))
		return false;

	if(id == -1)
		t.id = GetNewGadgetComponentID();
	else
	{
		t.id = id;
		std::vector<SGadgetComponentTemplate>::iterator it = m_componentTemplates.begin();
		for(; it != m_componentTemplates.end(); ++it)
			if(it->id == id)
			{
				*it = t;
				return true;
			}
	}

	m_componentTemplates.push_back(t);

	return true;
}

long CItemManager::GetNewQuestItemID()
{
	long highestId = -1;
	std::vector<SQuestItemTemplate>::iterator it = m_questTemplates.begin();
	for(; it != m_questTemplates.end(); ++it)
		if(it->id > highestId)
			highestId = it->id;
	return ++highestId;
}

long CItemManager::GetNewShipPatchID()
{
	long highestId = -1;
	std::vector<SShipPatchTemplate>::iterator it = m_patchTemplates.begin();
	for(; it != m_patchTemplates.end(); ++it)
		if(it->id > highestId)
			highestId = it->id;
	return ++highestId;
}

long CItemManager::GetNewGadgetComponentID()
{
	long highestId = -1;
	std::vector<SGadgetComponentTemplate>::iterator it = m_componentTemplates.begin();
	for(; it != m_componentTemplates.end(); ++it)
		if(it->id > highestId)
			highestId = it->id;
	return ++highestId;
}