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

#include "GameInstructions.h"
#include "..\\OnTheEdge.h"

void CGameInstructions::AddGameCommands(CScriptEngine *scriptEngine, int &id)
{
	if(scriptEngine == NULL)
		return;

	CScriptEngine &se = *scriptEngine;

	/**
	 *	isSingleplayer
	 **/
	CInstructionTemplate isSingleplayer(new Script_isSingleplayer, scriptEngine, "isSingleplayer", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "BOOL");
	se.AddInstructionTemplate(isSingleplayer);

	/**
	 *	loadLevel
	 **/
	CInstructionTemplate loadLevel(new Script_loadLevel, scriptEngine, "loadLevel", 
		INSTRUCTION_TYPE_EXECUTE, id++, "STR|NUM");
	se.AddInstructionTemplate(loadLevel);

	/**
	 *	restartLevel
	 **/
	CInstructionTemplate restartLevel(new Script_restartLevel, scriptEngine, "restartLevel", 
		INSTRUCTION_TYPE_EXECUTE, id++, "STR");
	se.AddInstructionTemplate(restartLevel);

	/**
	 *	initiateDialogue
	 **/
	CInstructionTemplate initiateDialogue(new Script_initiateDialogue, scriptEngine, "initiateDialogue", 
		INSTRUCTION_TYPE_EXECUTE, id++, "NUM");
	se.AddInstructionTemplate(initiateDialogue);

	/**
	 *	playSound
	 **/
	CInstructionTemplate playSound(new Script_playSound, scriptEngine, "playSound", 
		INSTRUCTION_TYPE_EXECUTE, id++, "NUM");
	se.AddInstructionTemplate(playSound);

	/**
	 *	updateMissionObjective
	 **/
	CInstructionTemplate updateMissionObjective(new Script_updateMissionObjective, scriptEngine, "updateMissionObjective", 
		INSTRUCTION_TYPE_EXECUTE, id++, "STR|NUM|BOOL");
	se.AddInstructionTemplate(updateMissionObjective);



	/**
	 *	openTerminal
	 **/
	CInstructionTemplate openTerminal(new Script_openTerminal, scriptEngine, "openTerminal", 
		INSTRUCTION_TYPE_EXECUTE, id++, "NUM");
	se.AddInstructionTemplate(openTerminal);

	/**
	 *	forceOpenTerminal
	 **/
	CInstructionTemplate forceOpenTerminal(new Script_forceOpenTerminal, scriptEngine, "forceOpenTerminal", 
		INSTRUCTION_TYPE_EXECUTE, id++, "NUM");
	se.AddInstructionTemplate(forceOpenTerminal);

	/**
	 *	enableTerminal
	 **/
	CInstructionTemplate enableTerminal(new Script_enableTerminal, scriptEngine, "enableTerminal", 
		INSTRUCTION_TYPE_EXECUTE, id++, "NUM|BOOL");
	se.AddInstructionTemplate(enableTerminal);

	/**
	 *	enableTerminalCommand
	 **/
	CInstructionTemplate enableTerminalCommand(new Script_enableTerminalCommand, scriptEngine, "enableTerminalCommand", 
		INSTRUCTION_TYPE_EXECUTE, id++, "NUM|NUM|BOOL");
	se.AddInstructionTemplate(enableTerminalCommand);

	/**
	 *	setTerminalCommandVisible
	 **/
	CInstructionTemplate setTerminalCommandVisible(new Script_setTerminalCommandVisible, scriptEngine, "setTerminalCommandVisible", 
		INSTRUCTION_TYPE_EXECUTE, id++, "NUM|NUM|BOOL");
	se.AddInstructionTemplate(setTerminalCommandVisible);




	/**
	 *	if_isGlobalSet
	 **/
	CInstructionTemplate if_isGlobalSet(new Script_if_isGlobalSet, scriptEngine, "if_isGlobalSet", 
		INSTRUCTION_TYPE_IF, id++, "STR");
	se.AddInstructionTemplate(if_isGlobalSet);

	/**
	 *	if_notIsGlobalSet
	 **/
	CInstructionTemplate if_notIsGlobalSet(new Script_if_notIsGlobalSet, scriptEngine, "if_notIsGlobalSet", 
		INSTRUCTION_TYPE_IF, id++, "STR");
	se.AddInstructionTemplate(if_notIsGlobalSet);

	/**
	 *	setGlobalValue
	 **/
	CInstructionTemplate setGlobalValue(new Script_setGlobalValue, scriptEngine, "setGlobalValue", 
		INSTRUCTION_TYPE_EXECUTE, id++, "STR|NUM");
	se.AddInstructionTemplate(setGlobalValue);

	/**
	 *	getGlobalValue
	 **/
	CInstructionTemplate getGlobalValue(new Script_getGlobalValue, scriptEngine, "getGlobalValue", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "NUM|STR");
	se.AddInstructionTemplate(getGlobalValue);

	/**
	 *	removeGlobalValue
	 **/
	CInstructionTemplate removeGlobalValue(new Script_removeGlobalValue, scriptEngine, "removeGlobalValue", 
		INSTRUCTION_TYPE_EXECUTE, id++, "STR");
	se.AddInstructionTemplate(removeGlobalValue);


	/**
	 *	getMissionObjective
	 **/
	CInstructionTemplate getMissionObjective(new Script_getMissionObjective, scriptEngine, "getMissionObjective", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "STR|NUM");
	se.AddInstructionTemplate(getMissionObjective);


	/**
	 *	writeToTerminal
	 **/
	CInstructionTemplate writeToTerminal(new Script_writeToTerminal, scriptEngine, "writeToTerminal", 
		INSTRUCTION_TYPE_EXECUTE, id++, "STR");
	se.AddInstructionTemplate(writeToTerminal);

	/**
	 *	closeCurrentTerminal
	 **/
	CInstructionTemplate closeCurrentTerminal(new Script_closeCurrentTerminal, scriptEngine, "closeCurrentTerminal", 
		INSTRUCTION_TYPE_EXECUTE, id++, "");
	se.AddInstructionTemplate(closeCurrentTerminal);

	/**
	 *	closeLevel
	 **/
	CInstructionTemplate closeLevel(new Script_closeLevel, scriptEngine, "closeLevel", 
		INSTRUCTION_TYPE_EXECUTE, id++, "BOOL");
	se.AddInstructionTemplate(closeLevel);

	/**
	 *	getCurrentTerminalID
	 **/
	CInstructionTemplate getCurrentTerminalID(new Script_getCurrentTerminalID, scriptEngine, "getCurrentTerminalID", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "INT");
	se.AddInstructionTemplate(getCurrentTerminalID);

	/**
	 *	reportTutorial
	 **/
	CInstructionTemplate reportTutorial(new Script_reportTutorial, scriptEngine, "reportTutorial", 
		INSTRUCTION_TYPE_SCRIPT_END, id++, "");
	se.AddInstructionTemplate(reportTutorial);

	/**
	 *	displayAssistantMsg
	 **/
	CInstructionTemplate displayAssistantMsg(new Script_displayAssistantMsg, scriptEngine, "displayAssistantMsg", 
		INSTRUCTION_TYPE_EXECUTE, id++, "NUM");
	se.AddInstructionTemplate(displayAssistantMsg);

	/**
	 *	addAssistantMsg
	 **/
	CInstructionTemplate addAssistantMsg(new Script_addAssistantMsg, scriptEngine, "addAssistantMsg", 
		INSTRUCTION_TYPE_EXECUTE, id++, "STR");
	se.AddInstructionTemplate(addAssistantMsg);
}





////////////////////////////
/// Instruction handlers ///
////////////////////////////

ScriptReturn CGameInstructions::Script_isSingleplayer::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->bool_value = !GetOTE()->IsMultiplayer();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_loadLevel::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CTerminal *t = CTerminal::GetCurrentlyOpenTerminal();
	if(t != NULL)
		t->CloseTerminal();
	GetOTE()->GetCampaign()->ForceEndLevel(param[0].str_value, (long)param[1].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_restartLevel::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	GetOTE()->AddScreenMessage(param[0].str_value);
	GetOTE()->GetCampaign()->RestartLevel();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_closeLevel::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	GetOTE()->GetCampaign()->ForceEndLevel("", -1);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_playSound::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CSound *s = CSoundManager::GetManager().GetSound((long)param[0].num_value);
	if(s)
		s->Play();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_updateMissionObjective::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	GetOTE()->GetCampaign()->SetMissionObjective(param[0].str_value, (int)param[1].num_value);
	if(param[2].bool_value)
		GetOTE()->AddScreenMessage("Mission objectives updated");

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_getMissionObjective::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	int index = (int)param[1].num_value;
	if(index < 0 || index >= 6)
		return SCRIPT_RETURN_NOVALUE;
	strncpy(modifyVar->str_value, GetOTE()->GetCampaign()->GetMissionObjective(index).c_str(), 1023);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_reportTutorial::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	// If tutorials are disabled then end the script
	if(!CGameSettings::GetSettingsObject()->GetSetting(GO_TUTORIALS_ENABLED))
		return SCRIPT_RETURN_TRUE;

	// Otherwise continue as normal
	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_initiateDialogue::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	GetOTE()->GetCurrentLevel()->InitiateDialogue((int)param[0].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_displayAssistantMsg::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	GetOTE()->GetCurrentLevel()->DisplayAssistentMessage((int)param[0].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_addAssistantMsg::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CAssistantMessages::GetIntance()->AddMessage(new std::string(param[0].str_value));

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_openTerminal::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	GetOTE()->GetCurrentLevel()->OpenTerminal((long)param[0].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_forceOpenTerminal::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CTerminal *t = GetOTE()->GetCurrentLevel()->GetTerminal((long)param[0].num_value);
	if(t)
		t->ForceOpenTerminal();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_enableTerminal::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CTerminal *t = GetOTE()->GetCurrentLevel()->GetTerminal((long)param[0].num_value);
	if(t)
	{
		t->SetIsFunctional(param[1].bool_value);
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_enableTerminalCommand::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CTerminal *t = GetOTE()->GetCurrentLevel()->GetTerminal((long)param[0].num_value);
	if(t)
	{
		t->SetCommandVisible((int)param[1].num_value, param[2].bool_value);
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_setTerminalCommandVisible::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CTerminal *t = GetOTE()->GetCurrentLevel()->GetTerminal((long)param[0].num_value);
	if(t)
	{
		t->SetCommandVisible((int)param[1].num_value, param[2].bool_value);
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_writeToTerminal::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CTerminal *t = CTerminal::GetCurrentlyOpenTerminal();
	if(t == NULL)
		return SCRIPT_RETURN_NOVALUE;

	t->AddMessage(param[0].str_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_closeCurrentTerminal::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CTerminal *t = CTerminal::GetCurrentlyOpenTerminal();
	if(t == NULL)
		return SCRIPT_RETURN_NOVALUE;

	t->CloseTerminal();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_getCurrentTerminalID::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CTerminal *t = CTerminal::GetCurrentlyOpenTerminal();
	if(t == NULL)
	{
		modifyVar->num_value = -1;
		return SCRIPT_RETURN_NOVALUE;
	}

	modifyVar->num_value = (double)t->GetID();

	return SCRIPT_RETURN_NOVALUE;
}



ScriptReturn CGameInstructions::Script_if_isGlobalSet::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(GetOTE()->GetCampaign()->DoesGlobalVariableExist(param[0].str_value))
		return SCRIPT_RETURN_TRUE;

	return SCRIPT_RETURN_FALSE;
}

ScriptReturn CGameInstructions::Script_if_notIsGlobalSet::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(GetOTE()->GetCampaign()->DoesGlobalVariableExist(param[0].str_value))
		return SCRIPT_RETURN_FALSE;

	return SCRIPT_RETURN_TRUE;
}

ScriptReturn CGameInstructions::Script_setGlobalValue::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	GetOTE()->GetCampaign()->SetGlobalVariable(param[0].str_value, (float)param[1].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_getGlobalValue::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->num_value = GetOTE()->GetCampaign()->GetGlobalVariable(param[1].str_value);	

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGameInstructions::Script_removeGlobalValue::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	GetOTE()->GetCampaign()->RemoveGlobalVariable(param[0].str_value);

	return SCRIPT_RETURN_NOVALUE;
}