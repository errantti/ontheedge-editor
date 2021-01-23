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

#include "LightEffectManager.h"
#include "Edgewalker\\Scene\\ISceneManager.h"
#include "Edgewalker\\Utilities.h"
using namespace std;

CLightEffectManager *CLightEffectManager::m_this = 0;

CLightEffectManager::CLightEffectManager()
{
	if(m_this == 0)
		m_this = this;
}

CLightEffectManager::~CLightEffectManager()
{
	if(m_this == this)
		m_this = 0;
}

SDynamicPointLightParams *CLightEffectManager::GetLightEffect(long id)
{
	vector<SDynamicPointLightParams>::iterator it = m_effects.begin();
	for(; it != m_effects.end(); ++it)
		if(it->id == id)
			return &(*it);
	return NULL;
}

CLightSceneNode *CLightEffectManager::CreateLightEffect(ISceneNode *parent, long effectID, D3DXVECTOR3 relativePosition)
{
	if(parent == NULL)
		return NULL;

	vector<SDynamicPointLightParams>::iterator it = m_effects.begin();
	for(; it != m_effects.end(); ++it)
		if(it->id == effectID)
		{
			ISceneManager *m = ISceneManager::GetManager();
			CLightSceneNode *light = m->AddLightSceneNode(parent);
			if(light == NULL)
				return NULL;

			light->SetTranslation(relativePosition);

			light->InitDynamicPointLight(*it);
			return light;
		}

	return NULL;
}

bool CLightEffectManager::LoadDatabase(const char *filename)
{
	return LoadVectorFromBinaryFile<SDynamicPointLightParams>(m_effects, filename, LIGHT_EFFECT_FILE_IDENTIFIER);
}

bool CLightEffectManager::SaveDatabase(const char *filename)
{
	return SaveVectorToBinaryFile<SDynamicPointLightParams>(m_effects, filename, LIGHT_EFFECT_FILE_IDENTIFIER);
}

void CLightEffectManager::ClearDatabase()
{
	m_effects.clear();
}

bool CLightEffectManager::ExportTemplate(long id, std::string fullPath)
{
	vector<SDynamicPointLightParams>::iterator it = m_effects.begin();
	for(;it != m_effects.end(); it++)
		if(it->id == id)
		{
			return ExportStructToFile(fullPath, *it, 1040);
		}
	return false;
}

bool CLightEffectManager::ImportTemplate(long id, std::string fullPath)
{
	SDynamicPointLightParams t;
	if(!ImportStructFromFile(fullPath, t, 1040))
		return false;

	if(id == -1)
		t.id = GetNewID();
	else
	{
		t.id = id;
		std::vector<SDynamicPointLightParams>::iterator it = m_effects.begin();
		for(;it != m_effects.end(); it++)
			if(it->id == id)
			{
				*it = t;
				return true;
			}
	}
	m_effects.push_back(t);

	return true;
}

long CLightEffectManager::GetNewID()
{
	long id = 1;
	std::vector<SDynamicPointLightParams>::iterator it = m_effects.begin();
	for(;it != m_effects.end(); it++)
		if(it->id >= id)
			id = it->id + 1;

	return id;
}