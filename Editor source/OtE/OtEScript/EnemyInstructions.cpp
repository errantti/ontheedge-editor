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

#include "EnemyInstructions.h"
#include "..\\OnTheEdge.h"

void CEnemyInstructions::AddEnemyCommands(CScriptEngine *scriptEngine, int &id)
{
	if(scriptEngine == NULL)
		return;

	CScriptEngine &se = *scriptEngine;

	/**
	 *	possessEnemy
	 **/
	CInstructionTemplate possessEnemy(new Script_possessEnemy, scriptEngine, "possessEnemy", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT");
	se.AddInstructionTemplate(possessEnemy);

	/**
	 *	unpossess
	 **/
	CInstructionTemplate unpossess(new Script_unpossess, scriptEngine, "unpossess", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "");
	se.AddInstructionTemplate(unpossess);

	/**
	 *	getPossessedEnemy
	 **/
	CInstructionTemplate getPossessedEnemy(new Script_getPossessedEnemy, scriptEngine, "getPossessedEnemy", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT");
	se.AddInstructionTemplate(getPossessedEnemy);

	/**
	 *	createEnemy
	 **/
	CInstructionTemplate createEnemy(new Script_createEnemy, scriptEngine, "createEnemy", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|NUM|VEC3|NUM");
	se.AddInstructionTemplate(createEnemy);

	/**
	 *	setWaypointSet
	 **/
	CInstructionTemplate setWaypointSet(new Script_setWaypointSet, scriptEngine, "setWaypointSet", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|NUM");
	se.AddInstructionTemplate(setWaypointSet);

	/**
	 *	startRouteFromBeginning
	 **/
	CInstructionTemplate startRouteFromBeginning(new Script_startRouteFromBeginning, scriptEngine, "startRouteFromBeginning", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT");
	se.AddInstructionTemplate(startRouteFromBeginning);

	/**
	 *	startRouteFromNearest
	 **/
	CInstructionTemplate startRouteFromNearest(new Script_startRouteFromNearest, scriptEngine, "startRouteFromNearest", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT");
	se.AddInstructionTemplate(startRouteFromNearest);

	/**
	 *	stopRoute
	 **/
	CInstructionTemplate stopRoute(new Script_stopRoute, scriptEngine, "stopRoute", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT");
	se.AddInstructionTemplate(stopRoute);

	/**
	 *	isAggroed
	 **/
	CInstructionTemplate isAggroed(new Script_isAggroed, scriptEngine, "isAggroed", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "BOOL|OBJECT");
	se.AddInstructionTemplate(isAggroed);

	/**
	 *	forceAggroTarget
	 **/
	CInstructionTemplate forceAggroTarget(new Script_forceAggroTarget, scriptEngine, "forceAggroTarget", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|OBJECT");
	se.AddInstructionTemplate(forceAggroTarget);

	/**
	 *	enemyHelpCall
	 **/
	CInstructionTemplate enemyHelpCall(new Script_enemyHelpCall, scriptEngine, "enemyHelpCall", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT");
	se.AddInstructionTemplate(enemyHelpCall);

	/**
	 *	addThreat
	 **/
	CInstructionTemplate addThreat(new Script_addThreat, scriptEngine, "addThreat", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|OBJECT|NUM");
	se.AddInstructionTemplate(addThreat);



	/**
	 * AI
	 **/

	/**
	 *	setAIShoots
	 **/
	CInstructionTemplate setAIShoots(new Script_setAIShoots, scriptEngine, "setAIShoots", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|BOOL");
	se.AddInstructionTemplate(setAIShoots);

	/**
	 *	setAIMobile
	 **/
	CInstructionTemplate setAIMobile(new Script_setAIMobile, scriptEngine, "setAIMobile", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|BOOL");
	se.AddInstructionTemplate(setAIMobile);

	/**
	 *	setAIAggroResponse
	 **/
	CInstructionTemplate setAIAggroResponse(new Script_setAIAggroResponse, scriptEngine, "setAIAggroResponse", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|STR");
	se.AddInstructionTemplate(setAIAggroResponse);

	/**
	 *	enableEnemyGadget
	 **/
	CInstructionTemplate enableEnemyGadget(new Script_enableEnemyGadget, scriptEngine, "enableEnemyGadget", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|NUM|BOOL");
	se.AddInstructionTemplate(enableEnemyGadget);
}




////////////////////////////
/// Instruction handlers ///
////////////////////////////

ScriptReturn CEnemyInstructions::Script_possessEnemy::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	if(!o)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	GetOTE()->GetPlayer(0)->PossessEnemy((CEnemy*)o);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_unpossess::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	GetOTE()->GetPlayer(0)->UnPosses();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_getPossessedEnemy::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->custom_ref = GetOTE()->GetPlayer(0)->GetPossessedEnemy();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_createEnemy::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->custom_ref = GetOTE()->GetObjectManager()->DropEnemy((long)param[1].num_value, param[2].vec3_value.GetD3DXVECTOR3(), (long)param[3].num_value, GetOTE()->GetEnemyAI());

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_setWaypointSet::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	if(!o)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	CEnemy *e = (CEnemy*)o;
	e->SetWaypointSetID((long)param[1].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_startRouteFromBeginning::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	if(!o)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	CEnemy *e = (CEnemy*)o;
	e->StartFollowingRoute(true, false);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_startRouteFromNearest::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	if(!o)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	CEnemy *e = (CEnemy*)o;
	e->StartFollowingRoute(false, false);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_stopRoute::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	if(!o)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	CEnemy *e = (CEnemy*)o;
	e->StopFollowingRoute();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_isAggroed::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	if(!o)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	CEnemy *e = (CEnemy*)o;
	modifyVar->bool_value = e->IsAggroed();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_forceAggroTarget::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	IGameObject *target = (IGameObject*)param[1].custom_ref;
	if(!o || !target)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;
	if(target->GetType() != ENEMY_OBJECT_ID && target->GetType() != SHIP_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	CEnemy *e = (CEnemy*)o;
	e->ForceAggroTarget((CVessel*)target);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_enemyHelpCall::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	if(!o)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	CEnemy *e = (CEnemy*)o;
	e->CallForHelp();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_addThreat::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	IGameObject *target = (IGameObject*)param[1].custom_ref;
	if(!o || !target)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;
	if(target->GetType() != ENEMY_OBJECT_ID && target->GetType() != SHIP_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	CEnemy *e = (CEnemy*)o;
	e->ReportThreat((CVessel*)target, (float)param[2].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_enableEnemyGadget::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	if(!o)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	((CEnemy*)o)->SetGadgetEnabled((int)param[1].num_value, param[2].bool_value);

	return SCRIPT_RETURN_NOVALUE;
}



/**
 * AI stuff
 **/

ScriptReturn CEnemyInstructions::Script_setAIShoots::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	if(!o)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	CEnemy *e = (CEnemy*)o;
	e->GetAI().bShoots = param[1].bool_value;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_setAIMobile::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	if(!o)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	CEnemy *e = (CEnemy*)o;
	e->GetAI().bMobile = param[1].bool_value;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CEnemyInstructions::Script_setAIAggroResponse::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	if(!o)
		return SCRIPT_RETURN_ERROR;
	if(o->GetType() != ENEMY_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	CEnemy *e = (CEnemy*)o;
	if(strcmp(param[1].str_value, "ATTACK"))
		e->GetAI().aggroResponse = ENEMY_AGGRO_RESPONSE_ATTACK;
	else if(strcmp(param[1].str_value, "FOLLOWROUTE"))
		e->GetAI().aggroResponse = ENEMY_AGGRO_RESPONSE_FOLLOWROUTE;
	else if(strcmp(param[1].str_value, "ESCAPE"))
		e->GetAI().aggroResponse = ENEMY_AGGRO_RESPONSE_ESCAPE;

	return SCRIPT_RETURN_NOVALUE;
}