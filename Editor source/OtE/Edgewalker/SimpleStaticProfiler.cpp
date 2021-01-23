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

#include "SimpleStaticProfiler.h"
#include <fstream>
#include <sstream>

bool CSimpleStaticProfiler::ProfilingEnabled = false;

using namespace std;

CSimpleStaticProfiler::CSimpleStaticProfiler(string outputFile)
: m_outputFilename(outputFile), m_shortestTake(1000), m_longestTake(0),
m_avarageTake(0), m_numTakes(0), m_custom(0)
{
	::QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
}

CSimpleStaticProfiler::~CSimpleStaticProfiler()
{
	ofstream out(m_outputFilename.c_str());
	if(!out.is_open())
		return;

	out << "Number of takes " << m_numTakes << endl;
	out << "Longest take " << m_longestTake << endl;
	out << "Shortest take " << m_shortestTake << endl;
	out << "Avarage take " << m_avarageTake << endl;
	out << "Custom " << m_custom << endl;

	out.close();
}

void CSimpleStaticProfiler::StartTiming()
{
	if(!ProfilingEnabled)
		return;

	::QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);
}

void CSimpleStaticProfiler::StopTiming()
{
	if(!ProfilingEnabled)
		return;

	INT64 endTime;
	double dTime;

	::QueryPerformanceCounter((LARGE_INTEGER*)&endTime);

	dTime = (double)(endTime - m_startTime);
	double seconds = dTime / (double)m_frequency;

	if(seconds > m_longestTake)
		m_longestTake = seconds;
	if(seconds < m_shortestTake)
		m_shortestTake = seconds;

	m_avarageTake = ((double)m_numTakes * m_avarageTake + seconds) / (double)(m_numTakes + 1);

	++m_numTakes;
}

void CSimpleStaticProfiler::IncrementCustom(double amount)
{
	if(!ProfilingEnabled)
		return;
	m_custom += amount;
}

void CSimpleStaticProfiler::CustomMax(double value)
{
	if(!ProfilingEnabled)
		return;

	if(value > m_custom)
		m_custom = value;
}