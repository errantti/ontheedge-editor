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

#include "NewLevelWnd.h"
#include "..\\resource.h"
#include "..\\Editor.h"
#include "..\\Databases\\DatabaseUtilities.h"

CNewLevelWnd *CNewLevelWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNewLevelWnd::CNewLevelWnd()
{
	m_pointer = this;
}

CNewLevelWnd::~CNewLevelWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CNewLevelWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CNewLevelWnd *p = CNewLevelWnd::m_pointer;
	//LPSTR str = NULL;
	char str[128];
	int size, height;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hWnd, IDC_SIZEX), "16");
		SetWindowText(GetDlgItem(hWnd, IDC_SIZEY), "16");
		SetWindowText(GetDlgItem(hWnd, IDC_NEWLEVEL_HEIGHT), "3");
		break;
	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_NEWLEVEL_OK:
			// Check that the input is integer
			GetWindowText(GetDlgItem(hWnd, IDC_SIZEX), str, 4);
			size = atoi(str);
			if(size < MIN_LEVEL_DIMENSION)
			{
				::MessageBox(hWnd, "Invalid or too small size", "Error!", MB_OK);
				break;
			}

			GetWindowText(GetDlgItem(hWnd, IDC_NEWLEVEL_HEIGHT), str, 4);
			height = atoi(str);
			if(height < 1 || height > MAX_HEIGHT_LEVEL)
			{
				::MessageBox(hWnd, "Invalid height", "Error!", MB_OK);
				break;
			}

			GetWindowText(GetDlgItem(hWnd, IDC_NEWLEVEL_TILESET), str, 127);

			GetEditorApp()->NewLevel(size, height, str);
			EndDialog(hWnd, FALSE);
			break;
		case ID_NEWLEVEL_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case IDC_SIZEX:
			if((int)HIWORD(wParam) == EN_CHANGE)
			{
				GetWindowText((HWND)lParam, str, 4);
				SetWindowText(GetDlgItem(hWnd, IDC_SIZEY), str);
			}
			break;
		case ID_NEWLEVEL_BROWSE:
			p->GetTileset(hWnd);
			break;
		}
		break;
	}

	return FALSE;
}

bool CNewLevelWnd::GetTileset(HWND hWnd)
{
	std::string filename;
	if(!CDatabaseUtilities::GetTilesetFilename(hWnd, filename))
		return false;

	SetWindowText(GetDlgItem(hWnd, IDC_NEWLEVEL_TILESET), filename.c_str());
	EnableWindow(GetDlgItem(hWnd, ID_NEWLEVEL_OK), TRUE);

	return true;
}

void CNewLevelWnd::Open(HWND parent, bool bModal)
{
	if(bModal)
	{
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_NEWLEVEL), parent, WndProc);
	}
	else
	{
		CreateDialog(NULL, MAKEINTRESOURCE(IDD_NEWLEVEL), parent, WndProc);
	}
}