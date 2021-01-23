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

#ifndef __CWaypointSetDatabase_H__
#define __CWaypointSetDatabase_H__

#include "..\\Databases\\IDatabaseHandler.h"

#include "..\\OtE\\Level.h"

class CWaypointSetDatabase : public IDatabaseHandler
{
public:
	CWaypointSetDatabase();
	~CWaypointSetDatabase();

	virtual void AddNewTemplate(ISelectionList *update, HWND parent);
	virtual void EditTemplate(long id, ISelectionList *update, HWND parent);
	virtual void RemoveTemplate(long id, ISelectionList *update);

	virtual bool LoadDatabase();
	virtual bool SaveDatabase();

	virtual bool ExportTemplate(long id, HWND parent) {return false;}
	virtual bool ImportTemplate(long replacesId, ISelectionList *update, HWND parent) {return false;}

	virtual void CloneTemplate(long id, ISelectionList *update) {}

	virtual void UpdateSelectionList(ISelectionList *list);

	virtual void SetManager(CLevel *manager) {m_manager = manager;}

protected:
	long GetNewID();

	std::vector<CWayPointSet*> *m_templates;
	CLevel *m_manager;
};

#endif // #ifndef __CWaypointSetDatabase_H__
