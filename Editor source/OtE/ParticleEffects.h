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

#ifndef __CPARTICLEEFFECTS_H__
#define __CPARTICLEEFFECTS_H__

#include "Edgewalker\\ParticleSystem\\IParticleSystemSceneNode.h"
#include <vector>

#define PARTICLE_FILE_BEGIN_IDENTIFIER	0x1234ABC1

struct SParticleSystemInfo
{
	char name[32];
	char category[32];
	float duration;
	long ID;
	bool bAutoKillObject;
	std::vector<sEMITTER_PARAMS> params;

	void FillDefaults()
	{
		name[0] = '\0';
		category[0] = '\0';
		ID = -1;
		duration = 4.f;
	}
};

class CParticleEffects
{
public:
	CParticleEffects();
	virtual ~CParticleEffects();

	static CParticleEffects &GetManager() {return *m_this;}

	void AddParticleEffect(SParticleSystemInfo info) {m_effects.push_back(info);}

	inline vector<SParticleSystemInfo> *GetEffects() {return &m_effects;}

	// Creates a particle emitter object and attaches it to an existing object in the hierarchy
	IParticleSystemSceneNode *StartEffect(ISceneNode *parent, long id, D3DXVECTOR3 relativePosition = D3DXVECTOR3(0.f, 0.f, 0.f));
	// Creates an independent particle emitter object in a fixed position
	IParticleSystemSceneNode *StartEffect(D3DXVECTOR3 position, D3DXVECTOR3 rotation, long id);

	IParticleSystemSceneNode *StartEffect(ISceneNode *parent, SParticleSystemInfo &info, D3DXVECTOR3 relativePosition = D3DXVECTOR3(0.f, 0.f, 0.f));
	IParticleSystemSceneNode *StartEffect(D3DXVECTOR3 position, D3DXVECTOR3 rotation, SParticleSystemInfo &info);

	SParticleSystemInfo *GetEffectData(long id);

	bool LoadDatabase(const char *filename);
	bool SaveDatabase(const char *filename);
	void ClearDatabase();

	bool ExportTemplate(long id, std::string fullPath);
	bool ImportTemplate(long id, std::string fullPath);

	long GetNewID();

	bool LoadDefaultDatabase();

	void LoadDefaultEffects();

protected:
	bool ReadEffectFromStream(std::fstream &f, SParticleSystemInfo &effect);
	bool WriteEffectIntoSTream(std::fstream &f, SParticleSystemInfo &effect);
	int LoadTexture(char *filename);

	static CParticleEffects *m_this;

	vector<SParticleSystemInfo> m_effects;
};

#endif // #ifndef __CPARTICLEEFFECTS_H__
