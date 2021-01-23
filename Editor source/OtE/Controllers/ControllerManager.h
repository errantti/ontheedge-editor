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

#ifndef __CCONTROLLERMANAGER_H__
#define __CCONTROLLERMANAGER_H__

#include "..\\Edgewalker\\IGameObject.h"
#include <map>
#include <list>

#define CONTROLLER_ID_PLAYER		10
#define CONTROLLER_ID_ENEMY			11

/**
 * Stores and manages game object controller objects
 * which can be attached to objects (making elevators,
 * doors, environmental animations etc)
 */
class CControllerManager
{
public:
	CControllerManager();
	~CControllerManager();

	IGameObjectController *GetController(long id);

	/**
	 * Makes instances of all general and instanced controllers
	 * and stores them into m_generalControllers and 
	 * m_instancedTemplates
	 */
	void Initialize();

	bool RemoveInstancedController(IGameObjectController *corpse);

	void RemoveTemplates();
	void RemoveInstancedControllers();

	std::map<long, IGameObjectController*> *GetControllerTemplates() {return &m_templates;}

	static CControllerManager &GetManager() {return *m_this;}

private:
	IGameObjectController *CreateInstancedController(IGameObjectController *c);

	//std::map<long, IGameObjectController*> m_generalControllers;
	std::map<long, IGameObjectController*> m_templates;
	std::list<IGameObjectController*> m_instancedControllers;

	static CControllerManager *m_this;
};

#endif // #ifndef __CCONTROLLERMANAGER_H__