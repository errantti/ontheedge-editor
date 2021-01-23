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

#ifndef __CPARTICLEEMITTERWND_H__
#define __CPARTICLEEMITTERWND_H__

#include <windows.h>
#include <vector>
#include <string>

#include "..\\..\\OtE\\ParticleEffects.h"
#include "..\\..\\Tools\\GetVectorWnd.h"
#include "..\\..\\Tools\\Get4DVectorWnd.h"

class CParticleEmitterWnd
{
public:
	CParticleEmitterWnd();
	~CParticleEmitterWnd();

	bool OpenWindow(HWND parent, sEMITTER_PARAMS &rTemplate, bool bNew);

private:
	void SelectColor(HWND hWnd, D3DXCOLOR &color);

	static BOOL CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	static CParticleEmitterWnd *m_pointer;

	void SetResourceType(bool value);

	sEMITTER_PARAMS m_template;
	bool m_bModified, m_bUsingMesh;

	HWND m_hWnd;

	CGetVectorWnd m_getVectorWnd;
	CGet4DVectorWnd m_getColorWnd;
};

#endif // #ifndef __CPARTICLEEMITTERWND_H__
