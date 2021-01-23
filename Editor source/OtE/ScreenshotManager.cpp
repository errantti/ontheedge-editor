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

#include "ScreenshotManager.h"
#include "OnTheEdge.h"

#include <atltime.h>

CScreenshotManager::CScreenshotManager() : m_folder("screenshots\\")
{
}

void CScreenshotManager::TakeScreenshot()
{
	std::string filename(m_folder);
	filename += "screenshot-";
	char tempbuf[128];

	_strtime(tempbuf);
	filename += tempbuf;
	for(int i=0; i<(int)filename.length(); ++i)
		if(filename[i] == ':')
			filename[i] = '-';
	filename += ".jpg";

	GetOTE()->TakeScreenshot(filename.c_str());
	GetOTE()->AddScreenMessage("Screenshot taken");
}

void CScreenshotManager::SetScreenshotFolder(std::string folder)
{
	m_folder = folder;
	if(m_folder.length() > 1)
		if(m_folder[m_folder.length()-1] != '\\')
			m_folder += '\\';
}