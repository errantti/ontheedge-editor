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

// Printer.cpp: implementation of the CPrinter class.
//
//////////////////////////////////////////////////////////////////////

#include "Printer.h"

ofstream CPrinter::out;
bool CPrinter::m_bErrorIDs[LOG_ID_COUNT];
bool CPrinter::m_bOpen = FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrinter::CPrinter()
{
}

CPrinter::~CPrinter()
{
}

void CPrinter::log(const char *text, LOG_ENTRY_TYPE type)
{
	if(!m_bOpen) 
	{
		OpenLog("default1log.txt");
		log("Problems opening log file");
	}
	switch(type)
	{
	case LOG_REMARK:
		out<<"Notice: ";
		break;
	case LOG_ERROR:
		out<<"Error: ";
		break;
	case LOG_FATALERROR:
		out<<"Fatal error: ";
		break;
	case LOG_WARNING:
		out<<"Warning: ";
		break;
	}
	out<<text<<"\n";
	out.flush();
}

void CPrinter::logs(const string text, LOG_ENTRY_TYPE type)
{
	if(!m_bOpen) 
	{
		OpenLog("default1log.txt");
		log("Problems opening log file");
	}
	switch(type)
	{
	case LOG_REMARK:
		out<<"Notice: ";
		break;
	case LOG_ERROR:
		out<<"Error: ";
		break;
	case LOG_FATALERROR:
		out<<"Fatal error: ";
		break;
	case LOG_WARNING:
		out<<"Warning: ";
		break;
	}
	out<<text<<"\n";
	out.flush();
}

void CPrinter::logError(const string text)
{
	if(!m_bOpen)
	{
		OpenLog("default1log.txt");
		log("Problems opening log file");
	}
	out << "Error: " << text << endl;
	out.flush();
}

void CPrinter::log(const char *text, LOG_ID ID, LOG_ENTRY_TYPE type)
{
	if(ID<0||ID>=LOG_ID_COUNT)
		return;
	if(m_bErrorIDs[ID])
		return;

	if(!m_bOpen) 
	{
		OpenLog("default1log.txt");
		log("Problems opening log file");
	}

	m_bErrorIDs[ID] = TRUE;

	switch(type)
	{
	case LOG_REMARK:
		out<<"Notice: ";
		break;
	case LOG_ERROR:
		out<<"Error: ";
		break;
	case LOG_FATALERROR:
		out<<"Fatal error: ";
		break;
	case LOG_WARNING:
		out<<"Warning: ";
		break;
	}
	out<<text<<"\n";
	out.flush();
}

void CPrinter::debug(const char *text)
{
	//OutputDebugString(text);
}

void CPrinter::OpenLog(const char *filename, const char *header)
{
	out.open(filename);
	if(!out.is_open())
		out.open("defaultlog.txt");
	if(!out.is_open())
		m_bOpen = FALSE;
	else
		m_bOpen = TRUE;
	if(header)
		out<<header<<"\n";
	else
		out<<" Edgewalker engine log file\n";
	out<<"---------------------------\n";
	out.flush();
	memset(&m_bErrorIDs, 0, LOG_ID_COUNT);
}