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

#ifndef __CLINEARINTERPOLATEANIMATOR_H__
#define __CLINEARINTERPOLATEANIMATOR_H__

#include "..\\..\\Edgewalker\\GUI\\GUIWindow.h"

/**
 * Animates a window by moving it from given source position
 * to a target position using linear interpolation (or another
 * interpolation method). Doesn't override rendering
 */
class CLinearInterpolateAnimator : public IGUIWindowAnimationSequence
{
public:
	CLinearInterpolateAnimator(CGUIWindow *window, float duration, POINT source, POINT target,
		bool bEnableControlsAfterAnimation);

	virtual bool OverridesRender() {return false;}

	virtual void Render() {}
	virtual void Update(float frameTime);

protected:
	//POINT m_sourcePos, m_targetPos;
	float m_deltaX, m_deltaY;
	float m_x, m_y;
	POINT m_target;

	float m_timeFromStart;
};

#endif // __CLINEARINTERPOLATEANIMATOR_H__