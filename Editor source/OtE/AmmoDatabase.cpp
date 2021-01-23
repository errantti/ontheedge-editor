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

#include "AmmoDatabase.h"
#include "OnTheEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAmmoDatabase::CAmmoDatabase()
{
}

CAmmoDatabase::~CAmmoDatabase()
{
}

void CAmmoDatabase::LoadDefaultAmmos()
{
	SAmmoTemplate t;
	t.FillDefaults();
	t.ID = 1;
	t.particleEffectID = 2;
	strcpy(t.meshFilename, "meshes\\A_bullet.X");
	strcpy(t.name, "Basic ammo");

	AddAmmoTemplate(t);
}

SAmmoTemplate *CAmmoDatabase::GetAmmoTemplate(long id)
{
	if(id < 1)
		return 0;
	vector<SAmmoTemplate>::iterator it = m_ammoTemplates.begin();
	for(;it != m_ammoTemplates.end(); it++)
		if(it->ID == id)
			return &(*it);/**/
	return NULL;
}

void CAmmoDatabase::ConstructReferences()
{
	vector<SAmmoTemplate>::iterator it = m_ammoTemplates.begin();
	for(; it != m_ammoTemplates.end(); ++it)
	{
		it->pEffect = GetOTE()->GetParticleEffects()->GetEffectData(it->particleEffectID);
		it->mesh = CResourceManager::GetManager().GetMesh(it->meshFilename);
	}
}

bool CAmmoDatabase::LoadDatabase(const char *filename)
{
	if(LoadVectorFromBinaryFile<SAmmoTemplate>(m_ammoTemplates, filename, AMMO_FILE_BEGIN_IDENTIFIER) == FALSE)
		return FALSE;
	vector<SAmmoTemplate>::iterator it = m_ammoTemplates.begin();
	for(; it != m_ammoTemplates.end(); ++it)
	{
		it->mesh = NULL;
		it->pEffect = NULL;
		it->controller = NULL;
	}

	return TRUE;
}

bool CAmmoDatabase::SaveDatabase(const char *filename)
{
	return SaveVectorToBinaryFile<SAmmoTemplate>(m_ammoTemplates, filename, AMMO_FILE_BEGIN_IDENTIFIER);
}

void CAmmoDatabase::ClearDatabase()
{
	m_ammoTemplates.clear();
}

bool CAmmoDatabase::ExportTemplate(long id, std::string fullPath)
{
	vector<SAmmoTemplate>::iterator it = m_ammoTemplates.begin();
	for(;it != m_ammoTemplates.end(); it++)
		if(it->ID == id)
		{
			return ExportStructToFile(fullPath, *it, 1040);
		}
	return false;
}

bool CAmmoDatabase::ImportTemplate(long id, std::string fullPath)
{
	SAmmoTemplate t;
	if(!ImportStructFromFile(fullPath, t, 1040))
		return false;

	if(id == -1)
		t.ID = GetNewID();
	else
	{
		t.ID = id;
		std::vector<SAmmoTemplate>::iterator it = m_ammoTemplates.begin();
		for(;it != m_ammoTemplates.end(); it++)
			if(it->ID == id)
			{
				*it = t;
				return true;
			}
	}
	m_ammoTemplates.push_back(t);

	return true;
}

long CAmmoDatabase::GetNewID()
{
	long id = 1;
	std::vector<SAmmoTemplate>::iterator it = m_ammoTemplates.begin();
	for(;it != m_ammoTemplates.end(); it++)
		if(it->ID >= id)
			id = it->ID + 1;

	return id;
}