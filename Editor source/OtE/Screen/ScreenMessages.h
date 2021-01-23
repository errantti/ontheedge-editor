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

#ifndef __CSCREENMESSAGES_H__
#define __CSCREENMESSAGES_H__

#include "..\\Edgewalker\\Edgewalker.h"

class CScreenMessages
{
public:
	struct SScreenMessage
	{
		// Info variables
		std::string message;
		float time, fadeTime;
		D3DCOLOR textColor;
		rect<int> area;
		DWORD flags;
		int ID, nextMessageID;	// It's possible to queue messages using nextMessage 
		bool bWaitingInQueue;	// and bWaitingInQueue setting
		bool bFades, bDelete;
		bool bAutoPushback;

		// Set the properties in constructor that would most easily be left unset (=random)
		SScreenMessage()
		{
			bWaitingInQueue = FALSE;
			ID = -1;
			bDelete = TRUE;
			nextMessageID = -1;
		}

		void FillDefaults()
		{
			message = "";
			time = 3.f;
			area = rect<int>(200, 600, 200, 400);
			nextMessageID = -1;
			bWaitingInQueue = FALSE;
			fadeTime = 1.f;
			textColor = D3DCOLOR_RGBA(200, 200, 200, 255);
			ID = -1;
			bFades = TRUE;
			bDelete = TRUE;
			flags = DT_CENTER | DT_VCENTER;
			bAutoPushback = FALSE;
		}

		void FillBasicMessage()
		{
			message = "";
			time = 4.f;
			fadeTime = 1.f;
			textColor = D3DCOLOR_RGBA(255, 255, 255, 255);
			area = rect<int>(200, 600, 290, 310);
			nextMessageID = -1;
			ID = -1;
			bWaitingInQueue = FALSE;
			bFades = TRUE;
			bDelete = TRUE;
			flags = DT_CENTER | DT_VCENTER;
			bAutoPushback = TRUE;
		}
	};

	CScreenMessages();
	~CScreenMessages();

	void Update(float frameTime);
	void Render();

	void AddMessage(std::string msg, float time = 4.f);
	void AddMessage(std::string msg, float time, D3DCOLOR color);

	void AddDetailedMessage(SScreenMessage s);
	bool UpdateDetailedMessage(int ID, std::string newMessage);
	bool RemoveDetailedMessage(int ID);

	void SetDefaultTextColor(D3DCOLOR color) {m_defaultTextColor = color;}

	inline void Empty() {m_detailedMessages.clear();}

private:
	void BumpBasicMessages();

	vector<SScreenMessage> m_detailedMessages;
	D3DCOLOR m_defaultTextColor;
	int m_halfMessageHeight;
};

#endif // #ifndef __CSCREENMESSAGES_H__
