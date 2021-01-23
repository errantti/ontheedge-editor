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

#include "GlobalScriptsHandler.h"
#include "..\\Windows\\ScriptWnd.h"
#include "..\\..\\OtE\\OnTheEdge.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlobalScriptsHandler::CGlobalScriptsHandler() : m_templates(NULL), m_manager(NULL), m_bLocalScripts(false)
{
	m_filename = "data\\globalScripts.dtb";
}

CGlobalScriptsHandler::~CGlobalScriptsHandler()
{
}

void CGlobalScriptsHandler::AddNewTemplate(ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	CScript *s = new CScript(&(CScriptManager::GetManager().GetScriptEngine()));
	s->SetIsTemporary(false);
	if(m_bLocalScripts)
		s->SetIsTemporary(true);
	CScriptWnd w;

	if(w.OpenWindow(parent, *s, TRUE) == TRUE)
	{
		m_templates->push_back(s);
		UpdateSelectionList(update);
	}
	else
		delete s;
}

void CGlobalScriptsHandler::EditTemplate(long id, ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	CScript *s = GetScriptByIndex(id);
	if(s == NULL)
		return;

	CScriptWnd w;

	std::vector<CScript*>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if(*it == s)
		{
			if(w.OpenWindow(parent, *s, FALSE))
			{
				//(*m_templates)[i] = d;
				UpdateSelectionList(update);
			}
			return;
		}

	::MessageBox(NULL, "Internal error: script by given name not found!", "OtE Editor error!", MB_OK);
}

void CGlobalScriptsHandler::RemoveTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	CScript *s = GetScriptByIndex(id);
	if(s == NULL)
		return;

	std::vector<CScript*>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if(*it == s)
		{
			delete *it;
			m_templates->erase(it);
			UpdateSelectionList(update);
			return;
		}
}

bool CGlobalScriptsHandler::LoadDatabase()
{
	if(m_manager == NULL)
		return false;

	//m_templates = m_manager->GetAmmoTemplates();
	m_templates = &(m_manager->GetScriptEngine().GetCurrentScriptCollection());

	return true;
}

bool CGlobalScriptsHandler::SaveDatabase()
{
	if(m_manager == NULL || m_bLocalScripts)
		return false;

	std::string filename(GetOTE()->GetCurrentCampaignFolder());
	filename += m_filename;

	if(m_manager->SaveScriptDatabase(filename.c_str(), false, false))
	{
		::MessageBox(NULL, "Global scripts saved succesfully!", "OtE Editor", MB_OK);
		return true;
	}

	::MessageBox(NULL, "Failed saving global scripts!", "OtE Editor error", MB_OK);

	return false;
}


void CGlobalScriptsHandler::UpdateSelectionList(ISelectionList *list)
{
	if(m_templates == NULL || list == NULL)
		return;

	list->ClearEntries();

	vector<CScript*>::iterator it = m_templates->begin();
	for(long i=0; it != m_templates->end(); ++it)
	{
		if((*it)->IsTemporary() == m_bLocalScripts)
		{
			list->AddEntry(SSelectionEntry((*it)->GetName(), i));
			++i;
		}
	}

	list->UpdateList();
}

CScript *CGlobalScriptsHandler::GetScriptByIndex(long index)
{
	if(index < 0)
		return NULL;
	vector<CScript*>::iterator it = m_templates->begin();
	for(long i=0; it != m_templates->end(); ++it)
		if((*it)->IsTemporary() == m_bLocalScripts)
		{
			if(i==index)
				return *it;
			++i;
		}
	return NULL;
}