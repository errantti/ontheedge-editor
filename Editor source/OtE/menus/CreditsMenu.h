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

#ifndef __CREDITSMENU_H__
#define __CREDITSMENU_H__

#include "FlowMenu.h"

enum CREDITS_MENU_ELEMENTS{
	CRE_CLOSE=100,
	CRE_COUNT
};

#define NUM_ROLLING_TEXT_CONTROLS	8

class CCreditsMenu : public CFlowMenu  
{
public:
	void OnClose();
	bool OnEvent(SEvent event);

	CCreditsMenu(IMenuManager *mgr);
	virtual ~CCreditsMenu();

	virtual void CustomInit();

	void Update();

private:
	struct SText
	{
		CGUILabel *control;
		bool enabled;
		float time;

		SText() : control(NULL), enabled(false) {}
		SText(CGUILabel *control, bool enabled) :
			control(control), enabled(enabled) {}
	};

	bool LoadCreditsFromFile(const std::string filename);
	void ResetTextControls();
	void UpdateRollingText(float frameTime);
	int GetNextFreeText();

	std::vector<std::string> m_creditsText;

	float m_timeFromLastText;
	int m_nextTextIndex;

	SText m_textControls[NUM_ROLLING_TEXT_CONTROLS];
};

#endif // ifndef __CREDITS_H__
