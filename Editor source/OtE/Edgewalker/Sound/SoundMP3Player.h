
/*********************************
 *
 *		Edgewalker engine
 * Copyright (2004) Jukka Tykkyläinen
 * http://koti.mbnet.fi/jtykky
 *
 * SoundMP3Player.h: interface for the CSoundMP3Player class
 *
 * This simple class implements a basic MP3 player
 *
 * Requires:	strmiids.lib
 * The '::CoInitialize(NULL);' must be callsed
 * before this class is used
 ****************************************/

#ifndef __CSOUNDMP3PLAYER_H__
#define __CSOUNDMP3PLAYER_H__

// include basic windows stuff...
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>

// define DWORD and LONG pointer
// for the compability with VC++ 6.0
// and basic WIN32 headers
#ifndef DWORD_PTR
#define DWORD_PTR DWORD*
#endif
#ifndef LONG_PTR
#define LONG_PTR LONG*
#endif

#include <dshow.h>



class CSoundMP3Player
{
public:
	double GetPosition();
	void SetPosition(double dPos);
	void Stop();
	void Pause();
	void Play();
	void Release();
	HRESULT Create(LPCTSTR strFilename);
	enum eSTATE {	eSTATE_STOPPED=0,
					eSTATE_PLAYING,
					eSTATE_PAUSED,
					eSTATE_FORCEDWORD=0xffffffff };

	CSoundMP3Player();
	virtual ~CSoundMP3Player();

private:
	IGraphBuilder	*m_pGraph;
	eSTATE			m_eState;
};

#endif // #ifndef __CSOUNDMP3PLAYER_H__
