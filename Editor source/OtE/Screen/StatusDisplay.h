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

#ifndef __CSTATUSDISPLAY_H__
#define __CSTATUSDISPLAY_H__

#define FPS_UPDATE_FREQUENCY	1.f

class CStatusDisplay  
{
public:
	CStatusDisplay();
	~CStatusDisplay();

	void Render();
	void Update(float frameTime);

	bool LoadDefaultGraphics();

private:
	CTexture m_lifeBar, m_energyBar, m_gadgetLoadBar;
	CTexture m_background;

	float m_lifePercentage, m_energyPercentage;
	float m_gadgetPercentage[2];
	DWORD m_gadgetModColor[2];

	float m_pl2Life, m_pl2Energy;
	float m_pl2gadget[2];

	bool m_bShowFPS, m_bYDown;
	float m_timeFromFPSUpdate;
};

#endif // #ifndef __CSTATUSDISPLAY_H__
