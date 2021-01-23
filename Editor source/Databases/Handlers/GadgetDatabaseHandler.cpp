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

#include "GadgetDatabaseHandler.h"
#include "..\\Windows\\GadgetWnd.h"
#include "..\\DatabaseUtilities.h"
#include "..\\..\\OtE\\OnTheEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGadgetDatabaseHandler::CGadgetDatabaseHandler()
{
	m_filename = "data\\gadgets.dtb";
}

CGadgetDatabaseHandler::~CGadgetDatabaseHandler()
{
}

void CGadgetDatabaseHandler::AddNewTemplate(ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	pair<SGadgetTemplate, SCustomGadgetTemplate*> d;
	DWORD id = 1;
	vector< pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_templates->begin();
	for(; it != m_templates->end(); it++)
		if(it->first.ID >= id)
			 id = it->first.ID+1;
	CGadgetWnd w;
	if(w.OpenWindow(parent, d, TRUE) == TRUE)
	{
		d.first.ID = id;
		m_templates->push_back(d);
		UpdateSelectionList(update);
	}
}

void CGadgetDatabaseHandler::EditTemplate(long id, ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	pair<SGadgetTemplate, SCustomGadgetTemplate*> d;
	CGadgetWnd w;
	vector< pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_templates->begin();

	for(int i=0; it != m_templates->end(); ++it, ++i)
		if(it->first.ID == id)
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

void CGadgetDatabaseHandler::RemoveTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	vector< pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if(it->first.ID == id)
		{
			if(it->second != NULL)
				delete it->second;
			m_templates->erase(it);
			UpdateSelectionList(update);
			return;
		}
}

bool CGadgetDatabaseHandler::LoadDatabase()
{
	if(m_manager == NULL)
		return false;

	m_templates = m_manager->GetGadgetTemplates();

	return true;
}

bool CGadgetDatabaseHandler::SaveDatabase()
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


void CGadgetDatabaseHandler::UpdateSelectionList(ISelectionList *list)
{
	if(m_templates == NULL || list == NULL)
		return;

	list->ClearEntries();

	std::string s;

	vector< pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
	{
		if(it->first.useFlags & GADGET_USABLE_ENEMY)
			s = "Enemy gadgets";
		else if(it->first.gadgetType == GADGET_TYPE_WEAPON)
			s = "Weapons";
		else if(it->first.gadgetType == GADGET_TYPE_KINETIC)
			s = "Kinetic guns";
		else if(it->first.gadgetType == GADGET_TYPE_HOVER)
			s = "Hover engines";
		else if(it->first.gadgetType == GADGET_TYPE_SHIELD)
			s = "Shields";
		else if(it->first.gadgetType == GADGET_TYPE_STEALTH)
			s = "Stealth devices";
		else if(it->first.gadgetType == GADGET_TYPE_POSSESS)
			s = "Mind control devices";
		else if(it->first.gadgetType == GADGET_TYPE_PULLRAY)
			s = "Pull rays";
		else
			s = "Default";

		list->AddEntry(SSelectionEntry(it->first.name, it->first.ID, s));
	}

	list->UpdateList();
}

bool CGadgetDatabaseHandler::ExportTemplate(long id, HWND parent)
{
	if(m_manager == NULL)
		return false;

	std::string str;

	if(!CDatabaseUtilities::GetFullExportPath(parent, str, "org"))
	{
		MessageBox(parent, "Failed exporting template!", "Error", MB_OK);
		return false;
	}

	if(!m_manager->ExportTemplate(id, str))
	{
		MessageBox(parent, "Failed exporting template!", "Error", MB_OK);
		return false;
	}

	return true;
}

bool CGadgetDatabaseHandler::ImportTemplate(long replacesId, ISelectionList *update, HWND parent)
{
	if(m_manager == NULL)
		return false;

	std::string str;

	if(!CDatabaseUtilities::GetFullImportPath(parent, str, "org"))
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
		CPrinter::logError("Failed importing selected gadget");
		MessageBox(parent, "Failed importing selected gadget", "OtE Editor error!", MB_OK);
		return false;
	}

	UpdateSelectionList(update);

	return true;
}

void CGadgetDatabaseHandler::CloneTemplate(long id, ISelectionList *update)
{
	/*if(m_templates == NULL)
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
		}*/
}