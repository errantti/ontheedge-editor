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

#ifndef __IApplication_H_
#define __IApplication_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Printer.h"
#include "Timer.h"

#define ODS(s) ::OutputDebugString(_T(s))

class IApplication  
{
public:
	static int RandInt(const int iLow, const int iHigh);
	static float RandFloat(const float fLow, const float fHigh);

	// OnCreate pure virtual function is called when
	// application has finished initializing 
	// User can return an error value if application initialization
	// failed. Application is terminated
	virtual HRESULT OnCreate() = 0;

	// OnRelease pure virtual function is called when
	// application is just about to release the internal resources.
	// This handler is usually used by the derived class to release all
	// app spesific resources.
	virtual void OnRelease() = 0;

	// OnFlip pure virtual function
	// derived class main loop function
	virtual void OnFlip() = 0;

	IApplication();
	virtual ~IApplication();

	void Release();
	void Close();
	HRESULT Create(int iWidth, int iHeight, int iBPP, BOOL bWindowed, LPCTSTR strTitle, DWORD dwFlags, bool bMaximizeBox = false);

	virtual void OnSize(DWORD dwType, int iWidth, int iHeight);
	void SetActive(BOOL bSet);
	static IApplication * GetApp();
	int Run();

	inline BOOL IsWindowed() const {return m_bWindowed;}
	inline BOOL IsActive() const {return m_bActive;}
	inline HWND GetWindow() {return m_hWnd;}

	// Returns the application timer class instance
	inline CTimer *GetTimer() {return &m_Timer;}

	// Returns seconds passed doing last update loop
	inline float GetFrameTime() const {return m_Timer.GetFrameTimeF();}

	virtual void DoFlip();
	virtual void OnMove(int iPosX, int iPosY);
	virtual void OnKeyDown(DWORD dwKey);
	virtual void OnMouseMove(int xpos, int ypos);

	inline RECT *GetViewportRect() {return &m_rcViewport;}
	inline RECT *GetScreenRect() {return &m_rcScreen;}

	void SetWindowCaption(char *str);

	void SetCursorVisibility(bool bValue);

protected:
	void UpdateRectangles();
	virtual BOOL OnMessage(UINT iMessage, WPARAM wParam, LPARAM lParam);
	BOOL CreateAppWindow(int iWidth, int iHeight, BOOL bWindowed, LPCTSTR strTitle, bool bMaximizeBox);

	HWND m_hWnd;
	BOOL m_bWindowed, m_bActive;

	WORD m_menuResource;

	//Screen and viewport rectangles
	RECT m_rcScreen, m_rcViewport;

	// Application main timer
	CTimer m_Timer;

	static long WINAPI WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
	static IApplication *m_pApp;
};

#endif // #ifndef __IApplication_H_
