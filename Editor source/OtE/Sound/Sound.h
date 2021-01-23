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

#ifndef __CSOUND_H__
#define __CSOUND_H__

#include <fmod.hpp>

#include <string>
#include <vector>

#include "SoundSettings.h"

class CSound
{
public:
	CSound(FMOD::System *system, long id);
	~CSound();

	bool LoadSound(std::string filename, bool stream, SOUND_TYPE type, float volume = 1.0f, float generalVolume = 1.f);
	void SetPriority(int priority);
	void UpdateVolume(float volume);

	void Play(int loopCount = 0);
	void Play(int loopCount, SOUND_TYPE type, FMOD_CHANNEL_CALLBACK callbackOnEnd = NULL);
	void PlayWithFade(int loopCount, float endFadeTime, bool bCallbackOnEnd = false);

	void UnPause(bool fadeIn, float fadeTime);

	void Pause(bool fade, float fadeTime);
	void Stop(bool fade, float fadeTime);

	void Release();

	inline SOUND_TYPE GetType() {return m_type;}

	inline long GetID() {return m_ID;}
	inline void SetID(long id) {m_ID = id;}

	inline bool IsMarked() {return m_bMarked;}
	inline void SetIsMarked(bool value) {m_bMarked = value;}

	inline void Kill() {m_bDead = true;}

	static void UpdateFades(float time);

private:
	void DeadCheck();

	FMOD::Sound *m_sound;
	FMOD::System *m_system;
	FMOD::Channel *m_channel;

	int m_priority;
	SOUND_TYPE m_type;
	long m_ID;

	float m_fadeTimer, m_fadeTimeCoefficient;

	bool m_bPausing;
	bool m_bFading;
	bool m_bFadingOut;

	bool m_bMarked;		// Used for memory optimization. Before new level is loaded
						// all sounds are marked, during the load process the sounds
						// that are used get unmarked and when level is fully loaded
						// the marked sounds are released

	float m_baseVolume;

	bool m_bDead;		// Pending for removal

	// Returns true if it wants to be removed from the update list
	bool UpdateFade(float time);

	static std::vector<CSound*> m_updateList;
	static void RemoveSoundFromUpdateList(CSound *sound);
	static void AddSoundToUpdateList(CSound *sound);
};

#endif // #ifndef __CSOUND_H__