
/*********************************
 *
 *		Edgewalker engine
 * Copyright (2004) Jukka Tykkyläinen
 * http://koti.mbnet.fi/jtykky
 *
 * SoundEngine.h: interface for the CSoundEngine class
 *
 * The sound engine implemented with direct sound
 * interfaces
 *
 * Requires:
 * Project wide define DIRECTSOUND_VERSION=0x900
 * Library: dsoung.lib
 ****************************************/

#ifndef __CSOUNDENGINE_H__
#define __CSOUNDENGINE_H__

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>

// define DWORD and LONG pointer
// for the compability with VC++ 6.0
// and basic WIN32 headers
#ifndef DWORD_PTR
#define DWORD_PTR DWORD*
#endif
#ifndef LONG_PTR
#define LONG_PTR LONG*
#endif

#include <dsound.h>
#include <tchar.h>

class CSoundEngine  
{
public:
	int GetMasterVolume();
	void SetMasterVolume(int iVolume);
	void Release();
	HRESULT Create(HWND hWnd, DWORD dwFrequency, DWORD dwBitsPerSample, DWORD dwChannels, DWORD dwFlags);
	static CSoundEngine * GetSoundEngine();
	CSoundEngine();
	virtual ~CSoundEngine();

	inline HWND GetWindow() {return m_hWnd;}
	inline LPDIRECTSOUNDBUFFER GetPrimaryBuffer() {return m_pDSBPrimary;}
	inline LPDIRECTSOUND8 GetDirectSound() {return m_pDS;}

private:
	LPDIRECTSOUND8		m_pDS;
	LPDIRECTSOUNDBUFFER	m_pDSBPrimary;

	HWND				m_hWnd;

	static CSoundEngine	*m_pSoundEngine;

	int					m_iOriginalMasterVolume;
};

#endif // #ifndef __CSOUNDENGINE_H__
