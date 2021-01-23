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

#include "LightEffectDatabaseHandler.h"
#include "..\\Windows\\LightEffectWnd.h"
#include "..\\DatabaseUtilities.h"
#include "..\\..\\OtE\\OnTheEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightEffectDatabaseHandler::CLightEffectDatabaseHandler() : m_templates(NULL), m_manager(NULL)
{
	m_filename = "data\\lighteffects.dtb";
}

CLightEffectDatabaseHandler::~CLightEffectDatabaseHandler()
{
}

void CLightEffectDatabaseHandler::AddNewTemplate(ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	SDynamicPointLightParams d;
	d.FillDefaults();
	DWORD id = 1;
	for(DWORD i=0;i<m_templates->size();i++)
		if((*m_templates)[i].id >= id)
			 id = (*m_templates)[i].id+1;
	CLightEffectWnd w;
	if(w.OpenWindow(parent, d, TRUE) == TRUE)
	{
		d.id = id;
		m_templates->push_back(d);
		UpdateSelectionList(update);
	}
}

void CLightEffectDatabaseHandler::EditTemplate(long id, ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	SDynamicPointLightParams d;
	CLightEffectWnd w;
	for(int i=0;i<m_templates->size();i++)
		if((*m_templates)[i].id == id)
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

void CLightEffectDatabaseHandler::RemoveTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	std::vector<SDynamicPointLightParams>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if(it->id == id)
		{
			m_templates->erase(it);
			UpdateSelectionList(update);
			return;
		}
}

bool CLightEffectDatabaseHandler::LoadDatabase()
{
	if(m_manager == NULL)
		return false;

	m_templates = &m_manager->GetEffects();

	return true;
}

bool CLightEffectDatabaseHandler::SaveDatabase()
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


void CLightEffectDatabaseHandler::UpdateSelectionList(ISelectionList *list)
{
	if(m_templates == NULL || list == NULL)
		return;

	list->ClearEntries();

	std::string s;

	for(int i=0;i<m_templates->size();i++)
	{
		s = (*m_templates)[i].name;
		list->AddEntry(SSelectionEntry(s, (*m_templates)[i].id));
	}

	list->UpdateList();
}

bool CLightEffectDatabaseHandler::ExportTemplate(long id, HWND parent)
{
	if(m_manager == NULL)
		return false;

	std::string str;

	if(!CDatabaseUtilities::GetFullExportPath(parent, str, "orl"))
	{
		MessageBox(parent, "Failed exporting effect!", "Error", MB_OK);
		return false;
	}

	if(!m_manager->ExportTemplate(id, str))
		return false;

	return true;
}

bool CLightEffectDatabaseHandler::ImportTemplate(long replacesId, ISelectionList *update, HWND parent)
{
	if(m_manager == NULL)
		return false;

	std::string str;

	if(!CDatabaseUtilities::GetFullImportPath(parent, str, "orl"))
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
		CPrinter::logError("Failed importing selected effect");
		MessageBox(parent, "Failed importing selected effect", "OtE Editor error!", MB_OK);
		return false;
	}

	UpdateSelectionList(update);

	return true;
}

void CLightEffectDatabaseHandler::CloneTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	std::vector<SDynamicPointLightParams>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if(it->id == id)
		{
			SDynamicPointLightParams t(*it);
			t.id = m_manager->GetNewID();
			m_templates->push_back(t);
			UpdateSelectionList(update);
			return;
		}
}