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

#ifndef __CGUIENVIRONMENT_H__
#define __CGUIENVIRONMENT_H__

#include "IGUIElement.h"
#include "..\\DirectX9\\DirectX9.h"

#include <list>
#include <vector>
#include <iterator>
#include "..\\DirectX9\\Font.h"

#include "IGUIEnvironment.h"
#include "GUIButton.h"
#include "GUILabel.h"
#include "GUIImage.h"
#include "GUIScrollbar.h"
#include "GUIWindow.h"

class CGUIEnvironment: public IGUIEnvironment, public IGUIElement
{
public:
	CGUIEnvironment();
	virtual ~CGUIEnvironment();

	virtual IGUIElement *AddWindow(IGUIElement *parent, rect<int> position, long id, IEventReceiver *handler);
	virtual IGUIElement *AddButton(IGUIElement *parent, rect<int> position, long id, string text = "");
	virtual IGUIElement *AddLabel(IGUIElement *parent, rect<int> position, long id, string text = "");
	virtual IGUIElement *AddImage(IGUIElement *parent, rect<int> position, long id, string image = "");
	virtual IGUIElement *AddScrollbar(IGUIElement *parent, rect<int> position, long id);

	//virtual IGUIElement *GetElement(long id);
	//virtual IGUIElement *GetElement(POINT pos);
	//virtual IGUIElement *GetElementOnTop(POINT pos);
	//virtual IGUIElement *GetTopmostActiveElement(POINT pos);
	//virtual void Set
	//virtual void AddImage(CTexture *image);
	//virtual CTexture *AddImage(string filename, D3DCOLOR colorkey=0);
	//virtual void RemoveImages();

	//virtual CTexture *LoadImage(const char *filename, D3DCOLOR colorkey = 0);

	//virtual void SetEventReceiver(IEventReceiver *rcv) {m_receiver = rcv;}
	//virtual IEventReceiver *GetEventReceiver() {return m_receiver;}

	virtual IGUIElement *GetHoveringElement() {return m_hover;}
	virtual void SetHoveringElement(IGUIElement *element);
	// This sets the hovering element to the /param element without
	// messaging anyone. Usually called only when changing menu
	virtual void ForceHoverElement(IGUIElement *element);

	IGUIElement *GetFocusElement() {return m_focus;}
	void SetFocusElement(IGUIElement *element);
	void ForceFocusElement(IGUIElement *element);

	//virtual int LoadFont(char *filename, int size, bool bBold = FALSE);
	//virtual CFont *GetFont(int index) {return m_fonts[index];}

	bool OnEvent(SEvent event) {return false;}

	bool LoadDefaultFont();
	CFont *GetDefaultFont();

	virtual void Update();
	virtual void DrawAll();

	virtual bool GetCursorVisible() {return m_bCursorVisible;}
	virtual void SetCursorVisibility(bool value) {m_bCursorVisible = value;}
	virtual CTexture *GetCursorImage() {return m_cursorImage;}
	virtual void SetCursorImage(CTexture *image);
	virtual void SetCursorImage(string filename);
	bool LoadCursorImage(string filename);

	virtual bool RemoveChild(IGUIElement *child);

	static CGUIEnvironment *GetGUIEnvironment() {return m_this;}

protected:
	CTexture *m_cursorImage;
	bool m_bCursorVisible;
	bool m_bCriticalArea;

	CFont *m_defaultFont;

	IGUIElement *m_hover, *m_focus;

	vector<IGUIElement*> m_waitingToBeRemoved;

	virtual void RenderElement() {}
	void RemovePending();

	static CGUIEnvironment *m_this;
};

#endif // #ifndef __CGUIENVIRONMENT_H__
