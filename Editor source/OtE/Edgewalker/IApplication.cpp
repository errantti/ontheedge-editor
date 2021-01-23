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

#include "IApplication.h"
#include "..\\resource.h"

//declare the static member pointer
IApplication *IApplication::m_pApp = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IApplication::IApplication() : m_menuResource(0)
{
	m_bWindowed = TRUE;
	m_bActive = TRUE;
	m_hWnd = NULL;

	::memset(&m_rcScreen, 0, sizeof(RECT));
	::memset(&m_rcViewport, 0, sizeof(RECT));

	// seed the random number generator
	::srand(GetTickCount());

	m_pApp = this;

	CPrinter::OpenLog("log.txt");
}

IApplication::~IApplication()
{
	m_pApp = NULL;
}

/**
 * Create creates the application with the given parameters
 * and returns S_OK if succesfull or any system wide error code if failed.
 **/
HRESULT IApplication::Create(int iWidth, int iHeight, int iBPP, BOOL bWindowed, LPCTSTR strTitle, DWORD dwFlags, bool bMaximizeBox)
{
	if(CreateAppWindow(iWidth, iHeight, bWindowed, strTitle, bMaximizeBox) == FALSE)
	{
		return E_FAIL;
	}

	// Initialize the app timer
	m_Timer.Start();

	// call pure virtual OnCreate handler
	HRESULT hres = OnCreate();
	if(FAILED(hres))
	{
		CPrinter::log("OnCreate() failed", LOG_ERROR);
		Release();
		return hres;
	}

	SetActive(TRUE);
	return S_OK;
}

/**
 * Closes down the application and frees any allocated resources
 **/
void IApplication::Release()
{
	// Call pure virtual OnRelease
	OnRelease();

	//destroy the window
	if(m_hWnd != NULL)
	{
		;;DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

/**
 * Creates the application window
 **/
BOOL IApplication::CreateAppWindow(int iWidth, int iHeight, BOOL bWindowed, LPCTSTR strTitle, bool bMaximizeBox)
{
	m_bWindowed = bWindowed;

	//create the window
	HINSTANCE hInstance = ::GetModuleHandle(NULL);
	DWORD dwStyle = 0;
	HMENU hMenu = NULL;
	int iWndWidth = 0, iWndHeight = 0;
		
	if(IsWindowed())
	{
		//create bordered window
		//dwStyle = WS_OVERLAPPEDWINDOW;
		dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
		if(bMaximizeBox)
			dwStyle |= WS_MAXIMIZEBOX;

		iWndWidth = iWidth + GetSystemMetrics(SM_CXSIZEFRAME) * 2;
		iWndHeight = iHeight + GetSystemMetrics(SM_CYSIZEFRAME) * 2 + GetSystemMetrics(SM_CYMENU);
	}
	else
	{
		//Create borderless window
		dwStyle = WS_POPUP;
		iWndWidth = iWidth;
		iWndHeight = iHeight;
	}

	ShowCursor(FALSE);

	//register the window class
	WNDCLASSEX wc;

	memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = ::LoadCursor(hInstance, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(m_menuResource);
	wc.lpszClassName = _T("IGameAppWC");

	if(::RegisterClassEx(&wc) == FALSE)
		return FALSE;

	m_hWnd = ::CreateWindowEx(0, _T("IGameAppWC"), strTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, 
		iWndWidth, iWndHeight, NULL, hMenu, hInstance, NULL);

	if(m_hWnd == NULL)
	{
		CPrinter::log("IApplication - CreateWindowEx failed", LOG_ERROR);
		return FALSE;
	}

	//Window is ready, update & show it
	::UpdateWindow(m_hWnd);
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateRectangles();

	return TRUE;
}

/**
 * Closes the application window
 **/
void IApplication::Close()
{
	if(m_hWnd)
	{
		::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
	}
	else
	{
		CPrinter::log("NULL window handle - fatal error", LOG_ERROR);
		exit(-2);
	}
}

/**
 * Window procedure
 **/
long WINAPI IApplication::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	IApplication *pApp = IApplication::GetApp();
	if(pApp == NULL)
	{
		return ::DefWindowProc(hWnd, iMessage, wParam, lParam);
	}
	if(pApp->OnMessage(iMessage, wParam, lParam) == FALSE)
	{
		return ::DefWindowProc(hWnd, iMessage, wParam, lParam);
	}
	else
		return 0;
}

/**
 * The central application processing function
 **/
int IApplication::Run()
{
	MSG msg;
	BOOL bGotMsg = FALSE;

	::memset(&msg, 0, sizeof(MSG));
	
	//Loop until the WM_QUIT message is received from the window
	while(msg.message != WM_QUIT)
	{
		if(IsActive())
			bGotMsg = ::PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE);
		else
			bGotMsg = ::GetMessage(&msg, m_hWnd, 0, 0);

		if(bGotMsg)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			DoFlip();
		}
	}

	return msg.wParam;
}

/**
 * virtual window message handler
 * returns TRUE if message is handled by the function
 * and no further processing is desired. False if DefWindowProc is required
 **/
BOOL IApplication::OnMessage(UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_CREATE:
		::SetForegroundWindow(m_hWnd);
		break;

	case WM_CLOSE:
		SetActive(FALSE);
		Release();
		break;

	case WM_ACTIVATEAPP:
		if(wParam == 0)
		{
			//application is de-activating
			//if(IsWindowed() == FALSE)
				SetActive(FALSE);
		}
		else
			SetActive(TRUE);
		break;

	case WM_SIZE:
		OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_MOVE:
		OnMove(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_GETMINMAXINFO:
		if(IsActive() && IsWindowed())
		{
			//fix window minimum size
			MINMAXINFO *pInfo = (MINMAXINFO*)lParam;
			pInfo->ptMinTrackSize.x = 64;
			pInfo->ptMinTrackSize.y = 64;
		}
		break;

	case WM_SETCURSOR:
		if(IsActive() && !IsWindowed())
		{
			::SetCursor(NULL);
			return TRUE;
		}
		break;

	case WM_KEYDOWN:
		OnKeyDown(wParam);
		break;

	case WM_MOUSEMOVE:
		OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		return true;

	case WM_EXITSIZEMOVE:
		m_Timer.Continue();
		break;

	default:
		break;
	}

	return FALSE;
}

IApplication * IApplication::GetApp()
{
	return m_pApp;
}

/**
 * Activate/inactivate application
 **/
void IApplication::SetActive(BOOL bSet)
{
	m_bActive = bSet;
	m_Timer.Continue();
}

void IApplication::OnSize(DWORD dwType, int iWidth, int iHeight)
{
	if(dwType == SIZE_MINIMIZED)
	{
		SetActive(FALSE);
	}
	else
	{
		if(IsWindowed() == TRUE)
		{
			UpdateRectangles();
		}
		SetActive(TRUE);
	}
}

void IApplication::OnKeyDown(DWORD dwKey)
{
}

void IApplication::OnMouseMove(int xpos, int ypos)
{
}

void IApplication::SetWindowCaption(char *str)
{
	SetWindowText(m_hWnd, str);
}

/**
 * Virtual handler for the window movement
 **/
void IApplication::OnMove(int iPosX, int iPosY)
{
	if(IsWindowed())
	{
		UpdateRectangles();
	}
}

/**
 * Get the current viewport (client) and screen rectangles
 **/
void IApplication::UpdateRectangles()
{
	//Get the viewport rectangle
	::GetClientRect(m_hWnd, &m_rcViewport);

	// Get the screen rectangle
	::memcpy(&m_rcScreen, &m_rcViewport, sizeof(RECT));
	::ClientToScreen(m_hWnd, (POINT*)&m_rcScreen.left);
	::ClientToScreen(m_hWnd, (POINT*)&m_rcScreen.right);
}

/**
 * Perform one main loop frame
 **/
void IApplication::DoFlip()
{
	if(IsActive() == FALSE)
		return;

	m_Timer.ReportFrame();

	//m_Timer.BeginTimer();

	// Call pure virtual OnFlip
	OnFlip();
}

/**
 * returns a reandom integer number in between iLow and iHigh
 **/
int IApplication::RandInt(const int iLow, const int iHigh)
{
	return (::rand() % ((iHigh + 1) - iLow) + iLow);
}

float IApplication::RandFloat(const float fLow, const float fHigh)
{
	static const float c = 1.f / (float)RAND_MAX;
	return ( (fHigh - fLow) * (float)(::rand() * c)) + fLow;
}

void IApplication::SetCursorVisibility(bool bValue)
{
	ShowCursor(bValue);
}