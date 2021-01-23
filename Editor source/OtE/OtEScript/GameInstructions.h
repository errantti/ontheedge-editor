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

#ifndef __CGAMEINSTRUCTIONS_H__
#define __CGAMEINSTRUCTIONS_H__

#include "..\\Edgewalker\\EWScript\\ScriptEngine.h"

class CGameInstructions
{
public:
	static void AddGameCommands(CScriptEngine *scriptEngine, int &id);

private:
	/**
	 * General
	 **/
	class Script_isSingleplayer : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_loadLevel : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_restartLevel : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_closeLevel : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_initiateDialogue : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_displayAssistantMsg : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_addAssistantMsg : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_playSound : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_updateMissionObjective : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getMissionObjective : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_reportTutorial : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	/**
	 * Terminals
	 **/
	class Script_openTerminal : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_forceOpenTerminal : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_enableTerminal : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_enableTerminalCommand : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_setTerminalCommandVisible : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_writeToTerminal : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_closeCurrentTerminal : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getCurrentTerminalID : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	/**
	 * Global variables
	 **/
	class Script_if_isGlobalSet : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_if_notIsGlobalSet : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_setGlobalValue : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getGlobalValue : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_removeGlobalValue : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};
};

#endif // #ifndef __CGAMEINSTRUCTIONS_H__