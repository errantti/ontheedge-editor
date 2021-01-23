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

#ifndef __CENEMYINSTRUCTIONS_H__
#define __CENEMYINSTRUCTIONS_H__

#include "..\\Edgewalker\\EWScript\\ScriptEngine.h"

/**
 * Handles enemy instructions (OtEScript)
 */
class CEnemyInstructions
{
public:
	static void AddEnemyCommands(CScriptEngine *scriptEngine, int &id);

private:
	class Script_possessEnemy : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_unpossess : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getPossessedEnemy : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_createEnemy : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_setWaypointSet : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_startRouteFromBeginning : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_startRouteFromNearest : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_stopRoute : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_isAggroed : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_forceAggroTarget : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_enemyHelpCall : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_addThreat : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_enableEnemyGadget : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	/**
	 * AI
	 **/
	class Script_setAIShoots : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_setAIMobile : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_setAIAggroResponse : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};
};

#endif // __CENEMYINSTRUCTIONS_H__