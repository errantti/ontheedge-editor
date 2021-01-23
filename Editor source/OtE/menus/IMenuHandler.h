
/*********************************
 *
 *		On the Edge
 * Copyright (2007) Azuris Productions
 * Jukka Tykkyläinen
 * http://azuris.ote-game.com
 *
 * IMenuHandler - interface and basic functionality 
 * for OtE menu and game view implementation
 *
 *********************************/

#ifndef __IMENUHANDLER_H__
#define __IMENUHANDLER_H__

#include "IMenuManager.h"
#include "..\\Sound\\Sound.h"

class IMenuHandler : public IEventReceiver
{
public:
	IMenuHandler(MENU_ID id, IMenuManager *mgr);

	virtual ~IMenuHandler();

	// InitializeMenu
	// Loads the menu controls to the gui environment
	virtual bool InitializeMenu() = 0;
	// removes the menu controls from the gui environment
	virtual void OnClose() = 0;

	// handler for messages for this menu
	virtual bool OnEvent(SEvent event) = 0;

	void SetWindowHandle(CGUIWindow *window) {m_window = window;}

	MENU_ID GetID() {return m_ID;}

	void SendCloseMessage();

	void SetOpenSound(const char *filename);
	void SetMusic(const char *filename);
	void SetActionSound(const char *filename);
	void SetHoverSound(const char *filename);

	void PlayOpenSound();
	void PlayActivationSound();
	void PlayHoverSound();
	void PlayMusic();

protected:
	bool UpdateSelection(bool bConfirmWindow);
	void HandleRoutineInput(SEvent event);

	// Callback method which is called when menu back (usually Esc) is pressed
	// Should return true to signal that menu is closing down but the method
	// implementation is still responsible for actually executing the closedown
	// procedure
	virtual bool OnMenuBackPressed();

	CGUIEnvironment *m_environment;
	IMenuManager *m_manager;

	CGUIWindow *m_window;

	MENU_ID m_ID;

	bool m_bEscDown, m_bDownDown, m_bUpDown;
	bool m_bEnterDown;

	CSound *m_openSound, *m_music, *m_actionSound, *m_hoverSound;
};


#endif // #ifndef __IMENUHANDLER_H__