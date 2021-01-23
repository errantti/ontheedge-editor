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

#include "TileEditWnd.h"
#include "..\\..\\resource.h"
#include "..\\..\\Editor.h"

CTileEditWnd *CTileEditWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTileEditWnd::CTileEditWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CTileEditWnd::~CTileEditWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CTileEditWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CTileEditWnd *p = CTileEditWnd::m_pointer;
	STileInfo &t = p->m_template;
	std::string str;
	char buffer[128];

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		SetWindowText(GetDlgItem(hWnd, IDC_TILE_MESH), t.meshFilename);
		SetWindowText(GetDlgItem(hWnd, IDC_TILE_COLLISION_MESH), t.collisionMeshFilename);
		SetWindowText(GetDlgItem(hWnd, IDC_TILE_TEXTURE), t.overrideTexture);
		SetWindowText(GetDlgItem(hWnd, IDC_TILE_NAME), t.name);
		SetWindowText(GetDlgItem(hWnd, IDC_TILE_SUBCATEGORY), t.subcategory);

		sprintf(buffer, "%i", t.height);
		SetWindowText(GetDlgItem(hWnd, IDC_TILE_HEIGHT_LEVEL), buffer);
		sprintf(buffer, "%f", t.fHeight);
		SetWindowText(GetDlgItem(hWnd, IDC_TILE_HEIGHT), buffer);
		sprintf(buffer, "%f", t.shapeParam);
		SetWindowText(GetDlgItem(hWnd, IDC_TILE_SHAPE_PARAM), buffer);

		// collision type
		switch(t.collisionType)
		{
		case TILE_COLLISION_BOUNDING_BOX:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_BB), BM_SETCHECK, 1, 0);
			break;
		case TILE_COLLISION_RAMP:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_RAMP), BM_SETCHECK, 1, 0);
			break;
		case TILE_COLLISION_NONE:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_NONE), BM_SETCHECK, 1, 0);
			break;
		case TILE_COLLISION_CYLINDER:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_CYLINDERCHECK), BM_SETCHECK, 1, 0);
			break;
		case TILE_COLLISION_ACCURATE:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_ACCURATE), BM_SETCHECK, 1, 0);
			break;
		}

		switch(t.tileType)
		{
		case TILE_TYPE_PLATFORM:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_PLATFORM), BM_SETCHECK, 1, 0); break;
		case TILE_TYPE_PLATFORM_CORNER:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_PLATFORMCORNER), BM_SETCHECK, 1, 0); break;
		case TILE_TYPE_PLATFORM_DOUBLECORNER:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_PLATFORMDCORNER), BM_SETCHECK, 1, 0); break;
		case TILE_TYPE_PLATFORM_DOUBLECORNERCROSS:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_PLATFORM2CORNERCROSS), BM_SETCHECK, 1, 0); break;
			break;
		case TILE_TYPE_PLATFORM_TRIPLECORNER:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_PLATFORM3CORNER), BM_SETCHECK, 1, 0); break;
			break;
		case TILE_TYPE_PLATFORM_FOURCORNER:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_PLATFORM4CORNER), BM_SETCHECK, 1, 0); break;
			break;
		case TILE_TYPE_WALL:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_WALL), BM_SETCHECK, 1, 0); break;
		case TILE_TYPE_WALL_CORNER:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_WALLCORNER), BM_SETCHECK, 1, 0); break;
		case TILE_TYPE_WALL_BLOCK:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_WALLBLOCK), BM_SETCHECK, 1, 0); break;
		case TILE_TYPE_CATWALK:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_CATWALK), BM_SETCHECK, 1, 0); break;
			break;
		case TILE_TYPE_CATWALK_END:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_CATWALKEND), BM_SETCHECK, 1, 0); break;
			break;
		case TILE_TYPE_RAMP:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_RAMPTYPE), BM_SETCHECK, 1, 0); break;
		case TILE_TYPE_HOLE:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_HOLE), BM_SETCHECK, 1, 0); break;
			break;
		case TILE_TYPE_EMPTY:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_EMPTY), BM_SETCHECK, 1, 0); break;
		case TILE_TYPE_DUMMY:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_DUMMY), BM_SETCHECK, 1, 0); break;
		default:
			SendMessage(GetDlgItem(hWnd, IDC_TILE_OTHER), BM_SETCHECK, 1, 0); break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_TILE_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case ID_TILE_OK:
			// Get the form information and return success
			GetWindowText(GetDlgItem(hWnd, IDC_TILE_NAME), t.name, 64);
			if(strlen(t.name) < 3)
			{
				::MessageBox(hWnd, "Too short tile name", "Error", MB_OK);
				break;
			}
			GetWindowText(GetDlgItem(hWnd, IDC_TILE_MESH), t.meshFilename, 128);
			GetWindowText(GetDlgItem(hWnd, IDC_TILE_COLLISION_MESH), t.collisionMeshFilename, 128);
			GetWindowText(GetDlgItem(hWnd, IDC_TILE_TEXTURE), t.overrideTexture, 128);
			GetWindowText(GetDlgItem(hWnd, IDC_TILE_SUBCATEGORY), t.subcategory, 32);

			GetWindowText(GetDlgItem(hWnd, IDC_TILE_HEIGHT_LEVEL), buffer, 2);
			t.height = (UINT)atoi(buffer);
			GetWindowText(GetDlgItem(hWnd, IDC_TILE_HEIGHT), buffer, 12);
			t.fHeight = (float)atof(buffer);
			GetWindowText(GetDlgItem(hWnd, IDC_TILE_SHAPE_PARAM), buffer, 12);
			t.shapeParam = (float)atof(buffer);

			t.collisionType = TILE_COLLISION_BOUNDING_BOX;
			if(SendMessage(GetDlgItem(hWnd, IDC_TILE_BB), BM_GETCHECK, 0, 0))
				t.collisionType = TILE_COLLISION_BOUNDING_BOX;
			if(SendMessage(GetDlgItem(hWnd, IDC_TILE_RAMP), BM_GETCHECK, 0, 0))
				t.collisionType = TILE_COLLISION_RAMP;
			if(SendMessage(GetDlgItem(hWnd, IDC_TILE_NONE), BM_GETCHECK, 0, 0))
				t.collisionType = TILE_COLLISION_NONE;
			if(SendMessage(GetDlgItem(hWnd, IDC_TILE_ACCURATE), BM_GETCHECK, 0, 0))
				t.collisionType = TILE_COLLISION_ACCURATE;
			if(SendMessage(GetDlgItem(hWnd, IDC_TILE_CYLINDERCHECK), BM_GETCHECK, 0, 0))
				t.collisionType = TILE_COLLISION_CYLINDER;

			if(SendMessage(GetDlgItem(hWnd, IDC_TILE_DUMMY), BM_GETCHECK, 0, 0))
			{
				strcpy(t.subcategory, "Dummy");
				t.tileType = TILE_TYPE_DUMMY;
			}
			else
			{
				if(SendMessage(GetDlgItem(hWnd, IDC_TILE_PLATFORM), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Platforms");
					t.tileType = TILE_TYPE_PLATFORM;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_PLATFORMCORNER), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Platform corners");
					t.tileType = TILE_TYPE_PLATFORM_CORNER;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_PLATFORM2CORNER), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Platform corners");
					t.tileType = TILE_TYPE_PLATFORM_DOUBLECORNER;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_PLATFORM2CORNERCROSS), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Platform corners");
					t.tileType = TILE_TYPE_PLATFORM_DOUBLECORNERCROSS;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_PLATFORM3CORNER), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Platform corners");
					t.tileType = TILE_TYPE_PLATFORM_TRIPLECORNER;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_PLATFORM4CORNER), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Platform corners");
					t.tileType = TILE_TYPE_PLATFORM_FOURCORNER;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_WALL), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Walls");
					t.tileType = TILE_TYPE_WALL;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_WALLCORNER), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Walls");
					t.tileType = TILE_TYPE_WALL_CORNER;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_WALLBLOCK), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Walls");
					t.tileType = TILE_TYPE_WALL_BLOCK;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_CATWALK), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Catwalk");
					t.tileType = TILE_TYPE_CATWALK;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_CATWALKEND), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Catwalk");
					t.tileType = TILE_TYPE_CATWALK_END;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_RAMPTYPE), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Ramps");
					t.tileType = TILE_TYPE_RAMP;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_HOLE), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Other");
					t.tileType = TILE_TYPE_HOLE;
				}
				else if(SendMessage(GetDlgItem(hWnd, IDC_TILE_EMPTY), BM_GETCHECK, 0, 0))
				{
					strcpy(t.subcategory, "Other");
					t.tileType = TILE_TYPE_EMPTY;
				}
				else
				{
					strcpy(t.subcategory, "Other");
					t.tileType = TILE_TYPE_OTHER;
				}
			}

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case IDC_TILE_BROWSE_MESH:
			p->GetMesh(hWnd);
			break;
		case IDC_TILE_REMOVE_MESH:
			SetWindowText(GetDlgItem(hWnd, IDC_TILE_MESH), "");
			break;

		case IDC_TILE_BROWSE_TEXTURE:
			p->GetTexture(hWnd);
			break;
		case IDC_TILE_REMOVE_TEXTURE:
			SetWindowText(GetDlgItem(hWnd, IDC_TILE_TEXTURE), "");
			break;
		}
		break;
	}

	return FALSE;
}

void CTileEditWnd::GetMesh(HWND hWnd)
{
	std::string str, full;

	// Open common dialogue to browse for mesh file, don't include directory
	if(CDatabaseUtilities::GetMeshFilename(hWnd, str, CTileManager::GetManager()->GetMeshPath(), false) == FALSE)
		return;

	SetWindowText(GetDlgItem(hWnd, IDC_TILE_MESH), str.c_str());
	char buf[32];

	// Try loading the mesh and automatically setting height level and exact height
	full = CTileManager::GetManager()->GetMeshPath() + str;
	CMesh *mesh = CResourceManager::GetManager().GetMesh(full);
	if(mesh != NULL)
	{
		aabbox3d &box = mesh->GetBoundingBox();
		float height = -box.minCorner.y;
		if(height >= 0.f)
		{
			int level = (int)((height / TILE_LEVEL_HEIGHT) + 0.5f);
			sprintf(buf, "%i", level);
			SetWindowText(GetDlgItem(hWnd, IDC_TILE_HEIGHT_LEVEL), buf);
			sprintf(buf, "%f", height);
			SetWindowText(GetDlgItem(hWnd, IDC_TILE_HEIGHT), buf);
		}
		//CResourceManager::GetManager().ReleaseSpecifiedMeshes(full);
		CResourceManager::GetManager().ReleaseSpecifiedMeshes(CTileManager::GetManager()->GetMeshPath());
	}
}

void CTileEditWnd::GetTexture(HWND hWnd)
{
	std::string str;

	if(CDatabaseUtilities::GetTextureFilename(hWnd, str, CTileManager::GetManager()->GetTexturePath()) == FALSE)
		return;

	SetWindowText(GetDlgItem(hWnd, IDC_TILE_TEXTURE), str.c_str());
}

bool CTileEditWnd::OpenWindow(HWND parent, STileInfo &rTemplate, bool bNew)
{
	if(bNew)
		rTemplate.FillDefaults();

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TILE), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
