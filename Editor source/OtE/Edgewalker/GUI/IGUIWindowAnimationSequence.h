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

#ifndef __IGUIWINDOWANIMATIONSEQUENCE_H__
#define __IGUIWINDOWANIMATIONSEQUENCE_H__

class CGUIWindow;

/**
 * Interface for classes that animate a CGUIWindow object
 */
class IGUIWindowAnimationSequence
{
public:
	IGUIWindowAnimationSequence(CGUIWindow *window, bool bEnableControlsAfterAnimation)
		: m_window(window), m_bEnableControlsAfterAnimation(bEnableControlsAfterAnimation)
	{}

	virtual bool OverridesRender() = 0;
	virtual bool EnableControlsAfterAnimation() {return m_bEnableControlsAfterAnimation;}

	virtual void Render() = 0;
	virtual void Update(float frameTime) = 0;

	bool IsFadingIn() {return m_bFadingIn;}
	float GetAnimationDuration() {return m_duration;}

	void SetIsFadingIn(bool value) {m_bFadingIn = value;}
	void SetAnimationDuration(float value) {m_duration = value;}

	CGUIWindow *GetWindow() {return m_window;}

protected:
	bool m_bFadingIn;
	bool m_bEnableControlsAfterAnimation;
	float m_duration;

	CGUIWindow *m_window;
};

#endif // __IGUIWINDOWANIMATIONSEQUENCE_H__