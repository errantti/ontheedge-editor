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

#ifndef __CTIMER_H_
#define __CTIMER_H_

#include <windows.h>

#define FPS_INTERVAL	0.5

class CTimer
{
protected:
	INT64 m_startTicks;
	INT64 m_stopTicks;
	INT64 m_tickFrequency;

	double m_totalTime;

	/// Length of last frame, double-precision
	double m_frameTime;

	/// Length of last frame
	float m_fFrameTime;

	float m_fps;
	double m_lastUpdate;
	DWORD m_numFrames;

	bool m_bRunning;

public:
	CTimer();

	double GetTime();
	inline double GetTotalTime() const {return m_totalTime;}

	inline double GetFrameTime() const {return m_frameTime;}
	inline float GetFrameTimeF() const {return m_fFrameTime;}

	inline float GetFPS() const {return m_fps;}

	inline bool IsRunning() const {return m_bRunning;}

	void Stop();
	void Start();
	void Continue();

	/**
	 * Should be called once every frame to update fps,
	 * frametime and total time correctly
	 */
	void ReportFrame();
};

#endif // #ifndef __CTIMER_H_
