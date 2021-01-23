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

#include "LinearInterpolateAnimator.h"

CLinearInterpolateAnimator::CLinearInterpolateAnimator(CGUIWindow *window, float duration, POINT source, POINT target,
													   bool bEnableControlsAfterAnimation)
: IGUIWindowAnimationSequence(window, bEnableControlsAfterAnimation), m_timeFromStart(0.f)
{
	m_duration = duration;
	if(m_duration < 0.1f)
		m_duration = 0.1f;

	m_x = (float)source.x;
	m_y = (float)source.y;

	m_target = target;

	m_deltaX = (float)(target.x - source.x) / m_duration;
	m_deltaY = (float)(target.y - source.y) / m_duration;
}

void CLinearInterpolateAnimator::Update(float frameTime)
{
	m_timeFromStart += frameTime;

	if(m_window == NULL)
		return;

	rect<int> r = m_window->GetElementRect();
	int width = r.getWidth(), height = r.getHeight();

	if(m_timeFromStart >= m_duration)
	{
		if(m_window != NULL)
		{
			r.left = m_target.x;
			r.top = m_target.y;
			r.right = r.left + width;
			r.bottom = r.top + height;
			m_window->SetElementRect(r);

			m_window->ReportAnimationFinished();
		}
		return;
	}

	m_x += m_deltaX * frameTime;
	m_y += m_deltaY * frameTime;

	r.left = (int)m_x;
	r.top = (int)m_y;
	r.right = r.left + width;
	r.bottom = r.top + height;

	m_window->SetElementRect(r);
	//m_window->UpdateAbsoluteRect();
}