// On the Edge Editor
// 
// Copyright � 2004-2021 Jukka Tykkyl�inen
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

#ifndef __CGUIWINDOW_H__
#define __CGUIWINDOW_H__

#include "IGUIElement.h"
#include "IGUIWindowAnimationSequence.h"
#include "IExternalGUIWindowRendering.h"
#include "..\\DirectX9\\Texture.h"

class CGUIWindow : public IGUIElement
{
public:
	CGUIWindow(IGUIEnvironment *env, IGUIElement *parent, rect<int> position, long id, IEventReceiver *handler);
	virtual ~CGUIWindow();

	virtual bool OnEvent(SEvent event);
	virtual void Update();

	void SetBackgroundImage(CTexture *tex);
	void SetBackgroundImage(string filename);

	CTexture *GetBackground() {return m_background;}

	void TabSelect(bool forward);
	virtual void Draw();

	virtual void AttachAnimationSequence(IGUIWindowAnimationSequence *anim);
	virtual void ReportAnimationFinished();

	void SetChildrenEnabled(bool value);

	void SetExternalRendering(IExternalGUIWindowRendering *render) {m_externalRendering = render;}

protected:
	virtual void RenderElement();

	CTexture *m_background;
	bool m_bEnableControlsAfterAnimation;

	IGUIWindowAnimationSequence *m_animation;
	IExternalGUIWindowRendering *m_externalRendering;
};

#endif // #ifndef __CGUIWINDOW_H__