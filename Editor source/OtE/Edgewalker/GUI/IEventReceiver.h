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

#ifndef __IEVENRECEIVER_H__
#define __IEVENRECEIVER_H__

enum EW_EVENT_TYPE {
	EW_GUI_EVENT
};

enum EW_GUI_EVENT_TYPE {
	EWGUI_ELEMENT_HOVERED=0,
	EWGUI_ELEMENT_HOVERLOST,
	EWGUI_ELEMENT_FOCUS,
	EWGUI_ELEMENT_FOCUSLOST,
	EWGUI_ELEMENT_PRESSED,
	EWGUI_ELEMENT_CREATED,
	EWGUI_ELEMENT_DESTROYED,
	EWGUI_WINDOW_ANIMATION_SEQUENCE_FINISHED
};

/**
 * Structure that holds information about a single event message
 **/
struct SEvent
{
	EW_EVENT_TYPE type;
	union
	{
		struct
		{
			EW_GUI_EVENT_TYPE type;
			int x,y;
			void *element;
		} guiEvent;
	};
};

/**
 * Interface of an object which can receive messages
 **/
class IEventReceiver
{
public:
	IEventReceiver() : m_bRequiresUpdate(FALSE) {}
	virtual bool OnEvent(SEvent event) = 0;
	virtual void Update() {}
	inline bool GetRequiresConstantUpdate() {return m_bRequiresUpdate;}
	inline void SetRequiresConstantUpdate(bool value) {m_bRequiresUpdate = value;}
protected:
	bool m_bRequiresUpdate;
};

#endif //#ifndef __IEVENRECEIVER_H__