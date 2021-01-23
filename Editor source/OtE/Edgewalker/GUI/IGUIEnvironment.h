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

#ifndef __IGUIENVIRONMENT_H__
#define __IGUIENVIRONMENT_H__

#include "IGUIElement.h"
#include <string>

class IGUIEnvironment
{
public:
	virtual void DrawAll() = 0;
	virtual void Update() = 0;

	virtual IGUIElement *AddWindow(IGUIElement *parent, rect<int> position, long id, IEventReceiver *handler) = 0;
	virtual IGUIElement *AddButton(IGUIElement *parent, rect<int> position, long id, string text = "") = 0;
	virtual IGUIElement *AddLabel(IGUIElement *parent, rect<int> position, long id, string text = "") = 0;
	virtual IGUIElement *AddImage(IGUIElement *parent, rect<int> position, long id, string image = "") = 0;
	virtual IGUIElement *AddScrollbar(IGUIElement *parent, rect<int> position, long id) = 0;
};

#endif __IGUIENVIRONMENT_H__ // #ifndef __IGUIENVIRONMENT_H__