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

#ifndef __CDATABASEUTILITIES_H__
#define __CDATABASEUTILITIES_H__

#include <string>
#include <d3d9.h>
#include <d3dx9.h>

class CDatabaseUtilities
{
public:
	static bool GetMeshFilename(HWND hWnd, std::string &str, std::string directory = "meshes\\", bool includeDirectory = true)
	{
		OPENFILENAME ofn;
		char filename[128] = "";
		char tempFile[256] = "";

		ZeroMemory(&ofn,sizeof(OPENFILENAME));
		ofn.hwndOwner = hWnd;
		ofn.lpstrFile = tempFile;
		ofn.lpstrFileTitle = filename;
		ofn.lpstrFilter = "X Mesh (*.x)\0*.X\0\0";
		ofn.lpstrDefExt = "x";
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.nMaxFile = 127;
		ofn.nMaxFileTitle = 127;
		ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_SHAREAWARE | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;
		ofn.lpstrInitialDir = directory.c_str();

		if(!GetOpenFileName(&ofn))
			return false;

		str = "";
		if(includeDirectory)
			str = directory;
		str += ofn.lpstrFileTitle;

		return true;
	}

	static bool GetSkyboxTextureFilename(HWND hWnd, std::string &str)
	{
		OPENFILENAME ofn;
		char filename[128] = "";
		char tempFile[256] = "";

		ZeroMemory(&ofn,sizeof(OPENFILENAME));
		ofn.hwndOwner = hWnd;
		ofn.lpstrFile = tempFile;
		ofn.lpstrFileTitle = filename;
		ofn.lpstrFilter = "Texture images (*.jpg, *.bmp, *.tga, *.png)\0*.JPG;*.BMP;*.TGA;*.PNG\0\0";
		ofn.lpstrDefExt = "jpg";
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.nMaxFile = 127;
		ofn.nMaxFileTitle = 127;
		ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_SHAREAWARE | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;
		ofn.lpstrInitialDir = "textures\\skybox";

		if(!GetOpenFileName(&ofn))
			return false;

		str = "textures\\skybox\\";
		str += ofn.lpstrFileTitle;

		return true;
	}

	static bool GetGadgetIconFilename(HWND hWnd, std::string &str)
	{
		OPENFILENAME ofn;
		char filename[128] = "";
		char tempFile[256] = "";

		ZeroMemory(&ofn,sizeof(OPENFILENAME));
		ofn.hwndOwner = hWnd;
		ofn.lpstrFile = tempFile;
		ofn.lpstrFileTitle = filename;
		ofn.lpstrFilter = "Texture images (*.jpg, *.bmp, *.tga, *.png)\0*.JPG;*.BMP;*.TGA;*.PNG\0\0";
		ofn.lpstrDefExt = "jpg";
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.nMaxFile = 127;
		ofn.nMaxFileTitle = 127;
		ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_SHAREAWARE | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;
		ofn.lpstrInitialDir = "imgs\\gadgets";

		if(!GetOpenFileName(&ofn))
			return false;

		str = "imgs\\gadgets\\";
		str += ofn.lpstrFileTitle;

		return true;
	}

	static bool GetTextureFilename(HWND hWnd, std::string &str, std::string directory = "textures\\", bool includeDirectory = true)
	{
		OPENFILENAME ofn;
		char filename[128] = "";
		char tempFile[256] = "";

		ZeroMemory(&ofn,sizeof(OPENFILENAME));
		ofn.hwndOwner = hWnd;
		ofn.lpstrFile = tempFile;
		ofn.lpstrFileTitle = filename;
		//ofn.lpstrFilter = "Texture images (*.jpg, *.bmp, *.tga)\0*.JPG;*.BMP;*.TGA\0\0";
		ofn.lpstrFilter = "Texture images (*.jpg, *.bmp, *.tga, *.png)\0*.JPG;*.BMP;*.TGA;*.PNG\0\0";
		ofn.lpstrDefExt = "jpg";
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.nMaxFile = 127;
		ofn.nMaxFileTitle = 127;
		ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_SHAREAWARE | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;
		ofn.lpstrInitialDir = directory.c_str();

		if(!GetOpenFileName(&ofn))
			return false;

		str = "";
		if(includeDirectory)
			str = directory;
		str += ofn.lpstrFileTitle;

		return true;
	}

	static bool GetScriptFilename(HWND parent, std::string &filename, const std::string directory)
	{
		OPENFILENAME ofn;
		char buffer[256] = "";
		char tempFile[256] = "";

		ZeroMemory(&ofn,sizeof(OPENFILENAME));
		ofn.hwndOwner = parent;
		ofn.lpstrFile = tempFile;
		ofn.lpstrFileTitle = buffer;
		ofn.lpstrFilter = "On the Edge script files (*.ews, *.txt)\0*.EWS;*.TXT;\0\0";
		ofn.lpstrDefExt = "txt";
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.nMaxFile = 255;
		ofn.nMaxFileTitle = 127;
		ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_SHAREAWARE | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;
		ofn.lpstrInitialDir = "textures";
		ofn.lpstrInitialDir = directory.c_str();

		if(!GetOpenFileName(&ofn))
			return false;

		filename = directory;
		filename += ofn.lpstrFileTitle;

		return true;
	}

	static bool GetSoundFilename(HWND parent, std::string &filename, const std::string directory)
	{
		OPENFILENAME ofn;
		char buffer[256] = "";
		char tempFile[256] = "";

		ZeroMemory(&ofn,sizeof(OPENFILENAME));
		ofn.hwndOwner = parent;
		ofn.lpstrFile = tempFile;
		ofn.lpstrFileTitle = buffer;
		ofn.lpstrFilter = "Sound files (*.wav, *.mp3, *.ogg)\0*.WAV;*.MP3;*.OGG;\0\0";
		ofn.lpstrDefExt = "ogg";
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.nMaxFile = 255;
		ofn.nMaxFileTitle = 127;
		ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_SHAREAWARE | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;
		//ofn.lpstrInitialDir = directory.c_str();
		ofn.lpstrInitialDir = directory.c_str();

		if(!GetOpenFileName(&ofn))
			return false;

		filename = directory;
		filename += ofn.lpstrFileTitle;

		return true;
	}

	static bool GetTilesetFilename(HWND hWnd, std::string &str)
	{
		OPENFILENAME ofn;
		char filename[128] = "";
		char tempFile[256] = "";

		ZeroMemory(&ofn,sizeof(OPENFILENAME));
		ofn.hwndOwner = hWnd;
		ofn.lpstrFile = NULLtempFile
		ofn.lpstrFileTitle = filename;
		ofn.lpstrFilter = "On the Edge tileset (*.oet)\0*.OET\0\0";
		ofn.lpstrDefExt = "oet";
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.nMaxFile = 127;
		ofn.nMaxFileTitle = 127;
		ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_SHAREAWARE | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;
		ofn.lpstrInitialDir = "data";

		if(GetOpenFileName(&ofn) == FALSE)
			return false;

		str = "data\\";
		str += ofn.lpstrFileTitle;

		return true;
	}

	static bool SelectD3DCOLOR(HWND hWnd, D3DCOLOR &color)
	{
		CHOOSECOLOR cc;

		static COLORREF CColorList[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

		ZeroMemory(&cc, sizeof(CHOOSECOLOR));
		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = hWnd;
		cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;/**/
		cc.lpCustColors = CColorList;
		cc.rgbResult = RGB((color>>16) % 0xffff00ff, (color>>8) % 0xffff00ff, color % 0xffffff00);

		if(ChooseColor(&cc) == FALSE)
			return false;

		color = D3DCOLOR_XRGB(GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));

		return true;
	}

	static bool GetFullExportPath(HWND hWnd, std::string &outStr, std::string fileExt);
	static bool GetFullImportPath(HWND hWnd, std::string &outStr, std::string fileExt);

	static bool SelectParticleEffect(HWND hWnd, long &selection);
	static bool SelectLightEffect(HWND hWnd, long &selection);
	static bool SelectGadget(HWND hWnd, long &selection);
	static bool SelectWaypointSet(HWND hWnd, int &ID);
	static bool SelectDialogue(HWND hWnd, int &ID);
	static bool SelectAssistantMsg(HWND hWnd, int &ID);
	static bool SelectTerminal(HWND hWnd, long &ID);
	static bool SelectShip(HWND hWnd, long &ID);
	static bool SelectAmmo(HWND hWnd, long &ID);
	static bool SelectLocalScript(HWND hWnd, std::string &script);
	static bool SelectGlobalScript(HWND hWnd, std::string &script);
	static bool SelectScript(HWND hWnd, std::string &script, bool bIncludeGlobals = true, bool bIncludeTemporary = true);
	static bool SelectSound(HWND hWnd, long &ID);
	static bool SelectComponent(HWND hWnd, long &ID);
	static bool SelectQuestItem(HWND hWnd, long &ID);
	static bool SelectShipPatch(HWND hWnd, long &ID);
	static bool SelectController(HWND hWnd, long &ID);

	/*
	static std::string GetParticleEffectName(long id);
	static std::string GetGadgetName(long id);
	static std::string GetDialogueName(long id);
	static std::string GetTerminalName(long id);
	static std::string GetShipName(long id);
	static std::string GetAmmoName(long id);
	static std::string GetSoundName(long id);
	static std::string GetComponentName(long id);
	static std::string GetQuestItemName(long id);
	static std::string GetShipName(long id);*/

	/*static bool SelectD3DCOLOR(HWND hWnd, D3DXCOLOR &color)
	{
		CHOOSECOLOR cc;

		static COLORREF CColorList[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

		ZeroMemory(&cc, sizeof(CHOOSECOLOR));
		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = hWnd;
		cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
		cc.lpCustColors = CColorList;
		cc.rgbResult = RGB(color., (color>>8) % 0xffff00ff, color % 0xffffff00);

		if(ChooseColor(&cc) == FALSE)
			return false;

		color = D3DCOLOR_XRGB(GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));

		return true;
	}/**/
};


#endif // #ifndef __CDATABASEUTILITIES_H__