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

#ifndef __CMENUFILEPARSER_H__
#define __CMENUFILEPARSER_H__

#include "MenuDatabase.h"

class CMenuFileParser
{
public:
	CMenuFileParser();

	bool ParseFile(std::string filename, std::vector<CMenuDatabase::SMenuData> &outData);

private:
	bool GetParams(char *command, char *param, char *line);
	bool ProcessCommand(char *command, char *param, SMenuElement &e);
	bool ProcessHeaderCommand(char *command, char *param, SMenuHeader &e);
	void BeginColor(D3DCOLOR &color);
	void EndColor();
	void EndMenu();

	int GetMenuID(char *name);
	int GetElementID(char *name);

	int m_a, m_r, m_g, m_b;
	D3DCOLOR *m_currentColor;
};

#endif // __CMENUFILEPARSER_H__