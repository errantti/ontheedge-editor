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

#include "ParticleEffects.h"
#include "OnTheEdge.h"

CParticleEffects *CParticleEffects::m_this = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParticleEffects::CParticleEffects()
{
	m_this = this;
}

CParticleEffects::~CParticleEffects()
{
	m_this = NULL;
}

void CParticleEffects::LoadDefaultEffects()
{
	sEMITTER_PARAMS e;

	e.FillDefaults();
	e.timeToLive		= 1.8f;

	e.direction			= D3DXVECTOR3(0.f, 0.f, 0.f);
	e.directionSpread	= D3DXVECTOR3(25.f, 25.f, 25.f);

	e.colorStart		= D3DXCOLOR(1.2f, 0.4f, 0.2f, 0.2f);
	e.colorEnd			= D3DXCOLOR(0.8f, 0.3f, -0.2f, -0.1f);

	e.scaleStart		= 8.f;
	e.scaleEnd			= 13.0f;

	strcpy(e.resourceFilename, "textures\\part1.bmp");
	e.emitFrequency		= 0.1f;
	//e.additiveBlend = false;
	e.replaceOldParticles = false;

	e.particleCount		= 175;
	e.maxParticleCount	= 350;

	/*strcpy(e.resourceFilename, "meshes\\Ship.X");
	e.hasMesh = true;
	e.particleCount = 4;
	e.useRotationMatrices = true;
	e.maxRotationSpeed = D3DXVECTOR3(2.5f, 1.2f, 4.7f);
	e.numRotationMatrices = 3;
	e.colorStart		= D3DXCOLOR(0.0f, 0.4f, 1.0f, 1.0f);
	e.colorEnd			= D3DXCOLOR(0.99f, 0.4f, 0.0f, 0.0f);
	//e.emitFrequency = 10.f;/**/

	strcpy(e.name, "Explosion");

	SParticleSystemInfo i;
	i.params.push_back(e);
	i.ID = 2;
	i.duration = 1.8f;
	strcpy(i.name, "Auto explosion");

	AddParticleEffect(i);

	/***
	 * Engine burst
	 ***/

	e.FillDefaults();

	e.timeToLive		= 0.09f;

	e.position			= D3DXVECTOR3(0.f, 0.f, -10.f);
	e.positionSpread			= D3DXVECTOR3(2.f, 0.f, 0.f);

	e.direction			= D3DXVECTOR3(0.f, 0.f, -40.f);
	e.directionSpread	= D3DXVECTOR3(0.f, 80.f, 10.f);

	e.colorStart		= D3DXCOLOR(0.3f, 0.4f, 0.9f, 0.6f);
	e.colorEnd			= D3DXCOLOR(0.2f, 0.3f, 0.8f, 0.1f);/**/
	/*e.colorStart		= D3DXCOLOR(255, 255, 255, 255);
	e.colorEnd			= D3DXCOLOR(255, 255, 255, 255);/**/

	e.scaleStart		= 4.5f;
	e.scaleEnd			= 3.5f;

	//e.inheritParentSpeed	= false;
	e.autoEmit			= false;
	//e.additiveBlend = false;

	strcpy(e.resourceFilename, "textures\\part1.bmp");

	e.emitFrequency		= 0.01f;

	e.particleCount		= 6;
	e.maxParticleCount	= 60;

	strcpy(e.name, "Engine flame");

	// Default engine particle effect
	/*i.params.bAutoEmit = TRUE;
	i.params.bAutoKillObject = TRUE;
	i.ID = 2;
	i.params.fFrequency = 100.f;
	i.params.fObjectLifetime = 8.0f;
	strcpy(i.description, "Auto explosion");
	AddParticleEffect(i);*/

	SParticleSystemInfo i2;
	i2.ID = 3;
	i2.duration = 0.f;
	i2.params.push_back(e);
	i2.bAutoKillObject = false;
	strcpy(i2.name, "Ship engine");
	AddParticleEffect(i2);

}

bool CParticleEffects::LoadDatabase(const char *filename)
{
	/*SaveDatabase("data\\peffects.dtb");
	return true;/**/

	fstream f;
	f.open(filename, ios::in | ios::binary);
	if(!f.is_open())
	{
		CPrinter::logError("Couldn't open particle database file");
		return false;
	}
	m_effects.clear();

	DWORD id = 0;

	try {
		f.read((char*)&id, sizeof(DWORD));
		if(id != PARTICLE_FILE_BEGIN_IDENTIFIER)
		{
			f.close();
			CPrinter::logError("CParticleEffects::LoadDatabase: File begin identifier mismatch");
			return false;
		}

		DWORD count=0;
		DWORD emitterCount = 0;
		f.read((char*)&count, sizeof(DWORD));

		SParticleSystemInfo item;
		sEMITTER_PARAMS eParams;

		DWORD i;

		for(i=0;i<count;i++)
		{
			// Load the constant part of the info structure
			/*f.read((char*)&item.name[0], sizeof(char)*64+sizeof(float)+sizeof(long)+sizeof(bool) );
			f.read((char*)&emitterCount, sizeof(DWORD));
			item.params.clear();
			for(j=0; j<emitterCount; ++j)
			{
				f.read((char*)&eParams, sizeof(sEMITTER_PARAMS));
				item.params.push_back(eParams);
			}*/
			if(!ReadEffectFromStream(f, item))
				throw exception("Corrupted file");
			m_effects.push_back(item);
		}
	}
	catch (exception e) {
		CPrinter::logError("CParticleEffects::LoadDatabase: Unknown error while reading the file");
		f.close();
		return false;
	}

	f.close();

	// Preload particle textures / models
	vector<SParticleSystemInfo>::iterator it = m_effects.begin();
	vector<sEMITTER_PARAMS>::iterator it2;
	CResourceManager &r = CResourceManager::GetManager();
	for(; it != m_effects.end(); ++it)
	{
		for(it2 = it->params.begin(); it2 != it->params.end(); ++it2)
		{
			if(it2->hasMesh)
				r.GetMesh(it2->resourceFilename);
			else
				r.GetTexture(it2->resourceFilename);
		}
	}

	return TRUE;
}

void CParticleEffects::ClearDatabase()
{
	m_effects.clear();
}

bool CParticleEffects::SaveDatabase(const char *filename)
{
	if(filename == NULL)
		return false;

	fstream f;
	f.open(filename, ios::out | ios::binary);
	if(!f.is_open())
	{
		CPrinter::logError("Couldn't open particle database file for saving");
		return false;
	}

	DWORD identifier = PARTICLE_FILE_BEGIN_IDENTIFIER;

	DWORD i;
	try {
		f.write((const char*)&identifier, sizeof(DWORD));

		DWORD count = m_effects.size();
		f.write((const char*)&count, sizeof(DWORD));

		for(i=0;i<count;i++)
		{
			/*f.write((const char*)&m_effects[i].name[0], sizeof(char)*64+sizeof(float)+sizeof(long)+sizeof(bool) );

			emitterCount = m_effects[i].params.size();
			f.write((const char*)&emitterCount, sizeof(DWORD));
			for(j=0; j<emitterCount; ++j)
			{
				f.write((const char*)&m_effects[i].params[j], sizeof(sEMITTER_PARAMS));
			}*/
			WriteEffectIntoSTream(f, m_effects[i]);
		}
	} catch (exception e) {
		CPrinter::logError("CParticleEffects::SaveDatabase: unknown write error");
		f.close();
		return false;
	}

	f.close();

	return true;
}

bool CParticleEffects::ReadEffectFromStream(std::fstream &f, SParticleSystemInfo &effect)
{
	// Load the constant part of the info structure
	DWORD emitterCount = effect.params.size();
	f.read((char*)&effect.name[0], sizeof(char)*64+sizeof(float)+sizeof(long)+sizeof(bool) );
	f.read((char*)&emitterCount, sizeof(DWORD));

	if(emitterCount > 20)
		return false;

	effect.params.clear();
	sEMITTER_PARAMS eParams;
	for(DWORD j=0; j<emitterCount; ++j)
	{
		f.read((char*)&eParams, sizeof(sEMITTER_PARAMS));
		effect.params.push_back(eParams);
	}

	return true;
}

bool CParticleEffects::WriteEffectIntoSTream(std::fstream &f, SParticleSystemInfo &effect)
{
	f.write((const char*)&effect.name[0], sizeof(char)*64+sizeof(float)+sizeof(long)+sizeof(bool) );

	DWORD emitterCount = effect.params.size();
	f.write((const char*)&emitterCount, sizeof(DWORD));
	
	for(DWORD j=0; j<emitterCount; ++j)
	{
		f.write((const char*)&effect.params[j], sizeof(sEMITTER_PARAMS));
	}

	return true;
}

bool CParticleEffects::LoadDefaultDatabase()
{
	return LoadDatabase("data\\peffects.dtb");
}

IParticleSystemSceneNode *CParticleEffects::StartEffect(ISceneNode *parent, long id, D3DXVECTOR3 relativePosition)
{
	if(id < 0 || parent == NULL)
		return NULL;

	vector<SParticleSystemInfo>::iterator it = m_effects.begin();
	for(; it != m_effects.end(); it++)
		if(it->ID == id)
		{
			return StartEffect(parent, *it, relativePosition);
		}
	return NULL;
}

IParticleSystemSceneNode *CParticleEffects::StartEffect(D3DXVECTOR3 position, D3DXVECTOR3 rotation, long id)
{
	if(id < 0)
		return NULL;
	id = 2;

	vector<SParticleSystemInfo>::iterator it = m_effects.begin();
	for(; it != m_effects.end(); it++)
		if(it->ID == id)
		{
			return StartEffect(position, rotation, *it);
		}
	return NULL;
}

IParticleSystemSceneNode *CParticleEffects::StartEffect(ISceneNode *parent, SParticleSystemInfo &info, D3DXVECTOR3 relativePosition)
{
	if(parent == NULL)
		return NULL;

	IParticleSystemSceneNode *node = GetEdgewalkerApp()->GetSceneManager()->AddParticleSystemSceneNode(
		parent, 0, D3DXVECTOR3(0.f, 0.f, 0.f), info.bAutoKillObject, info.duration);
	if(node == NULL)
		return NULL;

	node->SetTranslation(relativePosition);
	
	vector<sEMITTER_PARAMS>::iterator i;
	for(i = info.params.begin(); i != info.params.end(); ++i)
		node->CreateEmitter(*i);

	return node;
}

IParticleSystemSceneNode *CParticleEffects::StartEffect(D3DXVECTOR3 position, D3DXVECTOR3 rotation, SParticleSystemInfo &info)
{
	IParticleSystemSceneNode *node = GetEdgewalkerApp()->GetSceneManager()->AddParticleSystemSceneNode(
		NULL, 0, position, true, info.duration);
	if(node == NULL)
		return NULL;
	node->SetRotation(rotation);
	node->UpdateAbsolutePosition();

	vector<sEMITTER_PARAMS>::iterator i;
	for(i = info.params.begin(); i != info.params.end(); ++i)
		node->CreateEmitter(*i);

	return node;
}

SParticleSystemInfo *CParticleEffects::GetEffectData(long id)
{
	if(id < 0)
		return NULL;

	vector<SParticleSystemInfo>::iterator it = m_effects.begin();
	for(; it != m_effects.end(); it++)
		if(it->ID == id)
		{
			return &(*it);
		}
	return NULL;
}

bool CParticleEffects::ExportTemplate(long id, std::string fullPath)
{
	vector<SParticleSystemInfo>::iterator it = m_effects.begin();
	for(;it != m_effects.end(); it++)
		if(it->ID == id)
		{
			fstream f;
			f.open(fullPath.c_str(), ios::out | ios::binary);
			if(!f.is_open())
			{
				CPrinter::logError("Couldn't open particle effect file for saving");
				return false;
			}

			DWORD id = 51024;
			f.write((const char*)&id, sizeof(DWORD));

			WriteEffectIntoSTream(f, *it);

			f.close();

			return true;
		}
	return false;
}

bool CParticleEffects::ImportTemplate(long id, std::string fullPath)
{
	SParticleSystemInfo t;
	fstream f;
	f.open(fullPath.c_str(), ios::in | ios::binary);
	if(!f.is_open())
	{
		CPrinter::logError("Couldn't open particle file");
		return false;
	}

	DWORD iid;
	f.read((char*)&iid, sizeof(DWORD));
	if(iid != 51024)
	{
		f.close();
		return false;
	}

	if(!ReadEffectFromStream(f, t))
	{
		f.close();
		CPrinter::logError("Particle effect import failed: failed reading file");
		return false;
	}
	f.close();

	if(id == -1)
		t.ID = GetNewID();
	else
	{
		t.ID = id;
		std::vector<SParticleSystemInfo>::iterator it = m_effects.begin();
		for(;it != m_effects.end(); it++)
			if(it->ID == id)
			{
				*it = t;
				return true;
			}
	}
	m_effects.push_back(t);

	return true;
}

long CParticleEffects::GetNewID()
{
	long id = 1;
	std::vector<SParticleSystemInfo>::iterator it = m_effects.begin();
	for(;it != m_effects.end(); it++)
		if(it->ID >= id)
			id = it->ID + 1;

	return id;
}