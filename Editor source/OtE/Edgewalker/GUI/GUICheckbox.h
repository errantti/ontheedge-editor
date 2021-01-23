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

#ifndef __GUICHECKBOX_H__
#define __GUICHECKBOX_H__


#include "IGUIElement.h"

class CGUICheckbox : public IGUIElement  
{
public:
	CGUICheckbox(IGUIEnvironment *env, IGUIElement *parent, rect<int> position, long id);
	virtual ~CGUICheckbox();

	virtual bool OnEvent(SEvent event);

	virtual void SetImage(CTexture *image);
	virtual void SetImageHover(CTexture *image);
	virtual void SetImagePressed(CTexture *image);
	virtual void SetImagePressedHover(CTexture *image);

	virtual CTexture *GetImage() {return m_image;}
	virtual CTexture *GetImageHover() {return m_imageHover;}
	virtual CTexture *GetImagePressed() {return m_imagePressed;}
	virtual CTexture *GetImagePressedHover() {return m_imagePressedHover;}

protected:
	CTexture *m_image, *m_imageHover, *m_imagePressed, *m_imagePressedHover;

	virtual void RenderElement();
};

#endif // ifndef __GUICHECKBOX_H__