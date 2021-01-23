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

#ifndef __ISELECTIONLIST_H__
#define __ISELECTIONLIST_H__

#include <vector>

struct SSelectionEntry
{
	char name[64];
	char category[64];
	long id;

	SSelectionEntry() : id(0)
	{
		name[0] = '\0';
		category[0] = '\0';
	}

	SSelectionEntry(std::string s, long pid) : id(pid)
	{
		strncpy(name, s.c_str(), 63);
	}

	SSelectionEntry(std::string s, long pid, std::string strCategory) : id(pid)
	{
		strncpy(name, s.c_str(), 63);
		strncpy(category, strCategory.c_str(), 63);
		name[63] = '\0';
	}

	bool operator < (const SSelectionEntry &other) const
	{
		if(strcmp(other.category, category) < 0)
			return false;
		return true;
	}
};


class ISelectionList
{
public:
	virtual inline void AddEntry(SSelectionEntry e) = 0;
	virtual inline void ClearEntries() = 0;
	virtual inline void SetEntries(std::vector<SSelectionEntry> &entries) = 0;

	virtual void UpdateList() = 0;
};

#endif