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

#include "..\\OnTheEdge.h"
#include "AssistantMessages.h"
#include "..\\menus\\AssistantView.h"

using namespace std;

CAssistantMessages *CAssistantMessages::m_instance;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAssistantMessages::CAssistantMessages() : m_view(NULL), m_timeFromMessage(0.f), m_autoCloseTime(10.f),
	m_bClosingView(false), m_bMessageWaiting(false)
{
	if(m_instance == NULL)
		m_instance = this;
}

CAssistantMessages::~CAssistantMessages()
{
	if(m_instance == this)
		m_instance = NULL;
	CleanQueue();
}

void CAssistantMessages::CleanQueue()
{
	while(!m_messageQueue.empty())
	{
		delete m_messageQueue.front();
		m_messageQueue.pop();
	}
}

void CAssistantMessages::Update(float frameTime)
{
	// If queue is empty then the assistant is not currently open and there's nothing to update
	if(m_messageQueue.empty())
		return;

	if(GetOTE()->IsPaused())
	{
		if(m_view)
			m_view->Hide();
		return;
	}
	else
	{
		if(m_view)
			m_view->Reveal();

		if(m_bMessageWaiting)
		{
			m_bMessageWaiting = false;
			OpenView();
		}
	}
	m_timeFromMessage += frameTime;
	if(m_timeFromMessage > m_autoCloseTime)
		Close();
}

void CAssistantMessages::AddMessage(std::string *msg)
{
	if(msg == NULL)
		return;
	if(m_messageQueue.empty() && !m_bClosingView)
	{
		m_messageQueue.push(msg);
		OpenView();
	}
	else
		m_messageQueue.push(msg);
}

void CAssistantMessages::Reset()
{
	if(m_messageQueue.empty())
		return;
	CleanQueue();
	if(m_view)
		m_view->SendCloseMessage();
	m_view = NULL;
	m_bClosingView = false;
	m_bMessageWaiting = false;
}

void CAssistantMessages::OpenView()
{
	m_timeFromMessage = 0.f;
	GetOTE()->GetMenuManager()->OpenIndependentMenu(MENU_ID_ASSISTANT_VIEW);
	if(!m_messageQueue.empty())
	{
		int size = (int)m_messageQueue.front()->size();
		if(size < 100)
			m_autoCloseTime = 6.f;
		else if(size > 350)
			m_autoCloseTime = 18.f;
		else
		{
			// Scale the autoclose time so that 100 chars = 6 s, 350 
			// chars = 18 s and between those it scales in a linear fashion
			m_autoCloseTime = ((float)size-100.f)/250.f*12.f+6.f;
		}
	}
}

void CAssistantMessages::ReportViewClosed()
{
	m_bClosingView = false;
	m_view = NULL;
	if(!m_messageQueue.empty())
	{
		if(GetOTE()->IsPaused())
		{
			m_bMessageWaiting = true;
			return;
		}
		OpenView();
	}
}

void CAssistantMessages::Close()
{
	// Do this to avoid consecutive Close calls
	m_timeFromMessage = -10000.f;
	m_bClosingView = true;

	if(!m_messageQueue.empty())
	{
		delete m_messageQueue.front();
		m_messageQueue.pop();
	}
	if(m_view)
		m_view->CloseView();
}

void CAssistantMessages::SetAssistantViewObject(CAssistantView *view)
{
	m_view = view;
}

std::string *CAssistantMessages::GetCurrentMessage()
{
	if(m_messageQueue.empty())
		return NULL;
	return m_messageQueue.front();
}

void CAssistantMessages::SetAutoCloseTime(float time)
{
	m_autoCloseTime = time;
}

float CAssistantMessages::GetAutoCloseTime()
{
	return m_autoCloseTime;
}