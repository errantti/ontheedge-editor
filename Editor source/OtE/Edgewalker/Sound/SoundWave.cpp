
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

#include "SoundWave.h"
#include "WaveLoader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundWave::CSoundWave()
{
	m_ppDSB = NULL;
	m_dwBufferCount = 0;
}

CSoundWave::~CSoundWave()
{
}

/**
 * function creates an empty sound buffer with given parameters
 **/
HRESULT CSoundWave::Create(WORD dwFrequency, DWORD dwBitsPerSample, DWORD dwChannels, DWORD dwBytes, DWORD dwDuplicates, DWORD dwFlags)
{
	HRESULT hres;
	WAVEFORMATEX wfx;
	DSBUFFERDESC desc;
	
	// initialize the wfx
	::memset(&wfx, 0, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = (WORD)dwChannels;
	wfx.nSamplesPerSec = dwFrequency;
	wfx.wBitsPerSample = (WORD)dwBitsPerSample;
	wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	::memset(&desc, 0, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = dwFlags;
	desc.dwBufferBytes = dwBytes;
	desc.lpwfxFormat = &wfx;

	m_dwBufferCount = dwDuplicates + 1;

	// create the sound buffers
	LPDIRECTSOUND8 pDS = CSoundEngine::GetSoundEngine()->GetDirectSound();
	if(dwDuplicates<0)
		dwDuplicates = 0;
	m_ppDSB = new LPDIRECTSOUNDBUFFER8[m_dwBufferCount];

	if(m_ppDSB == NULL)
		return E_OUTOFMEMORY;

	::memset(m_ppDSB, 0, sizeof(LPDIRECTSOUNDBUFFER8) * m_dwBufferCount);

	LPDIRECTSOUNDBUFFER pDSB = NULL;
	hres = pDS->CreateSoundBuffer(&desc, &pDSB, NULL);
	if(FAILED(hres))
	{
		Release();
		return hres;
	}

	// query the latest sound buffer interface
	hres = pDSB->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_ppDSB[0]);

	pDSB->Release();
	if(FAILED(hres))
	{
		Release();
		return hres;
	}

	// create the duplicate buffers
	DWORD i;
	for(i=0; i<dwDuplicates;i++)
	{
		if((dwFlags & DSBCAPS_CTRLFX) == 0)
		{
			// no real time fx required, duplicate the sound buffer
			hres = pDS->DuplicateSoundBuffer(m_ppDSB[0], &pDSB);
		}
		else
		{
			// real time fx required...
			// create new sound buffer
			hres = pDS->CreateSoundBuffer(&desc, &pDSB, NULL);
		}

		if(SUCCEEDED(hres))
		{
			hres = pDSB->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_ppDSB[i+1]);
			pDSB->Release();
		}
		if(FAILED(hres))
		{
			Release();
			return hres;
		}
	}

	return S_OK;
}

/**
 * release all sound buffers
 **/
void CSoundWave::Release()
{
	if(m_ppDSB)
	{
		DWORD i;
		for(i=0; i<m_dwBufferCount; i++)
		{
			if(m_ppDSB[i])
			{
				m_ppDSB[i]->Release();
				m_ppDSB[i] = NULL;
			}
		}

		delete [] m_ppDSB;
		m_ppDSB = NULL;
	}
	m_dwBufferCount = 0;
}

/**
 * play the sound buffer or its duplicate
 **/
void CSoundWave::Play(BOOL bLoop, DWORD dwDuplicate)
{
	DWORD dwFlags = 0;
	if(bLoop)
	{
		dwFlags |= DSBPLAY_LOOPING;
	}
	m_ppDSB[dwDuplicate]->Play(0, 0, dwFlags);
}

/**
 * stops the playback of the sound buffer or its duplicate
 **/
void CSoundWave::Stop(DWORD dwDuplicate)
{
	if(dwDuplicate<0)
		dwDuplicate=0;
	m_ppDSB[dwDuplicate]->Stop();
}

/**
 * Loads a wave from the file
 **/
HRESULT CSoundWave::Load(LPCTSTR strFilename, DWORD dwDuplicates, DWORD dwFlags)
{
	HRESULT hres;
	CWaveLoader loader;

	// try to open file...
	hres = loader.Open((LPTSTR)strFilename, NULL, WAVEFILE_READ);
	if(FAILED(hres))
	{
		// failed to open wave file for reading
		// most likely the file doesn't exist
		// unknown format.
		return hres;
	}

	// create the sound buffer
	hres = Create((unsigned short)loader.m_pwfx->nSamplesPerSec, loader.m_pwfx->wBitsPerSample, loader.m_pwfx->nChannels, 
						loader.GetSize(), dwDuplicates, dwFlags);
	if(FAILED(hres))
	{
		loader.Close();
		return hres;
	}

	// load the wave data into the buffer(s)
	int i, iCount = 1;

	if((dwFlags & DSBCAPS_CTRLFX))
	{
		// real time effects are required
		// load sound data to all buffers
		iCount = m_dwBufferCount;
	}

	for(i=0;i<iCount;i++)
	{
		void *pBufferData = NULL;
		DWORD dwLength;

		// lock the sound buffer to access its data
		hres = m_ppDSB[i]->Lock(0, 0, &pBufferData, &dwLength, NULL, NULL, DSBLOCK_ENTIREBUFFER);
		if(SUCCEEDED(hres))
		{
			DWORD dwRead;
			loader.ResetFile();
			loader.Read((BYTE*)pBufferData, dwLength, &dwRead);
			m_ppDSB[i]->Unlock(pBufferData, dwLength, NULL, 0);
		}
	}

	loader.Close();
	return S_OK;
}

/**
 * plays a sound starting from the beginning
 **/
void CSoundWave::RePlay(BOOL bLoop, DWORD dwDuplicate)
{
	Stop(dwDuplicate);
	m_ppDSB[dwDuplicate]->SetCurrentPosition(0);
	Play(bLoop, dwDuplicate);
}

/**
 * converts position in ms to position in bytes
 **/
int CSoundWave::MillisecondsToBytes(int iMS)
{
	WAVEFORMATEX wfx;
	m_ppDSB[0]->GetFormat(&wfx, sizeof(WAVEFORMATEX), NULL);
	return iMS * wfx.nAvgBytesPerSec / 1000;
}

/**
 * converts position in bytes to position in ms
 **/
int CSoundWave::BytesToMilliseconds(int iBytes)
{
	WAVEFORMATEX wfx;
	m_ppDSB[0]->GetFormat(&wfx, sizeof(WAVEFORMATEX), NULL);
	return iBytes * 1000 / wfx.nAvgBytesPerSec;
}

/**
 * set buffer play position in milliseconds
 **/
void CSoundWave::SetPosition(int iPosition, DWORD dwDuplicate)
{
	DWORD dwPosition = MillisecondsToBytes(iPosition);
	m_ppDSB[dwDuplicate]->SetCurrentPosition(dwPosition);
}

/**
 * get buffer play position in milliseconds
 **/
int CSoundWave::GetPosition(DWORD dwDuplicate)
{
	DWORD dwPosition;
	m_ppDSB[dwDuplicate]->GetCurrentPosition(&dwPosition, NULL);
	return BytesToMilliseconds(dwPosition);
}

/**
 * sets the volume of the sound 
 * iVolume is a value from range -10000 to 0
 * where 0 is the maximum volume and -10000 is mute
 **/
void CSoundWave::SetVolume(int iVolume, DWORD dwDuplicate)
{
	m_ppDSB[dwDuplicate]->SetVolume(iVolume);
}

/**
 * returns the volume
 **/
int CSoundWave::GetVolume(DWORD dwDuplicate)
{
	long iVolume;
	m_ppDSB[dwDuplicate]->GetVolume(&iVolume);
	return iVolume;
}

/**
 * set the playback frequency of the sound buffer
 **/
void CSoundWave::SetFrequency(DWORD dwFrequency, DWORD dwDuplicate)
{
	m_ppDSB[dwDuplicate]->SetFrequency(dwFrequency);
}

/**
 * returns frequency
 **/
DWORD CSoundWave::GetFrequency(DWORD dwDuplicate)
{
	DWORD dwFrequency;
	m_ppDSB[dwDuplicate]->GetFrequency(&dwFrequency);
	return dwFrequency;
}

/**
 * sets the sound panning value
 * iPan parameter:
 * -10000	full left
 * 0		center
 * 10000	full right
 **/
void CSoundWave::SetPan(int iPan, DWORD dwDuplicate)
{
	m_ppDSB[dwDuplicate]->SetPan(iPan);
}

/**
 * returns pan
 **/
int CSoundWave::GetPan(DWORD dwDuplicate)
{
	long iPan;
	m_ppDSB[dwDuplicate]->GetPan(&iPan);
	return iPan;
}

/**
 * returns TRUE if sound is currently playing
 **/
BOOL CSoundWave::IsPlaying(DWORD dwDuplicate)
{
	DWORD dwFlags;
	m_ppDSB[dwDuplicate]->GetStatus(&dwFlags);
	return (dwFlags & DSBSTATUS_PLAYING) ? TRUE : FALSE;
}

/**
 * returns TRUE if sound is currently playing looping
 **/
BOOL CSoundWave::IsLooping(DWORD dwDuplicate)
{
	DWORD dwFlags;
	m_ppDSB[dwDuplicate]->GetStatus(&dwFlags);
	return (dwFlags & DSBSTATUS_LOOPING) ? TRUE : FALSE;
}


/** 
 * enables or disables the real time effects
 * Wave must be created with DSBCPS_CTRLFX flag
 **/
HRESULT CSoundWave::SetEffect(GUID gdEffect, DWORD dwDuplicate)
{
	// we cannot set effects into the sound
	// while it's playing
	Stop(dwDuplicate);

	// check if effect parameter is GUID_NULL
	if(::IsEqualGUID(gdEffect, GUID_NULL))
	{
		// no effect specified, stop any previous effect
		return m_ppDSB[dwDuplicate]->SetFX(0, NULL, NULL);
	}

	DSEFFECTDESC desc;
	::memset(&desc, 0, sizeof(DSEFFECTDESC));
	desc.dwSize = sizeof(DSEFFECTDESC);
	desc.guidDSFXClass = gdEffect;

	return m_ppDSB[dwDuplicate]->SetFX(1, &desc, NULL);
}
