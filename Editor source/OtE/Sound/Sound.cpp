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

#include "..\\Edgewalker\\Printer.h"
#include "Sound.h"

int CSoundSettings::m_priorities[SOUND_TYPE_ENVIRONMENT_MISC+1];
int CSoundSettings::m_channelCount[SOUND_TYPE_ENVIRONMENT_MISC+1];
int CSoundSettings::m_firstChannel[SOUND_TYPE_ENVIRONMENT_MISC+1];
int CSoundSettings::m_channelRotation[SOUND_TYPE_ENVIRONMENT_MISC+1];

std::vector<CSound*> CSound::m_updateList;

CSound::CSound(FMOD::System *system, long id) : 
	m_system(system), m_sound(NULL), m_ID(id), m_bMarked(false), m_baseVolume(1.f),
	m_bFading(false), m_bFadingOut(false), m_channel(NULL),	m_fadeTimer(0.f),
	m_bDead(false)
{
}

CSound::~CSound()
{
	Release();
	RemoveSoundFromUpdateList(this);
}

void CSound::RemoveSoundFromUpdateList(CSound *sound)
{
	std::vector<CSound*>::iterator it = m_updateList.begin();
	for(; it != m_updateList.end(); ++it)
		if(sound == *it)
		{
			m_updateList.erase(it);
			return;
		}
}

void CSound::AddSoundToUpdateList(CSound *sound)
{
	std::vector<CSound*>::iterator it = m_updateList.begin();
	for(; it != m_updateList.end(); ++it)
		if(sound == *it)
		{
			return;
		}
	m_updateList.push_back(sound);
}

void CSound::UpdateFades(float time)
{
	std::vector<CSound*>::iterator it = m_updateList.begin();
	for(; it != m_updateList.end();)
	{
		if(*it == NULL)
		{
			it = m_updateList.erase(it);
			continue;
		}
		else
		{
			if((*it)->UpdateFade(time))
			{
				it = m_updateList.erase(it);
				continue;
			}
		}
		++it;
	}
}

void CSound::Release()
{
	/*if(m_sound != NULL)
	{
		m_sound->release();
		m_sound = NULL;
	}*/
}

bool CSound::LoadSound(std::string filename, bool stream, SOUND_TYPE type, float volume, float generalVolume)
{
	if(m_sound != 0)
		return false;

	m_type = type;
	FMOD_RESULT result;

	if(stream)
	{
		if(type == SOUND_TYPE_MUSIC)
			result = m_system->createStream(filename.c_str(), FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &m_sound);
		else
			result = m_system->createStream(filename.c_str(), FMOD_DEFAULT, 0, &m_sound);
	}
	else
	{
		result = m_system->createSound(filename.c_str(), FMOD_DEFAULT, 0, &m_sound);
	}

	if(result != FMOD_OK)
	{
		CPrinter::logError("CSound::LoadSound: FMOD couldn't load sound");
		return false;
	}
	
	m_priority = CSoundSettings::GetPriority(m_type);

	m_baseVolume = volume * generalVolume;
	result = m_sound->setDefaults(44100, volume * generalVolume, 0.f, m_priority);
	if(result != FMOD_OK)
	{
		CPrinter::logError("CSound::LoadSound: couldn't set defaults for FMOD sound");
		return false;
	}

	return true;
}

void CSound::Play(int loopCount)
{
	Play(loopCount, m_type);
}

void CSound::Play(int loopCount, SOUND_TYPE type, FMOD_CHANNEL_CALLBACK callbackOnEnd)
{
	if(m_sound == NULL)
		return;

	m_bFading = m_bFadingOut = false;
	m_bPausing = false;

	//int channel = CSoundSettings::GetChannel(m_type);

	FMOD_RESULT res;
	//FMOD_RESULT res = m_system->playSound((FMOD_CHANNELINDEX)channel, m_sound, false, &fmodChannel);
	res = m_sound->setLoopCount(loopCount);
	//if(type == SOUND_TYPE_MUSIC)
		res = m_system->playSound(FMOD_CHANNEL_FREE, m_sound, false, &m_channel);
	/*else if(CSoundSettings::GetChannelCount(type) == 1)
		res = m_system->playSound((FMOD_CHANNELINDEX)CSoundSettings::GetFirstChannel(type), m_sound, false, &m_channel);
	else
		res = m_system->playSound(FMOD_CHANNEL_FREE, m_sound, false, &m_channel);*/

	if(res == FMOD_OK && callbackOnEnd != NULL)
	{
		//m_channel->setCallback(FMOD_CHANNEL_CALLBACKTYPE_END, callbackOnEnd, 0);
	}
}

void CSound::UnPause(bool fadeIn, float fadeTime)
{
	if(m_channel != NULL)
	{
		m_channel->setPaused(false);
		if(fadeIn)
			m_channel->setVolume(0.f);
	}

	m_fadeTimer = 0.f;
	if(fadeIn)
	{
		m_bFading = true;
		m_bFadingOut = false;
		m_fadeTimeCoefficient = 1.f / fadeTime;
		AddSoundToUpdateList(this);
	}
}

void CSound::Pause(bool fadeOut, float fadeTime)
{
	m_fadeTimer = 1.f;
	if(fadeOut)
	{
		m_bFading = true;
		m_bFadingOut = true;
		m_bPausing = true;
		m_fadeTimeCoefficient = 1.f / fadeTime;
		AddSoundToUpdateList(this);
	}
	else if(m_channel != NULL)
		m_channel->setPaused(true);
}

void CSound::Stop(bool fadeOut, float fadeTime)
{
	m_fadeTimer = 1.f;
	if(fadeOut)
	{
		m_bFading = true;
		m_bFadingOut = true;
		m_bPausing = false;
		m_fadeTimeCoefficient = 1.f / fadeTime;
		m_updateList.push_back(this);
	}
	else if(m_channel != NULL)
	{
		m_channel->stop();
		m_channel = NULL;
	}
}

void CSound::UpdateVolume(float volume)
{
	if(m_sound == NULL)
		return;

	/*FMOD_RESULT result = m_sound->setDefaults(44100, m_baseVolume * volume, 0.f, m_priority);
	if(result != FMOD_OK)
	{
		CPrinter::logError("CSound::LoadSound: couldn't set defaults for FMOD sound");
	}/**/
	m_baseVolume = volume;
	m_sound->setDefaults(44100, volume, 0.f, m_priority);
	if(m_channel != NULL)
		m_channel->setVolume(volume);
}

bool CSound::UpdateFade(float time)
{
	// If not fading then request to be removed from update list
	if(!m_bFading || m_channel == NULL)
		return true;

	if(m_bFadingOut)
	{
		m_fadeTimer -= time * m_fadeTimeCoefficient;
		if(m_fadeTimer <= 0.f)
		{
			if(m_bPausing)
				m_channel->setPaused(true);
			else
			{
				m_channel->stop();
				m_channel = NULL;
			}
			m_bFading = false;
			return true;
		}
	}
	else
	{
		m_fadeTimer += time * m_fadeTimeCoefficient;
		if(m_fadeTimer >= 1.f)
		{
			m_bFading = false;
			return true;
		}
	}

	FMOD_RESULT res = m_channel->setVolume(m_fadeTimer * m_baseVolume);
	if(res != FMOD_OK)
	{
		//
		int i = 0;
	}

	return false;
}

void CSound::DeadCheck()
{
	if(m_bDead)
		CPrinter::logError("Accessed dead sound object");
}