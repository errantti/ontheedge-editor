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

#include "VerticalScaleAnimator.h"
#include "..\\..\\Edgewalker\\Edgewalker.h"

CVerticalScaleAnimator::CVerticalScaleAnimator(CGUIWindow *window, bool bFadeIn, float duration,
											   bool bEnableControlsAfterAnimation) :
IGUIWindowAnimationSequence(window, bEnableControlsAfterAnimation)
{
	m_bFadingIn = bFadeIn;
	m_duration = duration;
	if(bFadeIn)
		m_timeFromStart = 0.f;
	else
		m_timeFromStart = duration;

	if(m_window != NULL)
		m_background = m_window->GetBackground();
}

void CVerticalScaleAnimator::Update(float frameTime)
{
	bool bClose = false;
	if(m_bFadingIn)
	{
		m_timeFromStart += frameTime;
		if(m_timeFromStart >= m_duration)
			bClose = true;
	}
	else
	{
		m_timeFromStart -= frameTime;
		if(m_timeFromStart <= 0.f)
			bClose = true;
	}

	if(bClose)
	{
		// Animation sequence complete
		if(m_window != NULL)
		{
			m_window->ReportAnimationFinished();
			return;
		}
	}
}

void CVerticalScaleAnimator::Render()
{
	LPD3DXSPRITE sprite = GetDXApp()->GetSpriteInterface();
	if(sprite == NULL || m_background == NULL)
		return;

	D3DXMATRIX mat, id;
	float temp = m_timeFromStart / m_duration;
	D3DXMatrixIdentity(&mat);
	D3DXMatrixIdentity(&id);
	rect<int> r = m_window->GetAbsoluteRect();
	D3DXVECTOR3 center((float)r.getWidth() * 0.5f, (float)r.getHeight() * 0.5f, 0.f);
	D3DCOLOR modColor = m_window->GetModColor();
	D3DXVECTOR3 position(center.x + (float)r.left, center.y + (float)r.top, 0.f);

	mat._22 = temp;
	mat._42 = position.y - temp * position.y;
	sprite->SetTransform(&mat);
	//sprite->Draw(m_background->GetTexture(), NULL, &center, &position, modColor);
	sprite->Draw(m_background->GetTexture(), NULL, &center, &position, 0xFFFFFFFF);
	sprite->SetTransform(&id);
}