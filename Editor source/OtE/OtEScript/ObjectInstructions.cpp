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

#include "ObjectInstructions.h"
#include "..\\OnTheEdge.h"

void CObjectInstructions::AddObjectCommands(CScriptEngine *scriptEngine, int &id)
{
	if(scriptEngine == NULL)
		return;

	CScriptEngine &se = *scriptEngine;

	/**
	 *	createDynamicObject
	 **/
	CInstructionTemplate createDynamicObject(new Script_createDynamicObject, scriptEngine, "createDynamicObject", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|NUM|VEC3|NUM");
	se.AddInstructionTemplate(createDynamicObject);

	/**
	 *	createStaticObject
	 **/
	CInstructionTemplate createStaticObject(new Script_createStaticObject, scriptEngine, "createStaticObject", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|NUM|VEC3|NUM");
	se.AddInstructionTemplate(createStaticObject);

	/**
	 *	attachLight
	 **/
	CInstructionTemplate attachLight(new Script_attachLight, scriptEngine, "attachLight", 
		INSTRUCTION_TYPE_EXECUTE, id++, "OBJECT|NUM");
	se.AddInstructionTemplate(attachLight);

	/**
	 *	attachParticleEffect
	 **/
	CInstructionTemplate attachParticleEffect(new Script_attachParticleEffect, scriptEngine, "attachParticleEffect", 
		INSTRUCTION_TYPE_EXECUTE, id++, "OBJECT|NUM");
	se.AddInstructionTemplate(attachParticleEffect);

	/**
	 *	removeLight
	 **/
	CInstructionTemplate removeLight(new Script_removeLight, scriptEngine, "removeLight", 
		INSTRUCTION_TYPE_EXECUTE, id++, "OBJECT");
	se.AddInstructionTemplate(removeLight);

	/**
	 *	removeParticleEffect
	 **/
	CInstructionTemplate removeParticleEffect(new Script_removeParticleEffect, scriptEngine, "removeParticleEffect", 
		INSTRUCTION_TYPE_EXECUTE, id++, "OBJECT");
	se.AddInstructionTemplate(removeParticleEffect);

	/**
	 *	inflictDamage 
	 **/
	CInstructionTemplate inflictDamage(new Script_inflictDamage, scriptEngine, "inflictDamage", 
		INSTRUCTION_TYPE_EXECUTE, id++, "OBJECT|NUM|NUM");
	se.AddInstructionTemplate(inflictDamage);

	/**
	 *	getObjectName
	 **/
	CInstructionTemplate getObjectName(new Script_getObjectName, scriptEngine, "getObjectName", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "STR|OBJECT");
	se.AddInstructionTemplate(getObjectName);

	/**
	 *	getObjectID
	 **/
	CInstructionTemplate getObjectID(new Script_getObjectID, scriptEngine, "getObjectID", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "NUM|OBJECT");
	se.AddInstructionTemplate(getObjectID);

	/**
	 *	getObjectPosition
	 **/
	CInstructionTemplate getObjectPosition(new Script_getObjectPosition, scriptEngine, "getObjectPosition", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "VEC3|OBJECT");
	se.AddInstructionTemplate(getObjectPosition);

	/**
	 *	setObjectPosition
	 **/
	CInstructionTemplate setObjectPosition(new Script_setObjectPosition, scriptEngine, "setObjectPosition", 
		INSTRUCTION_TYPE_EXECUTE, id++, "OBJECT|VEC3");
	se.AddInstructionTemplate(setObjectPosition);

	/**
	 *	getObjectRotation
	 **/
	CInstructionTemplate getObjectRotation(new Script_getObjectRotation, scriptEngine, "getObjectRotation", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "VEC3|OBJECT");
	se.AddInstructionTemplate(getObjectRotation);

	/**
	 *	setObjectRotation
	 **/
	CInstructionTemplate setObjectRotation(new Script_setObjectRotation, scriptEngine, "setObjectRotation", 
		INSTRUCTION_TYPE_EXECUTE, id++, "OBJECT|VEC3");
	se.AddInstructionTemplate(setObjectRotation);

	/**
	 *	getPlayerShip
	 **/
	CInstructionTemplate getPlayerShip(new Script_getPlayerShip, scriptEngine, "getPlayerShip", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|NUM");
	se.AddInstructionTemplate(getPlayerShip);

	/**
	 *	countObjects
	 **/
	CInstructionTemplate countObjects(new Script_countObjects, scriptEngine, "countObjects", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "NUM|STR");
	se.AddInstructionTemplate(countObjects);

	/**
	 *	destroyObject
	 **/
	CInstructionTemplate destroyObject(new Script_destroyObject, scriptEngine, "destroyObject", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT");
	se.AddInstructionTemplate(destroyObject);

	/**
	 *	getObjectByID
	 **/
	CInstructionTemplate getObjectByID(new Script_getObjectByID, scriptEngine, "getObjectByID", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|NUM");
	se.AddInstructionTemplate(getObjectByID);

	/**
	 *	getObjectByName
	 **/
	CInstructionTemplate getObjectByName(new Script_getObjectByName, scriptEngine, "getObjectByName", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|STR");
	se.AddInstructionTemplate(getObjectByName);

	/**
	 *	enableForceUpdate
	 **/
	CInstructionTemplate enableForceUpdate(new Script_enableForceUpdate, scriptEngine, "enableForceUpdate", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|BOOL");
	se.AddInstructionTemplate(enableForceUpdate);

	/**
	 *	checkObjectIntersection
	 **/
	CInstructionTemplate checkObjectIntersection(new Script_checkObjectIntersection, scriptEngine, "checkObjectIntersection", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "BOOL|OBJECT|OBJECT");
	se.AddInstructionTemplate(checkObjectIntersection);

	/**
	 *	setStaticObTargetID
	 **/
	CInstructionTemplate setStaticObTargetID(new Script_setStaticObTargetID, scriptEngine, "setStaticObTargetID", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|NUM");
	se.AddInstructionTemplate(setStaticObTargetID);

	/**
	 *	setStaticObTargetRef
	 **/
	CInstructionTemplate setStaticObTargetRef(new Script_setStaticObTargetRef, scriptEngine, "setStaticObTargetRef", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|STR");
	se.AddInstructionTemplate(setStaticObTargetRef);

	/**
	 *	setController
	 **/
	CInstructionTemplate setController(new Script_setController, scriptEngine, "setController", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|NUM");
	se.AddInstructionTemplate(setController);

	/**
	 *	removeController
	 **/
	CInstructionTemplate removeController(new Script_removeController, scriptEngine, "removeController", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT");
	se.AddInstructionTemplate(removeController);

	/**
	 *	activateController
	 **/
	CInstructionTemplate activateController(new Script_activateController, scriptEngine, "activateController", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT");
	se.AddInstructionTemplate(activateController);

	/**
	 *	setObjectControllerParam
	 **/
	CInstructionTemplate setObjectControllerParam(new Script_setObjectControllerParam, scriptEngine, "setObjectControllerParam", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|NUM|NUM");
	se.AddInstructionTemplate(setObjectControllerParam);

	/**
	 *	getObjectHealth
	 **/
	CInstructionTemplate getObjectHealth(new Script_getObjectHealth, scriptEngine, "getObjectHealth", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "NUM|OBJECT");
	se.AddInstructionTemplate(getObjectHealth);

	/**
	 *	getObjectMaxHealth
	 **/
	CInstructionTemplate getObjectMaxHealth(new Script_getObjectMaxHealth, scriptEngine, "getObjectMaxHealth", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "NUM|OBJECT");
	se.AddInstructionTemplate(getObjectMaxHealth);

	/**
	 *	getObjectHealthPercent
	 **/
	CInstructionTemplate getObjectHealthPercent(new Script_getObjectHealthPercent, scriptEngine, "getObjectHealthPercent", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "NUM|OBJECT");
	se.AddInstructionTemplate(getObjectHealthPercent);

	/**
	 *	setObjectInvulnerable
	 **/
	CInstructionTemplate setObjectInvulnerable(new Script_setObjectInvulnerable, scriptEngine, "setObjectInvulnerable", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "OBJECT|BOOL");
	se.AddInstructionTemplate(setObjectInvulnerable);
}





////////////////////////////
/// Instruction handlers ///
////////////////////////////

/**
 * Create object
 **/

ScriptReturn CObjectInstructions::Script_createDynamicObject::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->custom_ref = GetOTE()->GetObjectManager()->DropDynamicObject((long)param[1].num_value, param[2].vec3_value.GetD3DXVECTOR3(), (long)param[3].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_createStaticObject::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->custom_ref = GetOTE()->GetObjectManager()->DropStaticObject((long)param[1].num_value, param[2].vec3_value.GetD3DXVECTOR3(), (long)param[3].num_value);

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_attachLight::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
	{
		IGameObject *ob = (IGameObject*)param[0].custom_ref;
		GetOTE()->GetLightEffectManager()->CreateLightEffect(ob, (long)param[1].num_value);
	}

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_attachParticleEffect::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
	{
		// TODO: improve
		IGameObject *ob = (IGameObject*)param[0].custom_ref;
		GetOTE()->GetParticleEffects()->StartEffect(ob, (long)param[1].num_value);
	}

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_removeLight::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
	{
	}

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_removeParticleEffect::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
	{
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_setController::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
	{
		IGameObject *ob = (IGameObject*)param[0].custom_ref;
		IGameObjectController *c = GetOTE()->GetControllerManager()->GetController((long)param[1].num_value);
		if(c != NULL)
		{
			c->InitObject(ob);
			ob->SetController(c);
			c->SetIsActive(true);
		}
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_activateController::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
	{
		IGameObjectController *c = ((IGameObject*)param[0].custom_ref)->GetController();
		if(c != NULL)
			c->SetIsActive(true);
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_removeController::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
	{
		((IGameObject*)param[0].custom_ref)->SetController(NULL);
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_setObjectControllerParam::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
	{
		IGameObject *ob = (IGameObject*)param[0].custom_ref;
		int index = (int)param[1].num_value;
		if(index < 0 || index > 1)
			return SCRIPT_RETURN_NOVALUE;
		int type = ob->GetType();
		if(type == STATIC_OBJECT_ID)
		{
			((CStaticObject*)ob)->SetInputControlParam(index, (float)param[2].num_value);
		}
		else
		{
			((CDynamicObject*)ob)->SetInputControlParam(index, (float)param[2].num_value);
		}
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_inflictDamage::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[0].custom_ref;
	if(ob != NULL)
	{
		int type = ob->GetType();
		if(type == DYNAMIC_OBJECT_ID || type == SHIP_OBJECT_ID || type == ENEMY_OBJECT_ID)
		{
			((CDynamicObject*)ob)->InflictDamage((float)param[1].num_value, true, (DAMAGE_TYPE)(int)param[2].num_value, NULL);
		}
		else
			return SCRIPT_RETURN_ERROR;
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_getObjectHealth::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[1].custom_ref;
	if(ob != NULL)
	{
		int type = ob->GetType();
		if(type == DYNAMIC_OBJECT_ID || type == SHIP_OBJECT_ID || type == ENEMY_OBJECT_ID)
		{
			modifyVar->num_value = ((CDynamicObject*)ob)->GetHitpoints();
		}
		else
			return SCRIPT_RETURN_ERROR;
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_getObjectMaxHealth::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[1].custom_ref;
	if(ob != NULL)
	{
		int type = ob->GetType();
		if(type == DYNAMIC_OBJECT_ID || type == SHIP_OBJECT_ID || type == ENEMY_OBJECT_ID)
		{
			modifyVar->num_value = ((CDynamicObject*)ob)->GetMaxHitPoints();
		}
		else
			return SCRIPT_RETURN_ERROR;
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_getObjectHealthPercent::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[1].custom_ref;
	if(ob != NULL)
	{
		int type = ob->GetType();
		if(type == DYNAMIC_OBJECT_ID || type == SHIP_OBJECT_ID || type == ENEMY_OBJECT_ID)
		{
			CDynamicObject *dob = (CDynamicObject*)ob;
			modifyVar->num_value = dob->GetHitpoints() / dob->GetMaxHitPoints();
		}
		else
			return SCRIPT_RETURN_ERROR;
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_getObjectName::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[1].custom_ref;
	if(ob)
	{
		//strcpy(modifyVar->str_value, ob-
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_getObjectID::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[1].custom_ref;
	if(ob)
	{
		modifyVar->num_value = ob->GetID();
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_getObjectPosition::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[1].custom_ref;
	if(ob)
	{
		D3DXVECTOR3 v = ob->GetTranslation();
		modifyVar->vec3_value.x = v.x;
		modifyVar->vec3_value.y = v.y;
		modifyVar->vec3_value.z = v.z;
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_setObjectPosition::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[0].custom_ref;
	if(ob)
	{
		ob->SetTranslation(param[1].vec3_value.GetD3DXVECTOR3());
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_getObjectRotation::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[1].custom_ref;
	if(ob)
	{
		D3DXVECTOR3 v = ob->GetRotation();
		modifyVar->vec3_value.x = v.x;
		modifyVar->vec3_value.y = v.y;
		modifyVar->vec3_value.z = v.z;
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_setObjectRotation::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[0].custom_ref;
	if(ob)
	{
		ob->SetRotation(param[1].vec3_value.GetD3DXVECTOR3());
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_getPlayerShip::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	int index = (int)param[1].num_value;
	if(index < 0 || index > 1)
		return SCRIPT_RETURN_NOVALUE;

	modifyVar->custom_ref = (void*)GetOTE()->GetPlayer((unsigned int)index)->GetShip();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_setObjectInvulnerable::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *o = (IGameObject*)param[0].custom_ref;
	if(!o)
		return SCRIPT_RETURN_ERROR;
	int type = o->GetType();
	if(type != ENEMY_OBJECT_ID && type != DYNAMIC_OBJECT_ID && type != SHIP_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;

	((CDynamicObject*)o)->SetIndestructible(param[1].bool_value);

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_countObjects::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(!strncmp(param[1].str_value, "$ENEMY", 7))
	{
		modifyVar->num_value = CEnemy::GetEnemyCount();
	}
	else if(!strncmp(param[1].str_value, "", 1))
	{
		// TODO: count all remaining objects
	}
	else
	{
		// TODO: count specified object type
	}

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_destroyObject::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[0].custom_ref;
	if(ob)
	{
		int type = ob->GetType();
		if(type == STATIC_OBJECT_ID)
		{
			((CStaticObject*)ob)->Destroy();
		}
		else if(type == DYNAMIC_OBJECT_ID || type == SHIP_OBJECT_ID || type == ENEMY_OBJECT_ID)
		{
			((CDynamicObject*)ob)->Kill();
		}
		else
			return SCRIPT_RETURN_ERROR;
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_getObjectByID::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->custom_ref = GetOTE()->GetObjectManagerSceneNode()->GetObject((long)param[1].num_value);

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_getObjectByName::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CObjectInstructions::Script_enableForceUpdate::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[0].custom_ref;
	if(ob)
	{
		ob->SetRequiresConstantUpdate(param[1].bool_value);
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_setStaticObTargetID::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[0].custom_ref;
	if(ob->GetType() != STATIC_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;
	((CStaticObject*)ob)->SetTargetID((long)param[1].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_setStaticObTargetRef::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob = (IGameObject*)param[0].custom_ref;
	if(ob->GetType() != STATIC_OBJECT_ID)
		return SCRIPT_RETURN_ERROR;
	((CStaticObject*)ob)->SetTargetRef(param[1].str_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CObjectInstructions::Script_checkObjectIntersection::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGameObject *ob1 = (IGameObject*)param[1].custom_ref, *ob2 = (IGameObject*)param[2].custom_ref;
	if(ob1 && ob2)
	{
		vector<SCollision> c;
		ob1->CheckForCollision(ob2, c);
		if(c.size() > 0)
			modifyVar->bool_value = true;
		else
			modifyVar->bool_value = false;
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}
