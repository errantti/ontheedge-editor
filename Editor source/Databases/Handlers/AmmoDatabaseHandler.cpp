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

#include "AmmoDatabaseHandler.h"
#include "..\\Windows\\AmmoWnd.h"
#include "..\\DatabaseUtilities.h"
#include "..\\..\\OtE\\OnTheEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAmmoDatabaseHandler::CAmmoDatabaseHandler() : m_templates(NULL), m_manager(NULL)
{
	m_filename = "data\\ammos.dtb";
}

CAmmoDatabaseHandler::~CAmmoDatabaseHandler()
{
}

void CAmmoDatabaseHandler::AddNewTemplate(ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	SAmmoTemplate d;
	d.FillDefaults();
	d.particleEffectID = -1;

	CAmmoWnd w;
	if(w.OpenWindow(parent, d, TRUE) == TRUE)
	{
		d.ID = m_manager->GetNewID();
		m_templates->push_back(d);
		UpdateSelectionList(update);
	}
}

void CAmmoDatabaseHandler::EditTemplate(long id, ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	SAmmoTemplate d;
	CAmmoWnd w;
	for(int i=0;i<m_templates->size();i++)
		if((*m_templates)[i].ID == id)
		{
			d = (*m_templates)[i];
			if(w.OpenWindow(parent, d, FALSE))
			{
				(*m_templates)[i] = d;
				UpdateSelectionList(update);
			}
			return;
		}

	::MessageBox(NULL, "Internal error: template id not found!", "OtE Editor error!", MB_OK);
}

void CAmmoDatabaseHandler::RemoveTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	std::vector<SAmmoTemplate>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if(it->ID == id)
		{
			m_templates->erase(it);
			UpdateSelectionList(update);
			return;
		}
}

bool CAmmoDatabaseHandler::LoadDatabase()
{
	if(m_manager == NULL)
		return false;

	m_templates = m_manager->GetAmmoTemplates();

	return true;
}

bool CAmmoDatabaseHandler::SaveDatabase()
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


void CAmmoDatabaseHandler::UpdateSelectionList(ISelectionList *list)
{
	if(m_templates == NULL || list == NULL)
		return;

	list->ClearEntries();

	std::string s;

	for(int i=0;i<m_templates->size();i++)
	{
		s = (*m_templates)[i].name;
		list->AddEntry(SSelectionEntry(s, (*m_templates)[i].ID));
	}

	list->UpdateList();
}

bool CAmmoDatabaseHandler::ExportTemplate(long id, HWND parent)
{
	if(m_manager == NULL)
		return false;

	std::string str;

	if(!CDatabaseUtilities::GetFullExportPath(parent, str, "ora"))
	{
		MessageBox(parent, "Failed exporting template!", "Error", MB_OK);
		return false;
	}

	if(!m_manager->ExportTemplate(id, str))
		return false;

	return true;
}

bool CAmmoDatabaseHandler::ImportTemplate(long replacesId, ISelectionList *update, HWND parent)
{
	if(m_manager == NULL)
		return false;

	std::string str;

	if(!CDatabaseUtilities::GetFullImportPath(parent, str, "ora"))
		return false;

	if(replacesId > 0)
	{
		if(MessageBox(parent, "Do you want the imported object to replace the selected one? Answering No will create a new object.", 
			"OtE Editor", MB_YESNO)	== IDNO)
		{
			replacesId = -1;
		}
	}

	if(!m_manager->ImportTemplate(replacesId, str))
	{
		CPrinter::logError("Failed importing selected object");
		MessageBox(parent, "Failed importing selected object", "OtE Editor error!", MB_OK);
		return false;
	}

	UpdateSelectionList(update);

	return true;
}

void CAmmoDatabaseHandler::CloneTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	std::vector<SAmmoTemplate>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if(it->ID == id)
		{
			SAmmoTemplate t(*it);
			t.ID = m_manager->GetNewID();
			m_templates->push_back(t);
			UpdateSelectionList(update);
			return;
		}
}