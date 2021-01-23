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

#ifndef __IDATABASEHANDLER_H__
#define __IDATABASEHANDLER_H__

#include "ISelectionList.h"
#include "..\\OtE\\Objects\\ObjectManager.h"
#include <string>

class IDatabaseHandler
{
public:
	IDatabaseHandler() : m_bSaved(TRUE), m_manager(NULL) {}

	virtual void AddNewTemplate(ISelectionList *update, HWND parent) = 0;
	virtual void EditTemplate(long id, ISelectionList *update, HWND parent) = 0;
	virtual void RemoveTemplate(long id, ISelectionList *update) = 0;

	virtual bool LoadDatabase() = 0;
	virtual bool SaveDatabase() = 0;

	virtual bool ExportTemplate(long id, HWND parent) = 0;
	virtual bool ImportTemplate(long replacesId, ISelectionList *update, HWND parent) = 0;

	virtual void CloneTemplate(long id, ISelectionList *update) = 0;

	virtual void UpdateSelectionList(ISelectionList *list) = 0;

	virtual void SetManager(CObjectManager *o) {m_manager = o;}

protected:
	std::string m_filename;
	bool m_bSaved;

	CObjectManager *m_manager;
};

#endif