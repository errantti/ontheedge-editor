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

#ifndef __CPRINTER_H__
#define __CPRINTER_H__

#include <fstream>
#include <windows.h>
#include <string>
using namespace std;

enum LOG_ENTRY_TYPE {
	LOG_REMARK,
	LOG_ERROR,
	LOG_FATALERROR,
	LOG_WARNING,
	LOG_UNDEFINED
};

enum LOG_ID {
	LOG_ID_PRESENT_FAILED=0,
	LOG_ID_BEGINSCENE_FAILED,
	LOG_ID_ENDSCENE_FAILED,
	LOG_ID_BEGINSPRITE_FAILED,
	LOG_ID_ENDSPRITE_FAILED,
	LOG_ID_DXCLEAR_FAILED,
	LOG_ID_D3DDEVICELOST,
	LOG_ID_D3DDEVICERESTOREFAILED,
	LOG_ID_BEGINSHADER_FAILED,
	LOG_ID_ENDSHADER_FAILED,
	LOG_ID_BEGINSHADERPASS_FAILED,
	LOG_ID_ENDSHADERPASS_FAILED,
	LOG_ID_VERTEXDECLARATION_FAILED,
	LOG_ID_SETSHADERTECHNIQUE_FAILED,
	LOG_ID_COUNT
};

class CPrinter
{
public:
	static void log(const char *text, LOG_ENTRY_TYPE type = LOG_REMARK);	// Writes a line of text to the log
	static void logs(const string text, LOG_ENTRY_TYPE type = LOG_REMARK);	// Writes a line of text to the log

	static void logError(const string text);

	// Writes a line of text to the log unless the ID has already been 'used'
	// This is reserved for error messages that would usually happen more than
	// once if they happen at all (filling the log file with identical error
	// messages)
	static void log(const char *text, LOG_ID ID, LOG_ENTRY_TYPE type = LOG_ERROR);

	static void debug(const char *text);									// Outputs a debug comment

	static void OpenLog(const char *filename, const char *header = NULL);

protected:
	CPrinter();
	~CPrinter();

	static ofstream out;
	static bool m_bErrorIDs[LOG_ID_COUNT];
	static bool m_bOpen;
};

#endif // #ifndef __CPRINTER_H__
