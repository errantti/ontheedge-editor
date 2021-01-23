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

#include "GadgetInstructions.h"
#include "..\\OnTheEdge.h"

void CGadgetInstructions::AddGadgetCommands(CScriptEngine *scriptEngine, int &id)
{
	if(scriptEngine == NULL)
		return;

	CScriptEngine &se = *scriptEngine;

	/**
	 *	getEquippedGadget
	 **/
	CInstructionTemplate getEquippedGadget(new Script_getEquippedGadget, scriptEngine, "getEquippedGadget", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "GADGET|NUM|NUM");
	se.AddInstructionTemplate(getEquippedGadget);

	/**
	 *	getGadgetName
	 **/
	CInstructionTemplate getGadgetName(new Script_getGadgetName, scriptEngine, "getGadgetName", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "STR|GADGET");
	se.AddInstructionTemplate(getGadgetName);

	/**
	 *	getFirstGadget
	 **/
	CInstructionTemplate getFirstGadget(new Script_getFirstGadget, scriptEngine, "getFirstGadget", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "GADGET|NUM");
	se.AddInstructionTemplate(getFirstGadget);

	/**
	 *	getInventoryGadget
	 **/
	CInstructionTemplate getInventoryGadget(new Script_getInventoryGadget, scriptEngine, "getInventoryGadget", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "GADGET|NUM|NUM");
	se.AddInstructionTemplate(getInventoryGadget);

	/**
	 *	removeGadget
	 **/
	CInstructionTemplate removeGadget(new Script_removeGadget, scriptEngine, "removeGadget", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "GADGET|NUM");
	se.AddInstructionTemplate(removeGadget);

	/**
	 *	createGadget
	 **/
	CInstructionTemplate createGadget(new Script_createGadget, scriptEngine, "createGadget", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "GADGET|NUM|NUM|BOOL");
	se.AddInstructionTemplate(createGadget);

	/**
	 *	removeGadgetID
	 **/
	CInstructionTemplate removeGadgetID(new Script_removeGadgetID, scriptEngine, "removeGadgetID", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "NUM|NUM");
	se.AddInstructionTemplate(removeGadgetID);

	/**
	 *	getGadgetID
	 **/
	CInstructionTemplate getGadgetID(new Script_getGadgetID, scriptEngine, "getGadgetID", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "NUM|GADGET");
	se.AddInstructionTemplate(getGadgetID);

	/**
	 * ITEMS
	 **/

	/**
	 *	getQuestItem
	 **/
	CInstructionTemplate getQuestItem(new Script_getQuestItem, scriptEngine, "getQuestItem", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "ITEM|NUM");
	se.AddInstructionTemplate(getQuestItem);

	/**
	 *	addQuestItem
	 **/
	CInstructionTemplate addQuestItem(new Script_addQuestItem, scriptEngine, "addQuestItem", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "ITEM|NUM|BOOL");
	se.AddInstructionTemplate(addQuestItem);

	/**
	 *	addShipPatch
	 **/
	CInstructionTemplate addShipPatch(new Script_addShipPatch, scriptEngine, "addShipPatch", 
		INSTRUCTION_TYPE_EXECUTE, id++, "NUM|BOOL");
	se.AddInstructionTemplate(addShipPatch);

	/**
	 *	addComponent
	 **/
	CInstructionTemplate addComponent(new Script_addComponent, scriptEngine, "addComponent", 
		INSTRUCTION_TYPE_EXECUTE, id++, "NUM|BOOL");
	se.AddInstructionTemplate(addComponent);

	/**
	 *	removeItem
	 **/
	CInstructionTemplate removeItem(new Script_removeItem, scriptEngine, "removeItem", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "ITEM");
	se.AddInstructionTemplate(removeItem);
}





////////////////////////////
/// Instruction handlers ///
////////////////////////////

ScriptReturn CGadgetInstructions::Script_getEquippedGadget::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CPlayer *p = GetOTE()->GetPlayer((unsigned int)param[2].num_value);
	if(p)
	{
		modifyVar->custom_ref = p->GetEquippedGadget((int)param[1].num_value);
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGadgetInstructions::Script_getGadgetName::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGadget *gadget = (IGadget*)param[1].custom_ref;
	if(gadget)
	{
		strcpy(modifyVar->str_value, gadget->GetName().c_str());
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGadgetInstructions::Script_getFirstGadget::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CPlayer *p = GetOTE()->GetPlayer((unsigned int)param[1].num_value);
	if(p)
	{
		modifyVar->custom_ref = p->GetFirstGadgetInInventory();
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGadgetInstructions::Script_getInventoryGadget::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CPlayer *p = GetOTE()->GetPlayer((unsigned int)param[2].num_value);
	if(p)
	{
		modifyVar->custom_ref = p->GetGadgetInInventory((long)param[1].num_value);
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGadgetInstructions::Script_removeGadget::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGadget *gadget = (IGadget*)param[0].custom_ref;
	CPlayer *p = GetOTE()->GetPlayer((unsigned int)param[1].num_value);
	if(p && gadget)
	{
		p->RemoveGadget(gadget->GetID());
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGadgetInstructions::Script_createGadget::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CPlayer *p = GetOTE()->GetPlayer((unsigned int)param[1].num_value);
	if(p)
	{
		p->AddGadget((long)param[2].num_value, param[3].bool_value);
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGadgetInstructions::Script_removeGadgetID::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CPlayer *p = GetOTE()->GetPlayer((unsigned int)param[1].num_value);
	if(p)
	{
		p->RemoveGadget((long)param[0].num_value);
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGadgetInstructions::Script_getGadgetID::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IGadget *gadget = (IGadget*)param[1].custom_ref;
	if(gadget)
	{
		modifyVar->num_value = gadget->GetID();
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}


ScriptReturn CGadgetInstructions::Script_getQuestItem::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CPlayer *p = GetOTE()->GetPlayer(0);
	if(p)
	{
		modifyVar->custom_ref = p->GetItemInInventory(INVENTORY_ITEM_QUESTITEM, (long)param[1].num_value);
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGadgetInstructions::Script_addQuestItem::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CPlayer *p = GetOTE()->GetPlayer(0);
	if(p)
	{
		modifyVar->custom_ref = p->AddQuestItem((long)param[1].num_value, param[2].bool_value);
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGadgetInstructions::Script_addShipPatch::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CPlayer *p = GetOTE()->GetPlayer(0);
	if(p)
	{
		p->AddPatch((long)param[0].num_value, param[1].bool_value);
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGadgetInstructions::Script_addComponent::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CPlayer *p = GetOTE()->GetPlayer(0);
	if(p)
	{
		p->AddComponent((long)param[0].num_value, param[1].bool_value);
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CGadgetInstructions::Script_removeItem::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	IInventoryItem *item = (IInventoryItem*)param[0].custom_ref;
	CPlayer *p = GetOTE()->GetPlayer(0);
	if(item && p)
	{
		p->RemoveItem(item);
	}
	else
		return SCRIPT_RETURN_ERROR;

	return SCRIPT_RETURN_NOVALUE;
}