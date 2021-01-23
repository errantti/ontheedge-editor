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

// Timer.cpp: implementation of the CTimer class.
//
//////////////////////////////////////////////////////////////////////

#include "Timer.h"
#include "Printer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimer::CTimer() : m_startTicks(0), m_stopTicks(0), m_fps(0.f), m_bRunning(false), m_lastUpdate(0),
m_totalTime(0)
{
	if(!::QueryPerformanceFrequency((LARGE_INTEGER*)&m_tickFrequency))
		m_tickFrequency = 1000;
}

/**
 * Returns time elapsed from starting the timer
 **/
double CTimer::GetTime()
{
	INT64 ticks;

	if(m_bRunning)
	{
		if(!QueryPerformanceCounter((LARGE_INTEGER*)&ticks))
			ticks = (INT64)timeGetTime();
	}
	else
		ticks = m_stopTicks;

	ticks -= m_startTicks;

	return (double)ticks / (double)m_tickFrequency;
}

/**
 * Starts the timer
 **/
void CTimer::Start()
{
	if(!QueryPerformanceCounter((LARGE_INTEGER*)&m_startTicks))
		m_startTicks = (INT64)timeGetTime();

	m_bRunning = true;

	m_numFrames = 0;
	m_lastUpdate = 0;
	m_frameTime = 0;
	m_fFrameTime = 0.f;
	m_totalTime = 0;
}

/**
 * Stops the timer
 **/
void CTimer::Stop()
{
	if(m_bRunning)
	{
		if(!QueryPerformanceCounter((LARGE_INTEGER*)&m_stopTicks))
			m_stopTicks = (INT64)timeGetTime();
		m_bRunning = false;
	}
}

void CTimer::Continue()
{
	if(!m_bRunning)
	{
		INT64 ticks;

		if(!QueryPerformanceCounter((LARGE_INTEGER*)&ticks))
			ticks = (INT64)timeGetTime();

		// Don't count in the time that the timer was paused
		m_startTicks += ticks - m_stopTicks;

		m_bRunning = true;
	}
}

void CTimer::ReportFrame()
{
	m_frameTime = GetTime() - m_totalTime;
	m_fFrameTime = (float)m_frameTime;
	m_totalTime += m_frameTime;
	++m_numFrames;

	if(m_totalTime - m_lastUpdate > FPS_INTERVAL)
	{
		m_fps = (float)m_numFrames / (float)(m_totalTime - m_lastUpdate);
		m_numFrames = 0;
		m_lastUpdate = m_totalTime;
	}
}