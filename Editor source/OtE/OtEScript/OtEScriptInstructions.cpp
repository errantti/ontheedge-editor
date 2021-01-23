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

#include "OtEScriptInstructions.h"
#include "TimerInstructions.h"
#include "GadgetInstructions.h"
#include "GameInstructions.h"
#include "ObjectInstructions.h"
#include "EnemyInstructions.h"

#include "..\\OnTheEdge.h"

void COtEScriptInstructions::AddOtEScriptCommands(CScriptEngine *scriptEngine)
{
	if(scriptEngine == NULL)
		return;

	CScriptEngine &se = *scriptEngine;

	se.AddVariableType("OBJECT");
	se.AddVariableType("GADGET");
	se.AddVariableType("TIMER");
	se.AddVariableType("ITEM");

	int id = 1000;

	/**
	 *	if_ptrNull
	 **/
	CInstructionTemplate if_ptrNull(new Script_if_ptrNull, scriptEngine, "if_ptrNull", 
		INSTRUCTION_TYPE_IF, id++, "ANY");
	se.AddInstructionTemplate(if_ptrNull);

	/**
	 *	if_ptrNotNull
	 **/
	CInstructionTemplate if_ptrNotNull(new Script_if_ptrNotNull, scriptEngine, "if_ptrNotNull", 
		INSTRUCTION_TYPE_IF, id++, "ANY");
	se.AddInstructionTemplate(if_ptrNotNull);

	/**
	 *	screenDisplayStr
	 **/
	CInstructionTemplate screenDisplayStr(new Script_screenDisplayStr, scriptEngine, "screenDisplayStr", 
		INSTRUCTION_TYPE_EXECUTE, id++, "STR|NUM");
	se.AddInstructionTemplate(screenDisplayStr);

	/**
	 *	screenDisplayStrEx
	 **/
	CInstructionTemplate screenDisplayStrEx(new Script_screenDisplayStrEx, scriptEngine, "screenDisplayStrEx", 
		INSTRUCTION_TYPE_EXECUTE, id++, "STR|NUM|NUM|VEC3");
	se.AddInstructionTemplate(screenDisplayStrEx);

	/**
	 *	randomizeLocation
	 **/
	CInstructionTemplate randomizeLocation(new Script_randomizeLocation, scriptEngine, "randomizeLocation", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "VEC3|NUM|NUM");
	se.AddInstructionTemplate(randomizeLocation);


	/**
	 *	isCallerObject
	 **/
	CInstructionTemplate isCallerObject(new Script_isCallerObject, scriptEngine, "isCallerObject", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "BOOL");
	se.AddInstructionTemplate(isCallerObject);

	/**
	 *	isCallerGadget
	 **/
	CInstructionTemplate isCallerGadget(new Script_isCallerGadget, scriptEngine, "isCallerGadget", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "BOOL");
	se.AddInstructionTemplate(isCallerGadget);

	/**
	 *	isCallerScript
	 **/
	CInstructionTemplate isCallerScript(new Script_isCallerScript, scriptEngine, "isCallerScript", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "BOOL");
	se.AddInstructionTemplate(isCallerScript);

	/**
	 *	isCallerCampaign
	 **/
	CInstructionTemplate isCallerCampaign(new Script_isCallerCampaign, scriptEngine, "isCallerCampaign", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "BOOL");
	se.AddInstructionTemplate(isCallerCampaign);

	/**
	 *	getCallerObject
	 **/
	CInstructionTemplate getCallerObject(new Script_getCallerObject, scriptEngine, "getCallerObject", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT");
	se.AddInstructionTemplate(getCallerObject);

	/**
	 *	getCallerGadget
	 **/
	CInstructionTemplate getCallerGadget(new Script_getCallerGadget, scriptEngine, "getCallerGadget", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "GADGET");
	se.AddInstructionTemplate(getCallerGadget);

	/**
	 *	getCallerObjectTypeID
	 **/
	CInstructionTemplate getCallerObjectTypeID(new Script_getCallerObjectTypeID, scriptEngine, "getCallerObjectTypeID", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "NUM");
	se.AddInstructionTemplate(getCallerObjectTypeID);

	/**
	 *	executeScript
	 **/
	CInstructionTemplate executeScript(new Script_executeScript, scriptEngine, "executeScript", 
		INSTRUCTION_TYPE_EXECUTE, id++, "STR");
	se.AddInstructionTemplate(executeScript);


	/**
	 * Other blocks
	 **/
	id = 2000;
	CTimerInstructions::AddTimerCommands(scriptEngine, id);
	id = 3000;
	CGadgetInstructions::AddGadgetCommands(scriptEngine, id);
	id = 4000;
	CObjectInstructions::AddObjectCommands(scriptEngine, id);
	id = 5000;
	CGameInstructions::AddGameCommands(scriptEngine, id);
	id = 6000;
	CEnemyInstructions::AddEnemyCommands(scriptEngine, id);
}




////////////////////////////
/// Instruction handlers ///
////////////////////////////

ScriptReturn COtEScriptInstructions::Script_if_ptrNull::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref == 0)
		return SCRIPT_RETURN_TRUE;
	return SCRIPT_RETURN_FALSE;
}

ScriptReturn COtEScriptInstructions::Script_if_ptrNotNull::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref == 0)
		return SCRIPT_RETURN_FALSE;
	return SCRIPT_RETURN_TRUE;
}

ScriptReturn COtEScriptInstructions::Script_screenDisplayStr::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CScreenMessages::SScreenMessage msg;

	/*msg.FillDefaults();
	msg.message = param[0].str_value;
	msg.bFades = true;
	msg.time = (float)param[1].num_value;*/

	GetOTE()->GetScreenMessageInterface()->AddMessage(param[0].str_value, (float)param[1].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn COtEScriptInstructions::Script_screenDisplayStrEx::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CScreenMessages::SScreenMessage msg;

	msg.FillDefaults();
	msg.message = param[0].str_value;
	msg.bFades = true;
	msg.time = (float)param[1].num_value;
	msg.fadeTime = (float)param[2].num_value;
	msg.textColor = D3DCOLOR_RGBA((int)param[3].vec3_value.x, (int)param[3].vec3_value.y, (int)param[3].vec3_value.z, 255);

	GetOTE()->GetScreenMessageInterface()->AddDetailedMessage(msg);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn COtEScriptInstructions::Script_randomizeLocation::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	float dx = (float)param[1].num_value;
	float dz = (float)param[2].num_value;

	modifyVar->vec3_value.x = GetOTE()->RandFloat(modifyVar->vec3_value.x - dx, modifyVar->vec3_value.x + dx);
	modifyVar->vec3_value.z = GetOTE()->RandFloat(modifyVar->vec3_value.z - dz, modifyVar->vec3_value.z + dz);

	return SCRIPT_RETURN_TRUE;
}

ScriptReturn COtEScriptInstructions::Script_isCallerObject::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(CScriptManager::GetManager().GetCallerType() == SCRIPT_CALLER_OBJECT)
		modifyVar->bool_value = true;
	else modifyVar->bool_value = false;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn COtEScriptInstructions::Script_isCallerGadget::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(CScriptManager::GetManager().GetCallerType() == SCRIPT_CALLER_GADGET)
		modifyVar->bool_value = true;
	else modifyVar->bool_value = false;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn COtEScriptInstructions::Script_isCallerScript::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(CScriptManager::GetManager().GetCallerType() == SCRIPT_CALLER_SCRIPT)
		modifyVar->bool_value = true;
	else modifyVar->bool_value = false;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn COtEScriptInstructions::Script_isCallerCampaign::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(CScriptManager::GetManager().GetCallerType() == SCRIPT_CALLER_CAMPAIGN)
		modifyVar->bool_value = true;
	else modifyVar->bool_value = false;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn COtEScriptInstructions::Script_getCallerObject::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(CScriptManager::GetManager().GetCallerType() != SCRIPT_CALLER_OBJECT)
		return SCRIPT_RETURN_ERROR;

	modifyVar->custom_ref = CScriptManager::GetManager().GetCaller();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn COtEScriptInstructions::Script_getCallerGadget::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(CScriptManager::GetManager().GetCallerType() != SCRIPT_CALLER_GADGET)
		return SCRIPT_RETURN_ERROR;

	modifyVar->custom_ref = CScriptManager::GetManager().GetCaller();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn COtEScriptInstructions::Script_getCallerObjectTypeID::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->num_value = CScriptManager::GetManager().GetCallerTypeID();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn COtEScriptInstructions::Script_executeScript::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CScriptManager *manager = GetOTE()->GetScriptManager();

	CScript *s = manager->GetScript(param[0].str_value);
	if(s != NULL)
		manager->Execute(s, SCRIPT_CALLER_SCRIPT, 0, NULL);

	return SCRIPT_RETURN_NOVALUE;
}