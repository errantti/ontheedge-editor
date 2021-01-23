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

#ifndef __CSIMPLESTATICPROFILER_H__
#define __CSIMPLESTATICPROFILER_H__

#include <string>
#include <windows.h>

/**
 * Extremely simple profiler class. When it is instanced
 * it is given file into which the results are written.
 * Call StartTiming and StopTiming to time a segment.
 * When the instance is destroyed the results are automatically
 * written into the given file.
 */
class CSimpleStaticProfiler
{
public:
	CSimpleStaticProfiler(std::string outputFile);
	~CSimpleStaticProfiler();

	void StartTiming();
	void StopTiming();

	void IncrementCustom(double amount);
	void CustomMax(double value);

	static bool ProfilingEnabled;

	inline double GetLongestTake() const {return m_longestTake;}
	inline double GetAvarageTake() const {return m_avarageTake;}
	inline double GetShortestTake() const {return m_shortestTake;}

private:
	std::string m_outputFilename;

	INT64 m_startTime;
	INT64 m_frequency;

	long m_numTakes;
	double m_custom;
	double m_longestTake;
	double m_avarageTake;
	double m_shortestTake;
};

#endif // __CSIMPLESTATICPROFILER_H__