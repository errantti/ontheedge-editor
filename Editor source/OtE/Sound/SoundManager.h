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

#ifndef __CSOUNDMANAGER_H__
#define __CSOUNDMANAGER_H__

#include <vector>
#include "Sound.h"

#define SOUND_DATABASE_BEGIN_IDENTIFIER		0xFF123456

class CSoundManager
{
public:
	struct SSoundTemplate
	{
		char filename[128];
		char name[32];
		float volume;
		long id;
		SOUND_TYPE soundType;
		bool stream;

		void FillDefaults()
		{
			filename[0] = '\0';
			name[0] = '\0';
			volume = 1.f;
			id = -1;
			soundType = SOUND_TYPE_ENVIRONMENT_MISC;
			stream = false;
		}
	};

	CSoundManager();
	~CSoundManager();

	void FillDefaultDatabase();

	bool InitializeSoundSystem();

	void Release();
	void ReleaseSoundSystem();
	void ReleaseSounds();

	bool LoadDatabase(std::string filename);
	bool SaveDatabase(std::string filename);

	void ClearDatabase();

	bool LoadDefaultDatabase();

	void SetMusicVolume(float volume);
	void SetEffectVolume(float volume);

	void Update(float frameTime);

	CSound *GetSound(long id);
	CSound *GetSound(std::string filename, bool bStream, SOUND_TYPE type = SOUND_TYPE_INTERFACE);

	std::vector<SSoundTemplate> &GetSoundTemplateDatabase() {return m_soundTemplates;}

	FMOD::System *GetSoundSystem() {return m_system;}

	/***
	 * This is an optional optimization
	 * When called, marks all the sounds using SetIsMarked(true)
	 * When a sound is called using GetSound the sound is
	 * unmarked. When EndLevelSounds is called only those sounds
	 * that aren't going to be used should be still marked.
	 ***/
	void BeginLevelSounds();
	/***
	 * When called, releases all marked sounds.
	 * See BeginLevelSounds for more info
	 ***/
	void EndLevelSounds();

	void ReportCurrentMenuClosed();
	void ReportGameUnPaused();

	void PlayMusic(CSound *sound, bool menu, bool fadeOld);

	static CSoundManager &GetManager() {return *m_this;}

private:
	static FMOD_RESULT F_CALLBACK OnPlayEnded(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, int command, unsigned int commanddata1, unsigned int commanddata2);

	std::vector<SSoundTemplate> m_soundTemplates;
	std::vector<CSound*> m_sounds;
	std::vector<std::pair<std::string, long> > m_dynamicSoundIDs;

	std::vector<std::pair<CSound*, bool> > m_musicStack;

	long m_lowestDynamicID;

	FMOD::System *m_system;

	float m_musicVolume, m_effectVolume;

	static CSoundManager *m_this;
};

#endif // #ifndef __CSOUNDMANAGER_H__