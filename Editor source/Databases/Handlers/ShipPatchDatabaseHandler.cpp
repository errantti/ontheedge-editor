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

#include "ShipPatchDatabaseHandler.h"
#include "..\\Windows\\ShipPatchWnd.h"
#include "..\\..\\OtE\\OnTheEdge.h"
#include "..\\DatabaseUtilities.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShipPatchDatabaseHandler::CShipPatchDatabaseHandler() : m_templates(NULL), m_manager(NULL)
{
	m_filename = "data\\items.dtb";
}

CShipPatchDatabaseHandler::~CShipPatchDatabaseHandler()
{
}

void CShipPatchDatabaseHandler::AddNewTemplate(ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	SShipPatchTemplate d;
	d.FillDefaults();
	long id = 1;
	vector<SShipPatchTemplate>::iterator it = m_templates->begin();
	for(; it != m_templates->end(); ++it)
		if(it->id >= id)
			id = it->id +1;

	CShipPatchWnd w;
	if(w.OpenWindow(parent, d, TRUE) == TRUE)
	{
		d.id = id;
		m_templates->push_back(d);
		UpdateSelectionList(update);
	}
}

void CShipPatchDatabaseHandler::EditTemplate(long id, ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	SShipPatchTemplate d;
	CShipPatchWnd w;

	vector<SShipPatchTemplate>::iterator it = m_templates->begin();
	for(int i=0; it != m_templates->end(); ++it, ++i)
		if(it->id == id)
		{
			d = *it;
			if(w.OpenWindow(parent, d, FALSE))
			{
				(*m_templates)[i] = d;
				UpdateSelectionList(update);
			}
			return;
		}

	::MessageBox(NULL, "Internal error: template id not found!", "OtE Editor error!", MB_OK);
}

void CShipPatchDatabaseHandler::RemoveTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	std::vector<SShipPatchTemplate>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if(it->id == id)
		{
			m_templates->erase(it);
			UpdateSelectionList(update);
			return;
		}
}

bool CShipPatchDatabaseHandler::LoadDatabase()
{
	if(m_manager == NULL)
		return false;

	m_templates = &m_manager->GetPatchTemplates();

	return true;
}

bool CShipPatchDatabaseHandler::SaveDatabase()
{
	if(m_manager == NULL)
		return false;

	std::string filename(GetOTE()->GetCurrentCampaignFolder());
	filename += m_filename;

	if(m_manager->SaveDatabase(filename.c_str()))
	{
		::MessageBox(NULL, "Database saved succesfully!", "OtE Editor", MB_OK);
		return true;
	}

	::MessageBox(NULL, "Failed saving database!", "OtE Editor error", MB_OK);

	return false;
}


void CShipPatchDatabaseHandler::UpdateSelectionList(ISelectionList *list)
{
	if(m_templates == NULL || list == NULL)
		return;

	list->ClearEntries();

	std::string s;

	std::vector<SShipPatchTemplate>::iterator it = m_templates->begin();
	for(; it != m_templates->end(); it++)
		list->AddEntry(SSelectionEntry(it->name, it->id));

	list->UpdateList();
}

bool CShipPatchDatabaseHandler::ExportTemplate(long id, HWND parent)
{
	if(m_manager == NULL)
		return false;

	std::string str;

	if(!CDatabaseUtilities::GetFullExportPath(parent, str, "orsp"))
	{
		MessageBox(parent, "Failed exporting template!", "Error", MB_OK);
		return false;
	}

	if(!m_manager->ExportShipPatchTemplate(id, str))
		return false;

	return true;
}

bool CShipPatchDatabaseHandler::ImportTemplate(long replacesId, ISelectionList *update, HWND parent)
{
	if(m_manager == NULL)
		return false;

	std::string str;

	if(!CDatabaseUtilities::GetFullImportPath(parent, str, "orsp"))
		return false;

	if(replacesId > 0)
	{
		if(MessageBox(parent, "Do you want the imported object to replace the selected one? Answering No will create a new object.", 
			"OtE Editor", MB_YESNO)	== IDNO)
		{
			replacesId = -1;
		}
	}

	if(!m_manager->ImportShipPatchTemplate(replacesId, str))
	{
		CPrinter::logError("Failed importing selected ship patch");
		MessageBox(parent, "Failed importing selected ship patch", "OtE Editor error!", MB_OK);
		return false;
	}

	UpdateSelectionList(update);

	return true;
}