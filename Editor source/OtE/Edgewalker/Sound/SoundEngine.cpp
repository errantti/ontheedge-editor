
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

#include "SoundEngine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// declare the static member pointer
CSoundEngine * CSoundEngine::m_pSoundEngine = NULL;

CSoundEngine::CSoundEngine()
{
	m_pDS = NULL;
	m_pDSBPrimary = NULL;
	m_hWnd = NULL;
	m_pSoundEngine = this;
	m_iOriginalMasterVolume = 0;
}

CSoundEngine::~CSoundEngine()
{
}

CSoundEngine * CSoundEngine::GetSoundEngine()
{
	return m_pSoundEngine;
}

HRESULT CSoundEngine::Create(	HWND hWnd, 
								DWORD dwFrequency, 
								DWORD dwBitsPerSample, 
								DWORD dwChannels, 
								DWORD dwFlags)
{
	m_hWnd = hWnd;

	// create the direct sound object
	HRESULT hres;
	hres = ::DirectSoundCreate8(NULL, &m_pDS, NULL);
	if(FAILED(hres))
	{
		// no directx 8 installed?
		return hres;
	}

	// set the co-op level to PRIORITY
	hres = m_pDS->SetCooperativeLevel(m_hWnd, DSSCL_PRIORITY);
	if(FAILED(hres))
	{
		Release();
		return hres;
	}

	// create primary sound buffer
	DSBUFFERDESC desc;
	::memset(&desc, 0, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;

	hres = m_pDS->CreateSoundBuffer(&desc, &m_pDSBPrimary, NULL);
	if(FAILED(hres))
	{
		Release();
		return hres;
	}

	// set the primary buffer format
	WAVEFORMATEX wfx;
	::memset(&wfx, 0, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = (WORD)dwChannels;
	wfx.nSamplesPerSec = dwFrequency;
	wfx.wBitsPerSample = (WORD)dwBitsPerSample;
	wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	hres = m_pDSBPrimary->SetFormat(&wfx);
	if(FAILED(hres))
	{
		Release();
		return hres;
	}

	// get the current master volume setting
	long iVolume;
	m_pDSBPrimary->GetVolume(&iVolume);
	m_iOriginalMasterVolume = iVolume;

	// start playing the primary buffer
	hres = m_pDSBPrimary->Play(0, 0, DSBPLAY_LOOPING);
	if(FAILED(hres))
	{
		Release();
		return hres;
	}

	return S_OK;
}

void CSoundEngine::Release()
{
	// release the primary buffer
	if(m_pDSBPrimary)
	{
		// Restore the original master volume
		m_pDSBPrimary->SetVolume(m_iOriginalMasterVolume-10);

		m_pDSBPrimary->Release();
		m_pDSBPrimary = NULL;
	}

	// release the direct sound
	if(m_pDS)
	{
		m_pDS->Release();
		m_pDS = NULL;
	}
	m_hWnd = NULL;
}

/**
 * sets the vlume of the entire sound system
 * iVolume is a value in range (0 - -10000)
 * where 0 is hte maximum volume and -10000 is
 * silence
 **/
void CSoundEngine::SetMasterVolume(int iVolume)
{
	if(m_pDSBPrimary)
		m_pDSBPrimary->SetVolume(iVolume);
}

/**
 * return current master volume
 **/
int CSoundEngine::GetMasterVolume()
{
	if(m_pDSBPrimary)
	{
		long iVolume;
		m_pDSBPrimary->GetVolume(&iVolume);
		return iVolume;
	}
	return DSBVOLUME_MIN;
}
