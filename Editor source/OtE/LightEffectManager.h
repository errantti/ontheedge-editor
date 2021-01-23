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

#ifndef __CLIGHTEFFECTMANAGER_H__
#define __CLIGHTEFFECTMANAGER_H__

#include "Edgewalker\\Scene\\LightSceneNode.h"
#include <vector>

#define LIGHT_EFFECT_FILE_IDENTIFIER		0x123456FB

class CLightEffectManager
{
public:
	CLightEffectManager();
	~CLightEffectManager();

	CLightSceneNode *CreateLightEffect(ISceneNode *parent, long effectID, D3DXVECTOR3 relativePosition = D3DXVECTOR3(0.f, 0.f, 0.f));

	SDynamicPointLightParams *GetLightEffect(long id);

	void AddEffect(SDynamicPointLightParams effect) {m_effects.push_back(effect);}

	std::vector<SDynamicPointLightParams> &GetEffects() {return m_effects;}

	bool LoadDatabase(const char *filename);
	bool SaveDatabase(const char *filename);

	void ClearDatabase();

	bool ExportTemplate(long id, std::string fullPath);
	bool ImportTemplate(long id, std::string fullPath);

	long GetNewID();

	static CLightEffectManager &GetManager() {return *m_this;}

protected:
	std::vector<SDynamicPointLightParams> m_effects;

	static CLightEffectManager *m_this;
};

#endif // #ifndef __CLIGHTEFFECTMANAGER_H__