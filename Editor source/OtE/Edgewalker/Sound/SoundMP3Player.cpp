
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

#include "SoundMP3Player.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundMP3Player::CSoundMP3Player()
{
	m_pGraph = NULL;
	m_eState = eSTATE_STOPPED;
}

CSoundMP3Player::~CSoundMP3Player()
{
}

/** 
 * initialize the player from the given file
 **/
HRESULT CSoundMP3Player::Create(LPCTSTR strFilename)
{
	HRESULT hres;

	// create the graph builder interface
	hres = ::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_pGraph);
	if(FAILED(hres))
	{
		return hres;
	}

	WCHAR filename[MAX_PATH];

#ifdef _UNICODE
	_tcscpy(filename, strFilename);
#else
	// convert 8bit ANSI character string into the
	// 16 bit UNICODE string
	::MultiByteToWideChar(CP_ACP, 0, strFilename, -1, filename, MAX_PATH);
#endif

	// start the player
	hres = m_pGraph->RenderFile(filename, NULL);
	if(FAILED(hres))
	{
		Release();
		return hres;
	}

	return S_OK;
}

/**
 * release the player
 **/
void CSoundMP3Player::Release()
{
	if(m_pGraph)
	{
		m_pGraph->Release();
		m_pGraph = NULL;
	}
	m_eState = eSTATE_STOPPED;
}

/**
 * start playing the MP3
 **/
void CSoundMP3Player::Play()
{
	if(m_pGraph == NULL || m_eState == eSTATE_PLAYING)
		return;

	IMediaControl *pMC = NULL;
	HRESULT hres;

	hres = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&pMC);
	if(FAILED(hres))
	{
		return;
	}

	pMC->Run();
	pMC->Release();
	m_eState = eSTATE_PLAYING;
}

/**
 * pause the playback
 **/
void CSoundMP3Player::Pause()
{
	if(m_pGraph == NULL || m_eState != eSTATE_PLAYING)
		return;

	IMediaControl *pMC = NULL;
	HRESULT hres;

	hres = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&pMC);
	if(FAILED(hres))
	{
		return;
	}

	pMC->Pause();
	pMC->Release();
	m_eState = eSTATE_PAUSED;
}

void CSoundMP3Player::Stop()
{
	if(m_pGraph == NULL || m_eState == eSTATE_STOPPED)
		return;

	IMediaControl *pMC = NULL;
	HRESULT hres;

	hres = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&pMC);
	if(FAILED(hres))
	{
		return;
	}

	pMC->Stop();
	pMC->Release();
	m_eState = eSTATE_STOPPED;
}

/** 
 * set the MP3 playback position
 * parameter dPos is the position ranging
 * from 0.0 to 1.0
 **/
void CSoundMP3Player::SetPosition(double dPos)
{
	if(m_pGraph = NULL)
		return;

	// Get the position controls
	IMediaPosition *pMP = NULL;
	HRESULT hres;

	hres = m_pGraph->QueryInterface(IID_IMediaPosition, (void**)&pMP);
	if(FAILED(hres))
		return;

	double dDuration = 0.0;
	pMP->get_Duration(&dDuration);

	dPos = dDuration * dPos;
	pMP->put_CurrentPosition(dPos);
	pMP->Release();
}

/**
 * get the MP3 playback position
 **/
double CSoundMP3Player::GetPosition()
{
	if(m_pGraph == NULL)
		return 0.0;

	// Get the position controls
	IMediaPosition *pMP = NULL;
	HRESULT hres;

	hres = m_pGraph->QueryInterface(IID_IMediaPosition, (void**)&pMP);
	if(FAILED(hres))
		return 0.0;

	double dDuration = 0.0, dPos = 0.0;
	pMP->get_Duration(&dDuration);
	pMP->get_CurrentPosition(&dPos);
	pMP->Release();

	return dPos / dDuration;
}
