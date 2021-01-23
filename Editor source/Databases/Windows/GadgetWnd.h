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

#ifndef __CGadgetWnd_H__
#define __CGadgetWnd_H__

#include <windows.h>
#include <vector>
#include <string>

#include "..\\..\\OtE\\Gadgets\\GadgetDatabase.h"
#include "..\\..\\OtE\\Gadgets\\WeaponGadget.h"

/**
 * CGadgetWnd handles a window that let's edit a single
 * gadget's information
 */
class CGadgetWnd
{
public:
	CGadgetWnd();
	~CGadgetWnd();

	bool OpenWindow(HWND parent, std::pair<SGadgetTemplate, SCustomGadgetTemplate*> &rTemplate, bool bNew);

private:
	static BOOL CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	static CGadgetWnd *m_pointer;

	void DisableTypeSelection(HWND hWnd);

	void SelectWeaponType(WEAPON_MODE mode, HWND hWnd, bool bFirst);

	/** 
	 * Filling the template structure from form
	 **/
	void FillTemplateFromForm(HWND hWnd);
	void FillGeneralTemplateInfo(HWND hWnd);
	void FillWeaponTemplateInfo(HWND hWnd);
	void FillKineticGunTemplateInfo(HWND hWnd);
	void FillShieldTemplateInfo(HWND hWnd);

	/**
	 * Filling form information
	 **/
	void FillFormInfo(HWND hWnd);
	void FillWeaponFormInfo(HWND hWnd);
	void FillKineticGunFormInfo(HWND hWnd);
	void FillShieldFormInfo(HWND hWnd);
	void FillGeneralFormInfo(HWND hWnd);

	std::pair<SGadgetTemplate, SCustomGadgetTemplate*> m_template;
	bool m_bModified, m_bNew, m_bTypeSelected;

	bool m_bAmmoWeapon;
};

#endif // #ifndef __CGadgetWnd_H__
