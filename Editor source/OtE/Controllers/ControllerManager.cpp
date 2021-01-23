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

#include "ControllerManager.h"
#include "HoveringController.h"
#include "SmoothRotationArcController.h"
#include "FixedRotationController.h"
#include "ActivateableDoorLiftController.h"

using namespace std;

CControllerManager *CControllerManager::m_this = NULL;

CControllerManager::CControllerManager()
{
	if(m_this == NULL)
		m_this = this;
}

CControllerManager::~CControllerManager()
{
	if(m_this == this)
		m_this = NULL;

	RemoveInstancedControllers();
	RemoveTemplates();
}

void CControllerManager::RemoveTemplates()
{
	map<long, IGameObjectController*>::iterator it = m_templates.begin();
	for(; it != m_templates.end(); ++it)
		delete it->second;
	m_templates.clear();
}

void CControllerManager::RemoveInstancedControllers()
{
	list<IGameObjectController*>::iterator it = m_instancedControllers.begin();
	for(; it != m_instancedControllers.end(); ++it)
		delete *it;
	m_instancedControllers.clear();
}

bool CControllerManager::RemoveInstancedController(IGameObjectController *corpse)
{
	list<IGameObjectController*>::iterator it = m_instancedControllers.begin();
	for(; it != m_instancedControllers.end(); ++it)
		if(*it == corpse)
		{
			delete *it;
			m_instancedControllers.erase(it);
			return true;
		}
	return false;
}

IGameObjectController *CControllerManager::GetController(long id)
{
	map<long, IGameObjectController*>::iterator it = m_templates.find(id);
	if(it != m_templates.end())
	{
		if(it->second->RequiresInstantiation())
			return CreateInstancedController(it->second);
		else
			return it->second;
	}

	return NULL;
}

IGameObjectController *CControllerManager::CreateInstancedController(IGameObjectController *c)
{
	if(c == NULL)
		return NULL;
	IGameObjectController *clone = c->Clone();
	m_instancedControllers.push_back(clone);
	return clone;
}

void CControllerManager::Initialize()
{
	CHoveringController *hover = new CHoveringController();
	m_templates.insert(std::pair<long, IGameObjectController*>(hover->GetControllerID(), hover));

	CFixedRotationController *rot = new CFixedRotationController();
	m_templates.insert(std::pair<long, IGameObjectController*>(rot->GetControllerID(), rot));

	CSmoothRotationArcController *sRot = new CSmoothRotationArcController();
	m_templates.insert(std::pair<long, IGameObjectController*>(sRot->GetControllerID(), sRot));

	CActivateableDoorLiftController *doorLift = new CActivateableDoorLiftController();
	m_templates.insert(std::pair<long, IGameObjectController*>(doorLift->GetControllerID(), doorLift));
}