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

#include "AssistantMsgDatabase.h"
#include "AssistantMsgWnd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAssistantMsgHandler::CAssistantMsgHandler() : m_templates(NULL), m_manager(NULL)
{
}

CAssistantMsgHandler::~CAssistantMsgHandler()
{
}

void CAssistantMsgHandler::AddNewTemplate(ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	DWORD id = 1;
	std::vector<std::pair<int,std::string> >::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if((DWORD)it->first >= id)
			 id = it->first+1;
	CAssistantMsgWnd w;
	std::pair<int,std::string> d;
	if(w.OpenWindow(parent, d, TRUE) == TRUE)
	{
		d.first = id;
		m_templates->push_back(d);
		UpdateSelectionList(update);
	}
}

void CAssistantMsgHandler::EditTemplate(long id, ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	std::pair<int,std::string> d;
	CAssistantMsgWnd w;

	for(int i=0;i<(int)m_templates->size();i++)
		if((*m_templates)[i].first == id)
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

void CAssistantMsgHandler::RemoveTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	std::vector<std::pair<int,std::string> >::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if(it->first == id)
		{
			m_templates->erase(it);
			UpdateSelectionList(update);
			return;
		}
}

bool CAssistantMsgHandler::LoadDatabase()
{
	if(m_manager == NULL)
		return false;

	m_templates = m_manager->GetAssistentMessages();

	return true;
}

bool CAssistantMsgHandler::SaveDatabase()
{
	return true;
}

void CAssistantMsgHandler::UpdateSelectionList(ISelectionList *list)
{
	if(m_templates == NULL || list == NULL)
		return;

	list->ClearEntries();

	std::string s;
	int size;

	std::vector<std::pair<int,std::string> >::iterator it = m_templates->begin();
	for(; it != m_templates->end(); ++it)
	{
		size = (int)it->second.size();
		if(size < 40)
			s = it->second;
		else
		{
			s = it->second.substr(0, 37);
			s += "...";
		}
		list->AddEntry(SSelectionEntry(s, it->first));
	}

	list->UpdateList();
}