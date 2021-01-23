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

#ifndef __CSOUNDSETTINGS_H__
#define __CSOUNDSETTINGS_H__

enum SOUND_TYPE {
	SOUND_TYPE_MUSIC=0,
	SOUND_TYPE_INTERFACE,
	SOUND_TYPE_EXPLOSION,
	SOUND_TYPE_PLAYER_GUN,
	SOUND_TYPE_PLAYER_MISC,
	SOUND_TYPE_ENEMY_GUN,
	SOUND_TYPE_ENEMY_MISC,
	SOUND_TYPE_ENVIRONMENT_MISC		// Must be last
};

class CSoundSettings
{
public:
	static int GetPriority(SOUND_TYPE type)
	{
		return m_priorities[type];
	}

	static int GetChannel(SOUND_TYPE type)
	{
		int channel = m_firstChannel[type] + m_channelRotation[type];
		++m_channelRotation[type];
		if(m_channelRotation[type] >= m_channelCount[type])
			m_channelRotation[type] = 0;
		return channel;
	}

	static void InitializeSettings()
	{
		m_priorities[SOUND_TYPE_MUSIC] = 0;
		m_priorities[SOUND_TYPE_INTERFACE] = 1;
		m_priorities[SOUND_TYPE_EXPLOSION] = 20;
		m_priorities[SOUND_TYPE_PLAYER_GUN] = 50;
		m_priorities[SOUND_TYPE_PLAYER_MISC] = 80;
		m_priorities[SOUND_TYPE_ENEMY_GUN] = 70;
		m_priorities[SOUND_TYPE_ENEMY_MISC] = 250;
		m_priorities[SOUND_TYPE_ENVIRONMENT_MISC] = 240;
		//m_priorities[SOUND_TYPE_] = ;

		m_channelCount[SOUND_TYPE_MUSIC] = 1;
		m_channelCount[SOUND_TYPE_INTERFACE] = 1;
		m_channelCount[SOUND_TYPE_EXPLOSION] = 3;
		m_channelCount[SOUND_TYPE_PLAYER_GUN] = 1;
		m_channelCount[SOUND_TYPE_PLAYER_MISC] = 2;
		m_channelCount[SOUND_TYPE_ENEMY_GUN] = 2;
		m_channelCount[SOUND_TYPE_ENEMY_MISC] = 2;
		m_channelCount[SOUND_TYPE_ENVIRONMENT_MISC] = 4;

		int channel = 0;
		for(int i=0;i<SOUND_TYPE_ENVIRONMENT_MISC+1; ++i)
		{
			m_firstChannel[i] = channel;
			channel += m_channelCount[i];
			m_channelRotation[i] = 0;
		}
	}

	static int GetChannelCount(SOUND_TYPE type) {return m_channelCount[type];}
	static int GetFirstChannel(SOUND_TYPE type) {return m_firstChannel[type];}

private:
	static int m_priorities[SOUND_TYPE_ENVIRONMENT_MISC+1];
	static int m_channelCount[SOUND_TYPE_ENVIRONMENT_MISC+1];
	static int m_firstChannel[SOUND_TYPE_ENVIRONMENT_MISC+1];
	static int m_channelRotation[SOUND_TYPE_ENVIRONMENT_MISC+1];
};

#endif // #ifndef __CSOUNDSETTINGS_H__