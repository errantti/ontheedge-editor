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

#include "..\\OnTheEdge.h"
#include "..\\menus\\DialogueWindow.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialogue::CDialogue() : m_state(DIALOGUE_STATE_FADE_IN), m_bAlreadyShown(FALSE), m_onDialogueEnd(NULL),
	m_bSkippable(true), m_missionPriority(0), m_bUpdateMissionObjective(false),
	m_dialogueWnd(NULL), m_bOpen(false)
{
	m_dialogueEndScriptName[0] = '\0';
}

CDialogue::~CDialogue()
{
}

void CDialogue::OpenDialogue()
{
	/*if(GetOTE()->GetSettings()->GetSetting(GO_SHOW_DIALOGUES_ALWAYS) && m_bAlreadyShown)
		return;

	if(!GetOTE()->GetSettings()->GetSetting(GO_SHOW_DIALOGUES))
		return;*/

	ForceOpenDialogue();
}

void CDialogue::ForceOpenDialogue()
{
	if(m_comments.size() == 0)
		return;

	CDialogueWindow::SetDialoguePointer(this);
	m_bOpen = true;
	m_currentComment = -1;
	GetOTE()->GetMenuManager()->OpenMenu(MENU_ID_DIALOGUE);
}

void CDialogue::CloseDialogue()
{
	if(!m_bSkippable)
		return;

	ForceCloseDialogue();
}

void CDialogue::ForceCloseDialogue()
{
	if(!m_bOpen)
		return;

	m_dialogueWnd->CloseView();
}

void CDialogue::NextComment()
{
	if(!m_bOpen)
		return;

	++m_currentComment;
	if(m_currentComment >= m_comments.size())
		ForceCloseDialogue();
	else
	{
		if(m_dialogueWnd != NULL)
		{
			m_dialogueWnd->UpdateDialogueComment(m_comments[m_currentComment].comment, m_comments[m_currentComment].speaker);
		}
	}
}

void CDialogue::UpdateOnEndScriptReference()
{
	m_onDialogueEnd = CScriptManager::GetManager().GetScript(m_dialogueEndScriptName);
}

bool CDialogue::ReadDialogueInfoFromStream(fstream &f)
{
	int commentCount;
	char szTemp[256];

	// Number of comments in dialogue
	f.read((char*)&commentCount, sizeof(int));

	if(commentCount > 1000 || commentCount < 0)
	{
		CPrinter::logError("Corrupted dialogue data");
		return false;
	}

	// ID
	f.read((char*)&m_ID, sizeof(long));

	// Dialogue name
	f.read((char*)szTemp, sizeof(char)*32);
	szTemp[31] = '\0';
	m_name = szTemp;

	// On dialogue end -script name
	f.read((char*)m_dialogueEndScriptName, sizeof(char)*32);

	// Is the dialogue skippable
	f.read((char*)&m_bSkippable, sizeof(bool));

	// Does the dialogue update mission objective
	f.read((char*)&m_bUpdateMissionObjective, sizeof(bool));

	// Mission object priority
	f.read((char*)&m_missionPriority, sizeof(int));

	// The updated mission object
	f.read((char*)szTemp, sizeof(char)*MAX_MISSION_OBJECTIVE_LENGTH);
	szTemp[MAX_MISSION_OBJECTIVE_LENGTH-1] = '\0';
	m_newMissionObjective = szTemp;

	m_comments.clear();
	SComment c;
	for(int i=0; i<commentCount; ++i)
	{
		f.read((char*)&c, sizeof(SComment));
		m_comments.push_back(c);
	}

	return true;
}

void CDialogue::WriteDialogueInfoIntoStream(fstream &f)
{
	int commentCount = (int)m_comments.size();
	char szTemp[256];

	// Number of comments in dialogue
	f.write((const char*)&commentCount, sizeof(int));
	// ID
	f.write((const char*)&m_ID, sizeof(long));

	// Dialogue name
	strncpy(szTemp, m_name.c_str(), 31);
	f.write((const char*)szTemp, sizeof(char)*32);

	// On dialogue end -script name
	f.write((const char*)m_dialogueEndScriptName, sizeof(char)*32);

	// Is the dialogue skippable
	f.write((const char*)&m_bSkippable, sizeof(bool));

	// Does the dialogue update mission objective
	f.write((const char*)&m_bUpdateMissionObjective, sizeof(bool));

	// Mission object priority
	f.write((const char*)&m_missionPriority, sizeof(int));

	// The updated mission object
	strncpy(szTemp, m_newMissionObjective.c_str(), MAX_MISSION_OBJECTIVE_LENGTH-1);
	f.write((const char*)szTemp, sizeof(char)*MAX_MISSION_OBJECTIVE_LENGTH);

	vector<CDialogue::SComment>::iterator it = m_comments.begin();
	for(; it != m_comments.end(); ++it)
	{
		f.write((const char*)&(*it), sizeof(SComment));
	}
}

void CDialogue::ReportDialogueWindowClosed()
{
	// On dialogue end script
	m_bOpen = false;
	if(m_onDialogueEnd)
		CScriptManager::Execute(m_onDialogueEnd, SCRIPT_CALLER_DIALOGUE, 0, NULL);

	// Update mission objective
	if(m_bUpdateMissionObjective)
	{
		GetOTE()->GetCampaign()->SetMissionObjective(m_newMissionObjective, m_missionPriority);
		GetOTE()->AddScreenMessage("Mission objective updated");
	}
}