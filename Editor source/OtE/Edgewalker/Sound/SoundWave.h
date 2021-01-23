
/*********************************
 *
 *		Edgewalker engine
 * Copyright (2004) Jukka Tykkyläinen
 * http://koti.mbnet.fi/jtykky
 *
 * SoundWave.h: interface for the CSoundWave class
 *
 * Class implements a playable wave sound
 *
 * Requires: CSoundEngine class
 *
 * The 'CoInitialize(NULL)' must be
 * called before the real time effects
 * can be used...
 ****************************************/

#ifndef __CSOUNDWAVE_H__
#define __CSOUNDWAVE_H__

#include "SoundEngine.h"

class CSoundWave  
{
public:
	HRESULT SetEffect(GUID gdEffect, DWORD dwDuplicate);
	BOOL IsLooping(DWORD dwDuplicate);
	BOOL IsPlaying(DWORD dwDuplicate);
	int GetPan(DWORD dwDuplicate);
	void SetPan(int iPan, DWORD dwDuplicate);
	DWORD GetFrequency(DWORD dwDuplicate);
	void SetFrequency(DWORD dwFrequency, DWORD dwDuplicate);
	int GetVolume(DWORD dwDuplicate);
	void SetVolume(int iVolume, DWORD dwDuplicate);
	int GetPosition(DWORD dwDuplicate);
	void SetPosition(int iPosition, DWORD dwDuplicate);
	int BytesToMilliseconds(int iBytes);
	int MillisecondsToBytes(int iMS);
	void RePlay(BOOL bLoop, DWORD dwDuplicate);
	HRESULT Load(LPCTSTR strFilename, DWORD dwDuplicates, DWORD dwFlags);
	void Stop(DWORD dwDuplicate);
	void Play(BOOL bLoop, DWORD dwDuplicate);
	void Release();
	HRESULT Create(WORD dwFrequency, DWORD dwBitsPerSample, DWORD dwChannels, DWORD dwBytes, DWORD dwDuplicates, DWORD dwFlags);
	CSoundWave();
	virtual ~CSoundWave();

	// returns the direct sound buffer at given index
	inline LPDIRECTSOUNDBUFFER8 GetSoundBuffer(int iIndex) {return m_ppDSB[iIndex];}
	inline DWORD GetBufferCount() const {return m_dwBufferCount;}

private:
	// the direct sound buffer(s)
	LPDIRECTSOUNDBUFFER8 *m_ppDSB;
	DWORD m_dwBufferCount;
};

#endif // #ifndef __CSOUNDWAVE_H__
