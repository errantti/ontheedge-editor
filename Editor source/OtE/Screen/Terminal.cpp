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

#include "Terminal.h"
#include "Dialogue.h"
#include "..\\OtEScript\\ScriptManager.h"
#include "..\\Edgewalker\\Printer.h"
#include "..\\menus\\TerminalView.h"
#include "..\\OnTheEdge.h"

using namespace std;

CTerminal *CTerminal::m_currentlyOpenTerminal = NULL;

CTerminal::CTerminal() : m_ID(-1), m_activeCloseCommand(NULL),
m_bOpen(false), m_terminalWnd(NULL), m_logMsgCount(0)
{
}

CTerminal::~CTerminal()
{
	if(m_currentlyOpenTerminal == this)
		m_currentlyOpenTerminal = NULL;
}

void CTerminal::FillDefaults()
{
	m_name = "";
	m_welcomeMessage = "";
	m_disfunctionalMessage = "The terminal isn't functional";

	m_onCloseTemplate.FillDefaults();

	for(int i=0; i<COMMANDS_PER_TERMINAL; ++i)
		m_commandTemplates[i].FillDefaults();
}

void CTerminal::OpenTerminal()
{
	if(!m_bFunctional || m_bOpen)
		return;

	ForceOpenTerminal();
}

void CTerminal::ForceOpenTerminal()
{
	CTerminalView::SetTerminalPointer(this);
	GetOTE()->GetMenuManager()->OpenMenu(MENU_ID_TERMINAL);
	m_bOpen = true;
	m_logMsgCount = 0;
	m_activeCloseCommand = NULL;
	m_currentlyOpenTerminal = this;
}

void CTerminal::CloseTerminal()
{
	m_activeCloseCommand = &m_onClose;
	ForceCloseTerminal();
}

void CTerminal::ForceCloseTerminal()
{
	m_currentlyOpenTerminal = NULL;
	if(!m_bOpen)
		return;

	//CMenuManager::GetManager()->CloseMenu();
	m_terminalWnd->CloseView();
}

void CTerminal::SetCommandVisible(int terminalIndex, bool visible)
{
	if(terminalIndex < 0 || terminalIndex >= COMMANDS_PER_TERMINAL)
		return;

	m_commands[terminalIndex].bVisible = visible;

	if(this == m_currentlyOpenTerminal)
	{
		// Update the change
		m_terminalWnd->SetCommandVisibility(terminalIndex, visible);
	}
}

void CTerminal::ExecuteCommand(int commandIndex)
{
	if(commandIndex < 0 || commandIndex >= COMMANDS_PER_TERMINAL)
		return;

	if(!m_commands[commandIndex].bEnabled || !m_commands[commandIndex].bVisible)
		return;
	ExecuteCommand(m_commands[commandIndex]);
}

void CTerminal::ReportTerminalViewClosed()
{
	m_bOpen = false;
	if(m_activeCloseCommand != NULL)
		ExecuteCommand(*m_activeCloseCommand);
}

void CTerminal::AddMessage(std::string str)
{
	int num = m_logMsgCount;
	if(num >= TERMINAL_LOG_MESSAGES)
		--num;
	else
		++m_logMsgCount;
	for(int i=num-1;i>=0;--i)
		m_log[i+1] = m_log[i];
	m_log[0] = str;

	// Update the message
	if(m_terminalWnd != NULL)
	{
		std::string msg = "";
		for(int i=0; i<m_logMsgCount; ++i)
		{
			msg += m_log[i];
			msg += '\n';
		}
		m_terminalWnd->UpdateTerminalMessage(msg);
	}
}

void CTerminal::ClearMessageLog()
{
	m_logMsgCount = 0;

	// Update the message
	if(m_terminalWnd != NULL)
		m_terminalWnd->UpdateTerminalMessage("");
}

void CTerminal::ExecuteCommand(SCommand &command)
{
	m_bExecutingCommand = true;
	if(command.triggerType == TERMINAL_TRIGGER_NONE)
	{
		m_bExecutingCommand = false;
		return;
	}
	else if(command.triggerType == TERMINAL_TRIGGER_SCRIPT)
	{
		CScriptManager::Execute(command.script, SCRIPT_CALLER_TERMINAL, 0, NULL);
	}
	else if(command.triggerType == TERMINAL_TRIGGER_MESSAGE)
	{
		if(m_bOpen)
		{
			if(m_terminalWnd != NULL)
				//m_terminalWnd->UpdateTerminalMessage(command.message);
				AddMessage(command.message);
		}
		else
		{
			GetOTE()->AddScreenMessage(command.message);
		}
	}

	// Close terminal commands
	else if(command.triggerType == TERMINAL_TRIGGER_CLOSEFUNCTION)
	{
		// Close without invoking the default on close command
		if(m_bOpen)
		{
			m_activeCloseCommand = &command;
			ForceCloseTerminal();
		}
		else
			GetOTE()->AddScreenMessage(command.message);
	}
	else if(command.triggerType == TERMINAL_TRIGGER_DIALOGUE)
	{
		// 
		if(m_bOpen)
		{
			m_activeCloseCommand = &command;
			ForceCloseTerminal();
		}
		else
			GetOTE()->GetCurrentLevel()->InitiateDialogue(command.dialogueID);
	}

	m_bExecutingCommand = false;
}

bool CTerminal::ReadTerminalDataFromStream(fstream &f, bool updateCommands)
{
	char buffer[TERMINAL_MESSAGE_LENGTH];

	f.read((char*)&m_ID, sizeof(long));
	if(m_ID < 0)
	{
		CPrinter::logError("CTerminal::ReadTerminalDataFromStream: corrupted data (invalid id)");
		return false;
	}
	f.read((char*)&m_bFunctional, sizeof(bool));

	f.read(buffer, TERMINAL_NAME_LENGTH);
	m_name = buffer;
	f.read(buffer, TERMINAL_MESSAGE_LENGTH);
	m_welcomeMessage = buffer;
	f.read(buffer, TERMINAL_MESSAGE_LENGTH);
	m_disfunctionalMessage = buffer;

	DWORD count;
	f.read((char*)&count, sizeof(DWORD));
	if(count != COMMANDS_PER_TERMINAL)
	{
		CPrinter::logError("CTerminal::ReadTerminalDataFromStream: corrupted data or wrong data version");
		return false;
	}

	f.read((char*)&m_onCloseTemplate, sizeof(STerminalCommand));

	STerminalCommand temp;
	for(int i=0; i<COMMANDS_PER_TERMINAL; ++i)
	{
		f.read((char*)&temp, sizeof(STerminalCommand));
		m_commandTemplates[i] = temp;
	}

	if(updateCommands)
		UpdateCommands();

	return true;
}

void CTerminal::WriteTerminalDataIntoStream(fstream &f)
{
	char buffer[TERMINAL_MESSAGE_LENGTH];

	f.write((const char*)&m_ID, sizeof(long));
	f.write((const char*)&m_bFunctional, sizeof(bool));

	strncpy(buffer, m_name.c_str(), TERMINAL_NAME_LENGTH-1);
	f.write(buffer, TERMINAL_NAME_LENGTH);

	strncpy(buffer, m_welcomeMessage.c_str(), TERMINAL_MESSAGE_LENGTH-1);
	f.write(buffer, TERMINAL_MESSAGE_LENGTH);

	strncpy(buffer, m_disfunctionalMessage.c_str(), TERMINAL_MESSAGE_LENGTH-1);
	f.write(buffer, TERMINAL_MESSAGE_LENGTH);

	DWORD count = COMMANDS_PER_TERMINAL;
	f.write((const char*)&count, sizeof(DWORD));

	f.write((const char*)&m_onCloseTemplate, sizeof(STerminalCommand));

	for(int i=0; i<COMMANDS_PER_TERMINAL; ++i)
		f.write((const char*)&m_commandTemplates[i], sizeof(STerminalCommand));
}

void CTerminal::UpdateCommands()
{
	m_onClose.bEnabled = m_onCloseTemplate.bEnabled;
	m_onClose.bVisible = m_onCloseTemplate.bVisible;
	m_onClose.name = m_onCloseTemplate.name;
	m_onClose.message = m_onCloseTemplate.message;
	m_onClose.triggerType = m_onCloseTemplate.triggerType;
	m_onClose.script = 0;
	m_onClose.dialogueID = m_onCloseTemplate.dialogueID;

	if(m_onCloseTemplate.triggerType == TERMINAL_TRIGGER_SCRIPT)
		m_onClose.script = CScriptManager::GetManager().GetScript(m_onCloseTemplate.script);

	for(int i=0; i<COMMANDS_PER_TERMINAL; ++i)
	{
		UpdateCommand(i);
	}
}

void CTerminal::UpdateCommand(int index)
{
	m_commands[index].bEnabled = m_commandTemplates[index].bEnabled;
	m_commands[index].bVisible = m_commandTemplates[index].bVisible;
	m_commands[index].name = m_commandTemplates[index].name;
	m_commands[index].message = m_commandTemplates[index].message;
	m_commands[index].triggerType = m_commandTemplates[index].triggerType;
	m_commands[index].script = 0;
	m_commands[index].dialogueID = m_commandTemplates[index].dialogueID;

	if(m_commandTemplates[index].triggerType == TERMINAL_TRIGGER_SCRIPT)
		m_commands[index].script = CScriptManager::GetManager().GetScript(m_commandTemplates[index].script);
}