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

#include "ParticleEffectDatabase.h"
#include "..\\Windows\\ParticleEffectWnd.h"
#include "..\\DatabaseUtilities.h"
#include "..\\..\\OtE\\OnTheEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParticleEffectDatabase::CParticleEffectDatabase()
{
	m_filename = "data\\peffects.dtb";
}

CParticleEffectDatabase::~CParticleEffectDatabase()
{
}

void CParticleEffectDatabase::AddNewTemplate(ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	SParticleSystemInfo d;
	d.FillDefaults();

	CParticleEffectWnd w;
	if(w.OpenWindow(parent, d, TRUE) == TRUE)
	{
		d.ID = GetNewID();
		m_templates->push_back(d);
		UpdateSelectionList(update);
	}
}

void CParticleEffectDatabase::EditTemplate(long id, ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	SParticleSystemInfo d;
	CParticleEffectWnd w;
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

void CParticleEffectDatabase::RemoveTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	std::vector<SParticleSystemInfo>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if(it->ID == id)
		{
			m_templates->erase(it);
			UpdateSelectionList(update);
			return;
		}
}

bool CParticleEffectDatabase::LoadDatabase()
{
	if(m_manager == NULL)
		return false;

	m_templates = m_manager->GetEffects();

	return true;
}

bool CParticleEffectDatabase::SaveDatabase()
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


void CParticleEffectDatabase::UpdateSelectionList(ISelectionList *list)
{
	if(m_templates == NULL || list == NULL)
		return;

	list->ClearEntries();

	std::string s;

	for(int i=0;i<m_templates->size();i++)
	{
		//s = (*m_templates)[i].ID + " - ";
		s = (*m_templates)[i].name;
		list->AddEntry(SSelectionEntry(s, (*m_templates)[i].ID));
	}

	list->UpdateList();
}

long CParticleEffectDatabase::GetNewID()
{
	long id = 1;
	std::vector<SParticleSystemInfo>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); ++it)
		if(it->ID >= id)
			 id = it->ID + 1;
	return id;
}

bool CParticleEffectDatabase::ExportTemplate(long id, HWND parent)
{
	if(m_manager == NULL)
		return false;

	std::string str;

	if(!CDatabaseUtilities::GetFullExportPath(parent, str, "orpe"))
	{
		MessageBox(parent, "Failed exporting effect!", "Error", MB_OK);
		return false;
	}

	if(!m_manager->ExportTemplate(id, str))
		return false;

	MessageBox(parent, "Succesfully exported effect!", "OtE Editor", MB_OK);

	return true;
}

bool CParticleEffectDatabase::ImportTemplate(long replacesId, ISelectionList *update, HWND parent)
{
	if(m_manager == NULL)
		return false;

	std::string str;

	if(!CDatabaseUtilities::GetFullImportPath(parent, str, "orpe"))
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

	MessageBox(parent, "Succesfully imported effect!", "OtE Editor", MB_OK);

	return true;
}

void CParticleEffectDatabase::CloneTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	std::vector<SParticleSystemInfo>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if(it->ID == id)
		{
			SParticleSystemInfo t(*it);
			t.ID = m_manager->GetNewID();
			//t.params = it->params.clo
			m_templates->push_back(t);
			UpdateSelectionList(update);
			return;
		}
}