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

#include "DatabaseUtilities.h"
#include "SelectionWnd.h"
#include "TreeSelectionWnd.h"
#include "..\\Editor.h"
#include <sstream>

using namespace std;

bool CDatabaseUtilities::SelectParticleEffect(HWND hWnd, long &selection)
{
	CTreeSelectionWnd s;
	long selected;
	
	std::vector<SParticleSystemInfo> *effects = GetEditorApp()->GetParticleEffects()->GetEffects();
	for(DWORD i=0;i<effects->size();i++)
		s.AddEntry(SSelectionEntry((*effects)[i].name, (*effects)[i].ID, (*effects)[i].category));

	if(s.OpenWindow(hWnd, selected, "particle effect"))
	{
		selection = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectLightEffect(HWND hWnd, long &selection)
{
	CSelectionWnd s;
	long selected;
	string str;
	
	std::vector<SDynamicPointLightParams> &effects = CLightEffectManager::GetManager().GetEffects();
	std::vector<SDynamicPointLightParams>::iterator it = effects.begin();
	for(; it != effects.end(); ++it)
	{
		stringstream num;
		num << it->id;
		str = num.str();
		str += " - ";
		str += it->name;
		s.AddEntry(SSelectionEntry(str, it->id));
	}

	if(s.OpenWindow(hWnd, selected, "light effect"))
	{
		selection = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectGadget(HWND hWnd, long &selection)
{
	CTreeSelectionWnd s;
	long selected;
	
	//std::vector<SGadgetTemplate> *gadgets = GetEditorApp()->GetGadgetDatabase()->GetGadgetTemplates();
	vector< pair<SGadgetTemplate, SCustomGadgetTemplate*> > *gadgets = GetEditorApp()->GetGadgetDatabase()->GetGadgetTemplates();
	vector< pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = gadgets->begin();

	std::string str;
	for(; it != gadgets->end(); ++it)
	{
		if(it->first.useFlags & GADGET_USABLE_ENEMY)
			str = "Enemy gadgets";
		else if(it->first.gadgetType == GADGET_TYPE_WEAPON)
			str = "Weapons";
		else if(it->first.gadgetType == GADGET_TYPE_KINETIC)
			str = "Kinetic guns";
		else if(it->first.gadgetType == GADGET_TYPE_HOVER)
			str = "Hover engines";
		else if(it->first.gadgetType == GADGET_TYPE_SHIELD)
			str = "Shields";
		else if(it->first.gadgetType == GADGET_TYPE_STEALTH)
			str = "Stealth devices";
		else if(it->first.gadgetType == GADGET_TYPE_POSSESS)
			str = "Mind control devices";
		else if(it->first.gadgetType == GADGET_TYPE_PULLRAY)
			str = "Pull rays";
		else
			str = "Default";

		s.AddEntry(SSelectionEntry(it->first.name, it->first.ID, str));
	}

	if(s.OpenWindow(hWnd, selected, "gadget"))
	{
		selection = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectAmmo(HWND hWnd, long &ID)
{
	CSelectionWnd s;
	long selected;
	string str;
	
	std::vector<SAmmoTemplate> *ammos = GetEditorApp()->GetAmmoDatabase()->GetAmmoTemplates();
	for(DWORD i=0;i<ammos->size();i++)
	{
		stringstream num;
		num << (*ammos)[i].ID;
		str = num.str();
		str += " - ";
		str += (*ammos)[i].name;
		s.AddEntry(SSelectionEntry(str.c_str(), (*ammos)[i].ID));
	}

	if(s.OpenWindow(hWnd, selected, "ammo"))
	{
		ID = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectShip(HWND hWnd, long &ID)
{
	CSelectionWnd s;
	long selected;
	string str;
	
	std::vector<SShipTemplate> *ships = GetEditorApp()->GetObjectManager()->GetShipTemplates();
	for(DWORD i=0;i<ships->size();i++)
	{
		stringstream num;
		num << (*ships)[i].vt.dt.templateID;
		str = num.str();
		str += " - ";
		str += (*ships)[i].vt.dt.name;
		s.AddEntry(SSelectionEntry(str.c_str(), (*ships)[i].vt.dt.templateID));
	}

	if(s.OpenWindow(hWnd, selected, "ship"))
	{
		ID = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectWaypointSet(HWND hWnd, int &ID)
{
	CSelectionWnd s;
	long selected;
	string str;
	
	std::vector<CWayPointSet*> *wps = GetEditorApp()->GetCurrentLevel()->GetWaypointSets();
	std::vector<CWayPointSet*>::iterator it = wps->begin();
	for(; it != wps->end(); ++it)
	{
		stringstream num;
		num << (*it)->GetID();
		str = num.str();
		str += " - ";
		str += (*it)->GetName();
		s.AddEntry(SSelectionEntry(str.c_str(), (*it)->GetID()));
	}

	if(s.OpenWindow(hWnd, selected, "waypoint set"))
	{
		ID = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectDialogue(HWND hWnd, int &ID)
{
	CSelectionWnd s;
	long selected;
	string str;
	
	std::vector<CDialogue*> *d = GetEditorApp()->GetCurrentLevel()->GetDialogues();
	std::vector<CDialogue*>::iterator it = d->begin();
	for(; it != d->end(); ++it)
	{
		stringstream num;
		num << (*it)->GetID();
		str = num.str();
		str += " - ";
		str += (*it)->GetName();
		s.AddEntry(SSelectionEntry(str.c_str(), (*it)->GetID()));
	}

	if(s.OpenWindow(hWnd, selected, "dialogue"))
	{
		ID = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectAssistantMsg(HWND hWnd, int &ID)
{
	CSelectionWnd s;
	long selected;
	string str;

	std::vector<std::pair<int,std::string> > *msgs = GetOTE()->GetCurrentLevel()->GetAssistentMessages();
	std::vector<std::pair<int,std::string> >::iterator it = msgs->begin();
	int size;
	for(; it != msgs->end(); ++it)
	{
		stringstream num;

		size = (int)it->second.size();
		num << it->first;
		str = num.str();
		str += " - ";

		if(size < 40)
			str += it->second;
		else
		{
			str += it->second.substr(0, 37);
			str += "...";
		}
		s.AddEntry(SSelectionEntry(str, it->first));
	}
	
	if(s.OpenWindow(hWnd, selected, "assistant msg"))
	{
		ID = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectTerminal(HWND hWnd, long &ID)
{
	CSelectionWnd s;
	long selected;
	string str;
	
	std::vector<CTerminal*> *d = GetEditorApp()->GetCurrentLevel()->GetTerminals();
	std::vector<CTerminal*>::iterator it = d->begin();
	for(; it != d->end(); ++it)
	{
		stringstream num;
		num << (*it)->GetID();
		str = num.str();
		str += " - ";
		str += (*it)->GetName();
		s.AddEntry(SSelectionEntry(str.c_str(), (*it)->GetID()));
	}

	if(s.OpenWindow(hWnd, selected, "terminal"))
	{
		ID = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectLocalScript(HWND hWnd, std::string &script)
{
	/*CSelectionWnd s;
	long selected;
	
	std::vector<CScript*> *d = &CScriptManager::GetManager().GetScriptEngine().GetCurrentScriptCollection();
	std::vector<CScript*>::iterator it = d->begin();
	for(selected=0; it != d->end(); ++it, ++selected)
		if((*it)->IsTemporary())
			s.AddEntry(SSelectionEntry((*it)->GetName().c_str(), selected));

	if(s.OpenWindow(hWnd, selected, "level script", &script))
		return true;

	return false;*/
	return SelectScript(hWnd, script, false, true);
}

bool CDatabaseUtilities::SelectGlobalScript(HWND hWnd, std::string &script)
{
	/*CSelectionWnd s;
	long selected;
	
	std::vector<CScript*> *d = &CScriptManager::GetManager().GetScriptEngine().GetCurrentScriptCollection();
	std::vector<CScript*>::iterator it = d->begin();
	for(selected=0; it != d->end(); ++it, ++selected)
		if(!(*it)->IsTemporary())
			s.AddEntry(SSelectionEntry((*it)->GetName().c_str(), selected));

	if(s.OpenWindow(hWnd, selected, "global script", &script))
		return true;

	return false;*/
	return SelectScript(hWnd, script, true, false);
}

bool CDatabaseUtilities::SelectScript(HWND hWnd, std::string &script, bool bIncludeGlobals, bool bIncludeTemporary)
{
	CSelectionWnd s;
	long selected;
	
	std::vector<CScript*> *d = &CScriptManager::GetManager().GetScriptEngine().GetCurrentScriptCollection();
	std::vector<CScript*>::iterator it = d->begin();
	for(selected=0; it != d->end(); ++it, ++selected)
	{
		if((*it)->IsTemporary())
		{
			if(!bIncludeTemporary)
				continue;
		}
		else
		{
			if(!bIncludeGlobals)
				continue;
		}
		s.AddEntry(SSelectionEntry((*it)->GetName().c_str(), selected));
	}

	if(s.OpenWindow(hWnd, selected, "global script", &script))
		return true;

	return false;
}

bool CDatabaseUtilities::SelectSound(HWND hWnd, long &ID)
{
	CSelectionWnd s;
	long selected;
	string str;
	
	std::vector<CSoundManager::SSoundTemplate> *d = &CSoundManager::GetManager().GetSoundTemplateDatabase();
	std::vector<CSoundManager::SSoundTemplate>::iterator it = d->begin();
	for(; it != d->end(); ++it)
	{
		stringstream num;
		num << it->id;
		str = num.str();
		str += " - ";
		str += it->name;
		s.AddEntry(SSelectionEntry(str.c_str(), it->id));
	}

	if(s.OpenWindow(hWnd, selected, "sound"))
	{
		ID = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectComponent(HWND hWnd, long &ID)
{
	CSelectionWnd s;
	long selected;
	string str;
	
	std::vector<SGadgetComponentTemplate> &d = CItemManager::GetManager().GetComponentTemplates();
	std::vector<SGadgetComponentTemplate>::iterator it = d.begin();
	for(; it != d.end(); ++it)
	{
		stringstream num;
		num << it->id;
		str = num.str();
		str += " - ";
		str += it->name;
		s.AddEntry(SSelectionEntry(str.c_str(), it->id));
	}

	if(s.OpenWindow(hWnd, selected, "gadget component"))
	{
		ID = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectQuestItem(HWND hWnd, long &ID)
{
	CSelectionWnd s;
	long selected;
	string str;
	
	std::vector<SQuestItemTemplate> &d = CItemManager::GetManager().GetQuestItemTemplates();
	std::vector<SQuestItemTemplate>::iterator it = d.begin();
	for(; it != d.end(); ++it)
	{
		stringstream num;
		num << it->id;
		str = num.str();
		str += " - ";
		str += it->name;
		s.AddEntry(SSelectionEntry(str.c_str(), it->id));
	}

	if(s.OpenWindow(hWnd, selected, "quest item"))
	{
		ID = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectShipPatch(HWND hWnd, long &ID)
{
	CSelectionWnd s;
	long selected;
	string str;
	
	std::vector<SShipPatchTemplate> &d = CItemManager::GetManager().GetPatchTemplates();
	std::vector<SShipPatchTemplate>::iterator it = d.begin();
	for(; it != d.end(); ++it)
	{
		stringstream num;
		num << it->id;
		str = num.str();
		str += " - ";
		str += it->name;
		s.AddEntry(SSelectionEntry(str.c_str(), it->id));
	}

	if(s.OpenWindow(hWnd, selected, "ship patch"))
	{
		ID = selected;
		return true;
	}

	return false;
}

bool CDatabaseUtilities::SelectController(HWND hWnd, long &ID)
{
	CSelectionWnd s;
	long selected;

	std::map<long, IGameObjectController*> *controllers = CControllerManager::GetManager().GetControllerTemplates();

	std::map<long, IGameObjectController*>::iterator it = controllers->begin();
	for(; it != controllers->end(); ++it)
		s.AddEntry(SSelectionEntry(it->second->GetControllerName(), it->first));

	if(s.OpenWindow(hWnd, selected, "controller"))
	{
		ID = selected;
		return true;
	}

	return false;
}


bool CDatabaseUtilities::GetFullExportPath(HWND hWnd, std::string &outStr, std::string fileExt)
{
	OPENFILENAME ofn;

	char filename[256];
	char f[128];
	char szExtension[16];
	char filter[128];
	filename[0] = '\0';
	strncpy(szExtension, fileExt.c_str(), 15);
	sprintf(filter, "On the Edge object specific export file (*.%s)", szExtension); // \0\0*.%s\0\0
	int len = strlen(filter);
	filter[len] = '\0';
	sprintf(&filter[len+1], "*.%s", szExtension);
	len += 3 + (int)fileExt.size();
	filter[len] = '\0';
	filter[len+1] = '\0';

	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = filename;
	ofn.lpstrFileTitle = f;
	ofn.lpstrFilter = filter;
	ofn.lpstrDefExt = szExtension;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.nMaxFile = 127;
	ofn.nMaxFileTitle = 63;
	ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_SHAREAWARE | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.lpstrInitialDir = "editorexports";

	if(!GetSaveFileName(&ofn))
		return false;

	outStr = filename;

	/*bool dotFound = false;
	for(int i=strlen(filename); i>0; --i)
		if(filename[i] == '.')
		{
			dotFound = true;
			break;
		}

	if(dotFound)
	{
		filename[i+1] = 'o';
		filename[i+2] = 'e';
		filename[i+3] = 'e';
		filename[i+4] = '\0';
		outStr = filename;
	}
	else
	{
	}*/
	int index = -1;
	if((index = (int)outStr.find('.')) >= 0)
	{
		outStr.replace(index, outStr.size()-1, "");
	}
	outStr += ".";
	outStr += fileExt;

	return true;
}

bool CDatabaseUtilities::GetFullImportPath(HWND hWnd, std::string &outStr, std::string fileExt)
{
	OPENFILENAME ofn;

	char tempFile[256] = "";
	char filename[256] = "";
	filename[0] = '\0';
	char szExtension[16];
	char filter[128];
	filename[0] = '\0';
	strncpy(szExtension, fileExt.c_str(), 15);
	sprintf(filter, "On the Edge object specific export file (*.%s)", szExtension); // \0\0*.%s\0\0
	int len = strlen(filter);
	filter[len] = '\0';
	sprintf(&filter[len+1], "*.%s", szExtension);
	len += 3 + (int)fileExt.size();
	filter[len] = '\0';
	filter[len+1] = '\0';

	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = filename;
	ofn.lpstrFileTitle = tempFile;
	ofn.lpstrFilter = filter;
	ofn.lpstrDefExt = szExtension;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.nMaxFile = 255;
	ofn.nMaxFileTitle = 63;
	ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_SHAREAWARE | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.lpstrInitialDir = "editorexports";

	if(!GetOpenFileName(&ofn))
		return false;

	outStr = filename;

	return true;
}