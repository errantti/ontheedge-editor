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

#ifndef __CTERMINAL_H__
#define __CTERMINAL_H__

#include <fstream>
#include <string>
#include "..\\Edgewalker\\Input\\IInputReceiver.h"
#include "..\\Edgewalker\\EWScript\\Script.h"

enum TERMINAL_TRIGGER_TYPE {
	// Closes the terminal to start the dialogue
	TERMINAL_TRIGGER_DIALOGUE=0,
	TERMINAL_TRIGGER_SCRIPT,
	TERMINAL_TRIGGER_MESSAGE,
	TERMINAL_TRIGGER_NONE,
	TERMINAL_TRIGGER_CLOSEFUNCTION,
	TERMINAL_TRIGGER_FORCEDWORD = 0xFFFFFFFF
};

#define COMMANDS_PER_TERMINAL		6
#define TERMINAL_MESSAGE_LENGTH		128
#define TERMINAL_NAME_LENGTH			64

#define TERMINAL_LOG_MESSAGES		5

struct STerminalCommand
{
	char name[32];
	union {
		char script[32];
		char message[TERMINAL_MESSAGE_LENGTH];
	};
	long dialogueID;
	TERMINAL_TRIGGER_TYPE triggerType;
	bool bVisible;
	bool bEnabled;

	void FillDefaults()
	{
		name[0] = '\0';
		script[0] = '\0';
		dialogueID = -1;
		triggerType = TERMINAL_TRIGGER_NONE;
		bVisible = false;
		bEnabled = true;
	}
};

class CDialogue;
class CTerminalView;

/**
 * Terminal in the game is a graphical menu with up to six
 * buttons which represent commands given to a computer
 * terminal. Each button can trigger either script or a
 * dialogue (or nothing). Buttons can also be rendered
 * invisible and the status can be changed through scripts.
 * Also the whole terminal is either functional and not
 * and can be activated / deactivated through scripts.
 * In addition terminal has an onClose command which is
 * triggered when the terminal window is closed
 */
class CTerminal
{
public:
	CTerminal();
	~CTerminal();

	long GetID() {return m_ID;}
	void SetID(long id) {m_ID = id;}
	bool IsFunctional() {return m_bFunctional;}
	void SetIsFunctional(bool value) {m_bFunctional = value;}

	std::string GetName() {return m_name;}
	std::string GetWelcomeMessage() {return m_welcomeMessage;}
	std::string GetDisfunctionalMessage() {return m_disfunctionalMessage;}

	void SetName(std::string str) {m_name = str;}
	void SetWelcomeMessage(std::string str) {m_welcomeMessage = str;}
	void SetDisfunctionalMessage(std::string str) {m_disfunctionalMessage = str;}

	void AddMessage(std::string str);
	void ClearMessageLog();

	void ExecuteCommand(int commandIndex);

	/// Opens the terminal
	void OpenTerminal();
	/// Opens the terminal regardless of functional state
	void ForceOpenTerminal();

	/// Signals the terminal to close itself as soon as possible and execute on close command
	void CloseTerminal();
	/// Immediately starts closing the terminal without executing on close command
	void ForceCloseTerminal();

	void FillDefaults();

	void SetCommandVisible(int terminalIndex, bool visible);
	bool IsCommandVisible(int terminalIndex) {return m_commands[terminalIndex].bVisible;}

	std::string GetCommandName(int commandIndex) {return m_commands[commandIndex].name;}
	bool IsCommandEnabled(int commandIndex) {return m_commands[commandIndex].bEnabled;}

	bool ReadTerminalDataFromStream(std::fstream &f, bool updateCommands = true);
	void WriteTerminalDataIntoStream(std::fstream &f);

	STerminalCommand *GetTerminalCommandData() {return m_commandTemplates;}
	void UpdateCommands();

	STerminalCommand *GetOnCloseCommand() {return &m_onCloseTemplate;}

	void ProcessInput() {}

	void SetTerminalWindowHandler(CTerminalView *tv) {m_terminalWnd = tv;}

	/// Called by CTerminalView when the terminal window has been closed
	void ReportTerminalViewClosed();

	static CTerminal *GetCurrentlyOpenTerminal() {return m_currentlyOpenTerminal;}
	static void ResetCurrentTerminalReference() {m_currentlyOpenTerminal = NULL;}

protected:
	struct SCommand {
		std::string name;
		std::string message;
		TERMINAL_TRIGGER_TYPE triggerType;
		CScript *script;
		//CDialogue *dialogue;
		long dialogueID;
		bool bVisible;
		bool bEnabled;
	};

	void ExecuteCommand(SCommand &command);
	void UpdateCommand(int index);

	CTerminalView *m_terminalWnd;

	std::string m_name;
	std::string m_welcomeMessage;
	std::string m_disfunctionalMessage;

	std::string m_log[TERMINAL_LOG_MESSAGES];
	int m_logMsgCount;

	STerminalCommand m_onCloseTemplate;
	SCommand m_onClose;

	STerminalCommand m_commandTemplates[COMMANDS_PER_TERMINAL];
	SCommand m_commands[COMMANDS_PER_TERMINAL];

	long m_ID;
	bool m_bFunctional;
	bool m_bExecutingCommand, m_bOpen;
	SCommand *m_activeCloseCommand;

	static CTerminal *m_currentlyOpenTerminal;
};

#endif // #ifndef __CTERMINAL_H__