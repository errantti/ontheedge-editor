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

#include "WaypointSetDatabase.h"
#include "GetStringWnd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaypointSetDatabase::CWaypointSetDatabase() : m_templates(NULL)
{
	m_filename = "data\\waypoints.dtb";
}

CWaypointSetDatabase::~CWaypointSetDatabase()
{
}

void CWaypointSetDatabase::AddNewTemplate(ISelectionList *update, HWND parent)
{
	::MessageBox(NULL, "Waypoint sets are added from Insert-menu", "OtE Editor", MB_OK);
}

void CWaypointSetDatabase::EditTemplate(long id, ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	std::string d;
	CGetStringWnd w;
	std::vector<CWayPointSet*>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if((*it)->GetID() == id)
		{
			d = (*it)->GetName();
			if(w.OpenWindow(parent, d, "Waypoint set name"))
			{
				if(d.size() < 3)
				{
					::MessageBox(parent, "Too short name", "Error", MB_OK);
					return;
				}
				(*it)->SetName(d);
				UpdateSelectionList(update);
			}
			return;
		}

	::MessageBox(NULL, "Internal error: template id not found!", "OtE Editor error!", MB_OK);
}

void CWaypointSetDatabase::RemoveTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	std::vector<CWayPointSet*>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if((*it)->GetID() == id)
		{
			delete *it;
			m_templates->erase(it);
			UpdateSelectionList(update);
			return;
		}
}

bool CWaypointSetDatabase::LoadDatabase()
{
	if(m_manager == NULL)
		return false;

	m_templates = m_manager->GetWaypointSets();

	return true;
}

bool CWaypointSetDatabase::SaveDatabase()
{
	return true;
}

void CWaypointSetDatabase::UpdateSelectionList(ISelectionList *list)
{
	if(m_templates == NULL || list == NULL)
		return;

	list->ClearEntries();

	std::string s;

	vector<CWayPointSet*>::iterator it = m_templates->begin();
	for(; it != m_templates->end(); ++it)
	{
		s = (*it)->GetName();
		list->AddEntry(SSelectionEntry(s, (*it)->GetID()));
	}

	list->UpdateList();
}