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

#include "..\\GameSettings.h"
#include "..\\Edgewalker\\Printer.h"
#include "SoundManager.h"
#include "..\\Edgewalker\\Utilities.h"
#include "..\\OnTheEdge.h"
#include <fmod_errors.h>

CSoundManager *CSoundManager::m_this = 0;

CSoundManager::CSoundManager() : m_system(0), m_musicVolume(1.f), m_effectVolume(1.f),
  m_lowestDynamicID(-10001)
{
	CSoundSettings::InitializeSettings();

	if(m_this == 0)
	{
		m_this = this;
	}
}

CSoundManager::~CSoundManager()
{
	if(m_this == this)
		m_this = 0;

	Release();
}

void CSoundManager::Release()
{
	ReleaseSoundSystem();
	ReleaseSounds();
}

bool CSoundManager::InitializeSoundSystem()
{
	ReleaseSoundSystem();

	FMOD_RESULT res;
	std::string strError;

	res = FMOD::System_Create(&m_system);
	if(res != FMOD_OK)
	{
		strError = "Failed creating FMOD sound system: ";
		strError += FMOD_ErrorString(res);
		CPrinter::logError(strError);
		return false;
	}

	res = m_system->init(32, FMOD_INIT_NORMAL, NULL);
	if(res != FMOD_OK)
	{
		strError = "Failed initializing FMOD sound system: ";
		strError += FMOD_ErrorString(res);
		CPrinter::logError(strError);

		ReleaseSoundSystem();
		return false;
	}

	return true;
}

void CSoundManager::ReleaseSoundSystem()
{
	if(m_system)
	{
		m_system->release();
		m_system = 0;
	}
}

void CSoundManager::ReleaseSounds()
{
	std::vector<CSound*>::iterator it = m_sounds.begin();
	for(; it != m_sounds.end(); ++it)
		if(*it)
			delete (*it);
	m_sounds.clear();
}

CSound *CSoundManager::GetSound(long id)
{
	// Don't even check negative ids, that means it doesn't exist unless the
	// id is less than -10000 in which case the id means it has been assigned
	// for a dynamically loaded sound sample
	if(id < 1 && id > -10000)
		return 0;
	std::vector<CSound*>::iterator it = m_sounds.begin();
	for(; it != m_sounds.end(); ++it)
		if((*it)->GetID() == id)
		{
			(*it)->SetIsMarked(false);
			return (*it);
		}

	// If the sound is dynamic this shouldn't be reached but make the check all the same
	if(id < 1)
		return NULL;

	// Sound isn't loaded so create one
	bool found = false;
	std::vector<SSoundTemplate>::iterator tIt = m_soundTemplates.begin();
	for(; tIt != m_soundTemplates.end(); ++tIt)
		if(tIt->id == id)
		{
			found = true;
			break;
		}

	if(!found)
	{
		std::string str = "Couldn't find sound with id ";
		str += (char)id;
		str += " from database";
		CPrinter::logError(str);
		return 0;
	}

	CSound *sound = new CSound(m_system, id);
	m_sounds.push_back(sound);
	if(tIt->soundType == SOUND_TYPE_MUSIC)
		sound->LoadSound(tIt->filename, tIt->stream, tIt->soundType, tIt->volume, m_musicVolume);
	else
		sound->LoadSound(tIt->filename, tIt->stream, tIt->soundType, tIt->volume, m_effectVolume);

	return sound;
}

CSound *CSoundManager::GetSound(std::string filename, bool bStream, SOUND_TYPE type)
{
	if(filename.length() < 2)
		return NULL;

	bool found = false;
	std::vector<std::pair<std::string, long> >::iterator it = m_dynamicSoundIDs.begin();
	for(; it != m_dynamicSoundIDs.end(); ++it)
		if(it->first == filename)
		{
			found = true;
			break;
		}

	if(found)
	{
		return GetSound(it->second);
	}

	CSound *sound = new CSound(m_system, --m_lowestDynamicID);
	bool succeeded;
	if(bStream)
		succeeded = sound->LoadSound(filename, bStream, type, 1.0f, m_musicVolume);
	else
		succeeded = sound->LoadSound(filename, bStream, type, 1.0f, m_effectVolume);

	if(!succeeded)
	{
		// If loading the sound was unsuccesfull, store the filename with id -1 so
		// that the sound won't be tried to be loaded again and again but will be
		// outright rejected the next go
		delete sound;
		m_dynamicSoundIDs.push_back(std::pair<std::string,long>(filename, -1));
		return NULL;
	}
	else
	{
		// If succeeded store the filename - id pair and the sound itself
		m_dynamicSoundIDs.push_back(std::pair<std::string,long>(filename, m_lowestDynamicID));
		m_sounds.push_back(sound);
	}

	return sound;
}

void CSoundManager::SetMusicVolume(float volume)
{
	if(volume < 0.f)
		volume = 0.f;
	if(volume > 1.f)
		volume = 1.f;

	m_musicVolume = volume;

	std::vector<CSound*>::iterator it = m_sounds.begin();
	for(; it != m_sounds.end(); ++it)
		if((*it)->GetType() == SOUND_TYPE_MUSIC)
			(*it)->UpdateVolume(volume);
}

void CSoundManager::SetEffectVolume(float volume)
{
	if(volume < 0.f)
		volume = 0.f;
	if(volume > 1.f)
		volume = 1.f;

	m_effectVolume = volume;

	std::vector<CSound*>::iterator it = m_sounds.begin();
	for(; it != m_sounds.end(); ++it)
		if((*it)->GetType() != SOUND_TYPE_MUSIC)
			(*it)->UpdateVolume(volume);
}

bool CSoundManager::LoadDatabase(std::string filename)
{
	return LoadVectorFromBinaryFile<SSoundTemplate>(m_soundTemplates, filename.c_str(), SOUND_DATABASE_BEGIN_IDENTIFIER);
}

bool CSoundManager::SaveDatabase(std::string filename)
{
	return SaveVectorToBinaryFile<SSoundTemplate>(m_soundTemplates, filename.c_str(), SOUND_DATABASE_BEGIN_IDENTIFIER);
}

void CSoundManager::ClearDatabase()
{
	m_soundTemplates.clear();
	m_dynamicSoundIDs.clear();

	// Remove all static sounds from the music stack
	std::vector<std::pair<CSound*, bool> >::iterator mit = m_musicStack.begin();
	for(; mit != m_musicStack.end();)
	{
		if(mit->first->GetID() > 0)
		{
			mit->first->Stop(false, 0.f);
			mit = m_musicStack.erase(mit);
		}
		else
			++mit;
	}

	// Release all static sounds (sounds which have been loaded from the database templates)
	std::vector<CSound*>::iterator it = m_sounds.begin();
	for(; it != m_sounds.end();)
	{
		if(*it)
		{
			if((*it)->GetID() > 0)
			{
				delete (*it);
				//(*it)->Kill();
				it = m_sounds.erase(it);
				//++it;
				continue;
			}
		}
		else
		{
			it = m_sounds.erase(it);
			continue;
		}
		++it;
	}
}

bool CSoundManager::LoadDefaultDatabase()
{
	return LoadDatabase("data\\sounds.dtb");
}

void CSoundManager::FillDefaultDatabase()
{
	SSoundTemplate t;
	strcpy_s(t.filename, 127, "sounds\\music\\Ambient1.wav");
	t.id = 1;
	strcpy_s(t.name, 31, "Sekoambient");
	t.stream = true;
	t.soundType = SOUND_TYPE_MUSIC;
	t.volume = 1.f;

	m_soundTemplates.push_back(t);
}

void CSoundManager::BeginLevelSounds()
{
	std::vector<CSound*>::iterator it = m_sounds.begin();
	for(; it != m_sounds.end(); ++it)
		(*it)->SetIsMarked(true);
}

void CSoundManager::EndLevelSounds()
{
	std::vector<CSound*>::iterator it = m_sounds.begin();
	for(; it != m_sounds.end();)
	{
		if((*it)->IsMarked())
		{
			delete (*it);
			it = m_sounds.erase(it);
		}
		else
			++it;
	}
}

void CSoundManager::Update(float frameTime)
{
	m_system->update();
	CSound::UpdateFades(frameTime);

	float volume;
	volume = CGameSettings::GetSettingsObject()->GetSetting(GO_MUSIC_VOLUME);
	if(volume != m_musicVolume)
	{
		std::vector<CSound*>::iterator it = m_sounds.begin();
		for(; it != m_sounds.end(); ++it)
			if((*it)->GetType() == SOUND_TYPE_MUSIC)
				(*it)->UpdateVolume(volume);
	}
	m_musicVolume = volume;
	volume = CGameSettings::GetSettingsObject()->GetSetting(GO_SOUND_VOLUME);
	if(volume != m_effectVolume)
	{
		std::vector<CSound*>::iterator it = m_sounds.begin();
		for(; it != m_sounds.end(); ++it)
			if((*it)->GetType() != SOUND_TYPE_MUSIC)
				(*it)->UpdateVolume(volume);
	}
	m_effectVolume = volume;
}

void CSoundManager::PlayMusic(CSound *sound, bool menu, bool fadeOld)
{
	if(sound == NULL)
		return;
	//std::vector<std::pair<CSound*, bool> >
	if(!m_musicStack.empty())
	{
		int index = m_musicStack.size()-1;
		if(m_musicStack[index].first == sound)
		{
			sound->Stop(false, 0.f);
			sound->UpdateVolume(m_musicVolume);
			if(menu)
				sound->Play(-1, SOUND_TYPE_MUSIC);
			else
				//sound->Play(0, SOUND_TYPE_MUSIC, OnPlayEnded);
				sound->Play(0, SOUND_TYPE_MUSIC);
			return;
		}
		m_musicStack[index].first->Pause(true, 0.5f);
	}
	if(!menu)
	{
		// If game music starts playing, it gets priority and wipes the whole menu music stack away
		m_musicStack.clear();
	}
	if(sound != NULL)
	{
		sound->UpdateVolume(m_musicVolume);
		m_musicStack.push_back(std::pair<CSound*, bool>(sound, menu));
		if(menu)
			sound->Play(-1, SOUND_TYPE_MUSIC);
		else
			//sound->Play(0, SOUND_TYPE_MUSIC, OnPlayEnded);
			sound->Play(0, SOUND_TYPE_MUSIC);
	}
}

void CSoundManager::ReportCurrentMenuClosed()
{
	if(m_musicStack.empty())
		return;
	int index = m_musicStack.size()-1;
	if(m_musicStack[index].second)
		m_musicStack[index].first->Stop(true, 0.5f);
}

void CSoundManager::ReportGameUnPaused()
{
	if(m_musicStack.empty())
		return;

	int index = m_musicStack.size()-1;
	if(m_musicStack[index].second)
		m_musicStack[index].first->Stop(true, 0.5f);

	// Loop and discard menu musics from the stack unless you encounter the game music
	int popped = 0;
	for(; index >= 0; --index)
	{
		if(m_musicStack[index].second)
		{
			m_musicStack.pop_back();
			++popped;
			continue;
		}
		else
		{
			int i=2;
			break;
		}
	}

	// If this was topmost already then there's no reason to unpause (and unfade)
	if(index >= 0 && popped != 0)
	{
		m_musicStack[index].first->UnPause(true, 0.5f);
	}
}

FMOD_RESULT F_CALLBACK CSoundManager::OnPlayEnded(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, int command, unsigned int commanddata1, unsigned int commanddata2)
{
	FMOD::Channel *cppchannel = (FMOD::Channel *)channel;

	// TODO: better feedback system
	GetOTE()->GetCurrentLevel()->PlayRandomBackgroundMusic();
	return FMOD_OK;
}