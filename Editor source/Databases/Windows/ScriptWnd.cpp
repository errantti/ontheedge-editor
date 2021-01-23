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

#include "ScriptWnd.h"
#include "..\\..\\resource.h"
#include "..\\..\\OtE\\OnTheEdge.h"
#include "..\\DatabaseUtilities.h"
#include "..\\TreeSelectionWnd.h"
#include "..\\SelectionWnd.h"

#include <fstream>
using namespace std;

CScriptWnd *CScriptWnd::m_pointer = NULL;
bool CScriptWnd::m_bAutosaveScript = TRUE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScriptWnd::CScriptWnd() : m_bModified(FALSE), m_hWnd(NULL), m_numMessages(0)
{
	m_pointer = this;
	CScriptManager::GetManager().GetScriptEngine().SetLogWriter(this);
}

CScriptWnd::~CScriptWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CScriptWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CScriptWnd *p = CScriptWnd::m_pointer;
	CScript &s = *(p->m_template);
	p->m_hWnd = hWnd;

	char szTemp[256];
	std::string str;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_NAME), s.GetName().c_str());
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_FILENAME), s.GetFilename().c_str());

		if(p->m_bNew)
			EnableWindow(GetDlgItem(hWnd, IDC_SCRIPT_LOADBINARY), FALSE);

		if(s.GetFilename().compare(""))
			EnableWindow(GetDlgItem(hWnd, IDC_SCRIPT_SAVEFILE), TRUE);

		/*if(p->ImportFromScript(hWnd))
			p->AddMessage("Script loaded from database");
		else
			p->AddMessage("Failed loading script from database");*/

		if(p->Import(s.GetFilename(), hWnd, false))
			p->AddMessage("Script loaded from text file");
		else
		{
			p->AddMessage("Failed loading script from text file. Loading from database...");
			if(!p->ImportFromScript(hWnd))
				p->AddMessage("Oh well, that failed as well. Shit happens");
		}

		if(CScriptWnd::m_bAutosaveScript)
			SendMessage(GetDlgItem(hWnd, IDC_SCRIPT_CONFIRM), BM_SETCHECK, 1, 0);

		break;

	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_SCRIPT_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case IDC_SCRIPT_SAVE:
			CScriptWnd::m_bAutosaveScript = false;
			if(SendMessage(GetDlgItem(hWnd, IDC_SCRIPT_CONFIRM), BM_GETCHECK, 0, 0))
				CScriptWnd::m_bAutosaveScript = true;

			// Check whether saving criteria are fulfilled
			GetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_NAME), szTemp, 63);
			if(strlen(szTemp) < 3)
			{
				p->AddMessage("Failed to save script: too short script name");
				break;
			}

			GetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_FILENAME), szTemp, 255);
			p->ClearMessages();
			if(!strcmp(szTemp, ""))
			{
				p->AddMessage("You cannot save script into database before saving the script code into");
				p->AddMessage("an external file by using export");
				break;;
			}

			if(CScriptWnd::m_bAutosaveScript && !p->m_bSavedToFile)
			{
				/*result = MessageBox(hWnd, "You haven't saved the altered script code to an external file. Are you sure you want to leave?", "Inquiry", MB_YESNO);
				if(result == IDNO)
					break;*/
				p->SaveToFile(hWnd);
			}

			if(!p->Validate(hWnd))
			{
				p->AddMessage("You cannot save invalid script");
				break;
			}

			p->m_bModified = TRUE;

			GetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_NAME), szTemp, 63);
			s.SetName(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_FILENAME), szTemp, 255);
			s.SetFilename(szTemp);

			// Save the script itself
			if(!p->Validate(hWnd, true))
			{
				p->AddMessage("Can't save invalid script");
				break;
			}

			EndDialog(hWnd, FALSE);
			break;

		case IDC_SCRIPT_IMPORT:
			p->Import(hWnd);
			break;

		case IDC_SCRIPT_EXPORT:
			p->Export(hWnd);
			break;

		case IDC_SCRIPT_SAVEFILE:
			p->SaveToFile(hWnd);
			break;

		case IDC_SCRIPT_VALIDATE:
			p->Validate(hWnd);
			break;

		case IDC_SCRIPT_LOADBINARY:
			p->ClearMessages();
			if(p->ImportFromScript(hWnd))
				p->AddMessage("Script loaded from binary");
			else
				p->AddMessage("Failed loading script from binary");
			break;

		case IDC_SCRIPT_LOADTEXT:
			p->ClearMessages();

			GetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_FILENAME), szTemp, 255);
			if(!strcmp(szTemp, ""))
			{
				p->AddMessage("Script doesn't have original source (txt file containing the code)");
				break;
			}

			if(p->Import(szTemp, hWnd, true))
				p->AddMessage("Script loaded from original text code file");
			break;

		case IDC_SCRIPT_SCRIPT:
			if(HIWORD(wParam) == EN_CHANGE)
				p->m_bSavedToFile = false;
			break;

		// Helper select buttons
		case IDC_TILE_SELECT_DYNAMIC:
			p->SelectDynamic(hWnd);
			break;
		case IDC_TILE_SELECT_STATIC:
			p->SelectStatic(hWnd);
			break;
		case IDC_TILE_SELECT_ENEMY:
			p->SelectEnemy(hWnd);
			break;
		case IDC_TILE_SELECT_GADGET:
			p->SelectGadget(hWnd);
			break;
		case IDC_TILE_SELECT_COMPONENT:
			p->SelectComponent(hWnd);
			break;
		case IDC_TILE_SELECT_PATCH:
			p->SelectPatch(hWnd);
			break;
		case IDC_TILE_SELECT_QUESTITEM:
			p->SelectQuestItem(hWnd);
			break;
		case IDC_TILE_SELECT_PEFFECT:
			p->SelectPEffect(hWnd);
			break;
		case IDC_TILE_SELECT_LEFFECT:
			p->SelectLEffect(hWnd);
			break;
		case IDC_TILE_SELECT_SOUND:
			p->SelectSound(hWnd);
			break;
		case IDC_TILE_SELECT_GLOBAL_SCRIPT:
			p->SelectGlobalScript(hWnd);
			break;
		case IDC_TILE_SELECT_LOCAL_SCRIPT:
			p->SelectLocaScript(hWnd);
			break;
		case IDC_TILE_SELECT_DIALOGUE:
			p->SelectDialogue(hWnd);
			break;
		case IDC_TILE_SELECT_ASSISTANTMSG:
			p->SelectAssistantMsg(hWnd);
			break;
		case IDC_TILE_SELECT_TERMINAL:
			p->SelectTerminal(hWnd);
			break;
		case IDC_TILE_SELECT_ROUTE:
			p->SelectRoute(hWnd);
			break;
		case IDC_TILE_SELECT_CONTROLLER:
			p->SelectController(hWnd);
			break;
		}
		break;
	}

	return FALSE;
}

void CScriptWnd::ClearMessages()
{
	SendMessage(GetDlgItem(m_hWnd, IDC_SCRIPT_OUTPUT), LB_RESETCONTENT, 0, 0);
	m_numMessages = 0;
}

void CScriptWnd::AddMessage(const std::string msg)
{
	SendMessage(GetDlgItem(m_hWnd, IDC_SCRIPT_OUTPUT), LB_INSERTSTRING, m_numMessages, (LPARAM)msg.c_str());
	++m_numMessages;
}

void CScriptWnd::Log(const std::string text)
{
	AddMessage(text);
}

void CScriptWnd::LogError(const std::string errorMsg)
{
	AddMessage(errorMsg);
}

bool CScriptWnd::Validate(HWND hWnd, bool save)
{
	ClearMessages();

	int len = GetWindowTextLength(GetDlgItem(hWnd, IDC_SCRIPT_SCRIPT));
	len += 2;
	if(len < 5)
	{
		AddMessage("Failed validation: too short script");
		return false;
	}

	char *buffer = new char[len];
	GetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SCRIPT), buffer, len-1);

	// Parse the script
	char lineBuffer[512];
	
	CScriptParser parser(&CScriptManager::GetManager().GetScriptEngine());
	if(save)
		parser.BeginParse(m_template);
	else
		parser.BeginParse("defaultscript");

	int lineIndex = 0;
	bool failed = false;

	for(int i=0; i<len; ++i)
	{
		if(buffer[i] == '\n')
		{
			if(lineIndex > 0)
			{
				lineBuffer[lineIndex] = '\0';
				if(parser.ParseLine(lineBuffer) == false)
				{
					failed = true;
					break;
				}
			}
			lineIndex = 0;
		}
		else if(buffer[i] != '\r')
		{
			lineBuffer[lineIndex] = buffer[i];
			++lineIndex;
		}
	}

	if(lineIndex > 0)
	{
		lineBuffer[lineIndex] = '\0';
		if(parser.ParseLine(lineBuffer) == false)
			failed = true;
	}

	// Delete the buffer used to temporarily store the script code
	delete [] buffer;

	CScript *s = parser.EndParse();
	if(failed || s == NULL)
	{
		AddMessage("Failed parsing the script");
		if(!save)
			delete s;
		return false;
	}

	AddMessage("Script succesfully parsed");

	// Validate the script
	int lineNumber = 0;
	if(s->Validate(lineNumber) == false)
	{
		AddMessage("Failed validating the script");
		if(!save)
			delete s;
		return false;
	}

	AddMessage("Script validated succesfully");

	if(!save)
		delete s;

	return true;
}

bool CScriptWnd::Import(HWND hWnd)
{
	string filename;
	string strFolder(GetOTE()->GetCurrentCampaignFolder());
	strFolder += "scripts\\";
	if(CDatabaseUtilities::GetScriptFilename(hWnd, filename, strFolder) == false)
		return false;

	SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_FILENAME), filename.c_str());
	EnableWindow(GetDlgItem(hWnd, IDC_SCRIPT_SAVEFILE), TRUE);

	return Import(filename, hWnd, true);
}

bool CScriptWnd::Import(string filename, HWND hWnd, bool notifyFailure)
{
	ifstream f(filename.c_str());
	if(!f.is_open())
	{
		if(notifyFailure)
			AddMessage("Failed importing script: couldn't open script file");
		return false;
	}

	char *buffer = new char[1024];
	char lineBuffer[256];
	int size = 1024;
	int index = 0;
	while(f.peek() != EOF)
	{
		f.getline(lineBuffer, 255);
		int count = (int)strlen(lineBuffer);
		if(index + count + 3 > size)
		{
			int oldsize = size;
			size *= 2;
			char *old = buffer;
			buffer = new char[size];
			for(int i = 0; i < index; ++i)
				buffer[i] = old[i];
			delete [] old;
		}
		for(int i=0; i<count; ++i)
		{
			buffer[index] = lineBuffer[i];
			index++;
		}
		buffer[index++] = '\r';
		buffer[index++] = '\n';
	}
	buffer[index] = '\0';
	f.close();

	SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SCRIPT), buffer);

	delete [] buffer;

	return true;
}

bool CScriptWnd::ImportFromScript(HWND hWnd)
{
	if(m_template == NULL)
		return false;

	string script;
	m_template->ConstructText(script, "\r\n");
	//script.rfind

	m_bSavedToFile = true;

	return SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SCRIPT), script.c_str());
}

bool CScriptWnd::Export(HWND hWnd)
{
	ClearMessages();
	if(GetWindowTextLength(GetDlgItem(hWnd, IDC_SCRIPT_SCRIPT)) < 5)
	{
		AddMessage("Can't export script: too short");
		return false;
	}

	OPENFILENAME ofn;
	std::string strFilename;

	char filename[256];
	GetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_FILENAME), filename, 255);

	std::string strFolder(GetOTE()->GetCurrentCampaignFolder());
	strFolder += "scripts\\";

	char szFolder[256];
	strncpy(szFolder, strFolder.c_str(), 255);

	if(strlen(filename) < 1)
		GetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_NAME), filename, 63);

	strFilename = filename;
	int index;
	if((index = (int)strFilename.rfind('\\')) >= 0)
	{
		strFilename = strFilename.substr(index+1);
		strncpy(filename, strFilename.c_str(), 63);
	}

	strFilename = strFolder + strFilename;
	char szFile[256];
	strncpy(szFile, strFilename.c_str(), 255);

	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFileTitle = filename;
	ofn.lpstrFilter = "On the Edge text script (*.txt)\0*.TXT;\0\0";
	ofn.lpstrDefExt = "txt";
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.nMaxFile = 255;
	ofn.nMaxFileTitle = 63;
	ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_SHAREAWARE | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.lpstrInitialDir = szFolder;

	if(!GetSaveFileName(&ofn))
		return false;

	strFilename = strFolder;
	strFilename += ofn.lpstrFileTitle;

	if((int)strFilename.find(".txt") < 0)
		strFilename += ".txt";

	SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_FILENAME), strFilename.c_str());

	return SaveToFile(hWnd);
}

bool CScriptWnd::SaveToFile(HWND hWnd)
{
	char filename[256];
	GetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_FILENAME), filename, 255);

	ofstream f(filename);
	if(!f.is_open())
	{
		AddMessage("Failed saving script: couldn't open file to write");
		return false;
	}

	ClearMessages();

	int len = GetWindowTextLength(GetDlgItem(hWnd, IDC_SCRIPT_SCRIPT));
	len += 2;
	if(len < 5)
	{
		AddMessage("Can't save script: too short script");
		return false;
	}

	char *buffer = new char[len];
	GetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SCRIPT), buffer, len-1);

	// Parse the script
	char lineBuffer[512];
	
	int lineIndex = 0;
	bool failed = false;

	for(int i=0; i<len; ++i)
	{
		if(buffer[i] == '\n')
		{
			if(lineIndex > 0)
			{
				lineBuffer[lineIndex] = '\0';
				f << lineBuffer << endl;
			}
			lineIndex = 0;
		}
		else if(buffer[i] != '\r')
		{
			lineBuffer[lineIndex] = buffer[i];
			++lineIndex;
		}
	}

	if(lineIndex > 0)
	{
		lineBuffer[lineIndex] = '\0';
		f << lineBuffer << endl;
	}

	// Delete the buffer used to temporarily store the script code
	delete [] buffer;

	f.close();
	m_bSavedToFile = true;

	EnableWindow(GetDlgItem(hWnd, IDC_SCRIPT_SAVEFILE), TRUE);
	AddMessage("Script code saved succesfully");

	return true;
}

void CScriptWnd::SelectStatic(HWND hWnd)
{
	CTreeSelectionWnd s;
	long selected;
	char buf[16];
	
	std::vector<SStaticObjectTemplate> *objects = GetOTE()->GetObjectManager()->GetStaticObjectTemplates();
	for(DWORD i=0;i<objects->size();i++)
		s.AddEntry(SSelectionEntry((*objects)[i].name, (*objects)[i].templateID, (*objects)[i].subCategory));

	if(s.OpenWindow(hWnd, selected, "static object"))
	{
		sprintf(buf, "%i", selected);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectDynamic(HWND hWnd)
{
	CSelectionWnd s;
	long selected;
	char buf[16];
	
	std::vector<SDynamicObjectTemplate> *objects = GetOTE()->GetObjectManager()->GetDynamicObjectTemplates();
	for(DWORD i=0;i<objects->size();i++)
		s.AddEntry(SSelectionEntry((*objects)[i].name, (*objects)[i].templateID));

	if(s.OpenWindow(hWnd, selected, "dynamic object"))
	{
		sprintf(buf, "%i", selected);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectEnemy(HWND hWnd)
{
	CSelectionWnd s;
	long selected;
	char buf[16];
	
	std::vector<SEnemyTemplate> *enemies = GetOTE()->GetObjectManager()->GetEnemyTemplates();
	for(DWORD i=0;i<enemies->size();i++)
		s.AddEntry(SSelectionEntry((*enemies)[i].vt.dt.name, (*enemies)[i].vt.dt.templateID));

	if(s.OpenWindow(hWnd, selected, "enemy"))
	{
		sprintf(buf, "%i", selected);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectGadget(HWND hWnd)
{
	long temp;
	char buf[16];
	if(CDatabaseUtilities::SelectGadget(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectPatch(HWND hWnd)
{
	long temp;
	char buf[16];
	if(CDatabaseUtilities::SelectShipPatch(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectComponent(HWND hWnd)
{
	long temp;
	char buf[16];
	if(CDatabaseUtilities::SelectComponent(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectQuestItem(HWND hWnd)
{
	long temp;
	char buf[16];
	if(CDatabaseUtilities::SelectQuestItem(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectPEffect(HWND hWnd)
{
	long temp;
	char buf[16];
	if(CDatabaseUtilities::SelectParticleEffect(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectLEffect(HWND hWnd)
{
	long temp;
	char buf[16];
	if(CDatabaseUtilities::SelectLightEffect(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectSound(HWND hWnd)
{
	long temp;
	char buf[16];
	if(CDatabaseUtilities::SelectSound(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectLocaScript(HWND hWnd)
{
	std::string str;
	if(CDatabaseUtilities::SelectLocalScript(hWnd, str))
	{
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), str.c_str());
	}
}

void CScriptWnd::SelectGlobalScript(HWND hWnd)
{
	std::string str;
	if(CDatabaseUtilities::SelectGlobalScript(hWnd, str))
	{
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), str.c_str());
	}
}

void CScriptWnd::SelectDialogue(HWND hWnd)
{
	int temp;
	char buf[16];
	if(CDatabaseUtilities::SelectDialogue(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectAssistantMsg(HWND hWnd)
{
	int temp;
	char buf[16];
	if(CDatabaseUtilities::SelectAssistantMsg(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectTerminal(HWND hWnd)
{
	long temp;
	char buf[16];
	if(CDatabaseUtilities::SelectTerminal(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectRoute(HWND hWnd)
{
	int temp;
	char buf[16];
	if(CDatabaseUtilities::SelectWaypointSet(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}

void CScriptWnd::SelectController(HWND hWnd)
{
	long temp;
	char buf[16];
	if(CDatabaseUtilities::SelectController(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_SCRIPT_SELECTED), buf);
	}
}


bool CScriptWnd::OpenWindow(HWND parent, CScript &rTemplate, bool bNew)
{
	m_template = &rTemplate;

	m_bNew = bNew;
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SCRIPT), parent, WndProc);

	if(m_bModified)
	{
		//rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
