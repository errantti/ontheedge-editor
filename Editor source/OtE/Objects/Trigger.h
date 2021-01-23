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

#ifndef __CTRIGGER_H__
#define __CTRIGGER_H__

#include "..\\Edgewalker\\IGameObject.h"
#include "..\\Edgewalker\\EWScript\\Script.h"
#include "ObjectTypes.h"
#include "..\\ActionEvent.h"

#define MULTITRIGGER_DELAY		1.f

enum TRIGGER_TYPE {
	TRIGGER_TYPE_DIALOGUE=0,
	TRIGGER_TYPE_TERMINAL,
	TRIGGER_TYPE_SCREEN_MESSAGE,
	TRIGGER_TYPE_DAMAGES,
	TRIGGER_TYPE_EFFECT,
	TRIGGER_TYPE_ASSISTANTMSG
};

#define TRIGGER_MESSAGE_LENGTH		128

#define TRIGGER_OBJECT_PLAYER	1
#define TRIGGER_OBJECT_ENEMY	2
#define TRIGGER_OBJECT_OBJECT	4
#define TRIGGER_OBJECT_POSSESSED_ENEMY	8
#define TRIGGER_OBJECT_SPECIFIC_ID		16

struct STriggerInfo
{
	bool bMultitriggering, bActiveByDefault, bAEAtObject;
	TRIGGER_TYPE type;
	float multitriggeringDelay;
	float damage;
	DAMAGE_TYPE damageType;

	/// Dialogue / terminal ID
	long actionID;
	long triggeringObjectTypes;

	/// id of the specific object that can cause triggering
	long triggeringObjectID;
	long id;

	union {
		SActionEvent ae;
		char message[TRIGGER_MESSAGE_LENGTH];
	};
	aabbox3d bbox;

	void FillDefaults()
	{
		bMultitriggering = FALSE;
		bActiveByDefault = TRUE;
		bAEAtObject = TRUE;
		multitriggeringDelay = MULTITRIGGER_DELAY;
		type = TRIGGER_TYPE_DIALOGUE;
		actionID = -1;
		triggeringObjectID = -1;
		triggeringObjectTypes = TRIGGER_OBJECT_PLAYER;
		id = -1;
		bbox.minCorner = D3DXVECTOR3(0.f,0.f,0.f);
		bbox.maxCorner = D3DXVECTOR3(50.f, 50.f, 50.f);
		damage = 100.f;
		damageType = DAMAGE_TYPE_ENERGY;
	}
};

class CDynamicObject;
class CSound;
struct SParticleSystemInfo;

class CTrigger : public IGameObject
{
public:
	CTrigger();
	~CTrigger();

	virtual void Render(bool isShadowPass);
	virtual void Update(float frameTime);
	virtual void OnPreRender(bool isShadowPass);

	inline int GetActionID() {return m_actionID;}
	inline TRIGGER_TYPE GetTriggerType() {return m_triggerType;}
	inline bool IsMultitriggering() {return m_bMultitriggering;}

	void SetActionID(int id) {m_actionID = id;}
	void SetTriggerType(TRIGGER_TYPE type) {m_type = type;}
	void SetIsMultitriggering(bool value) {m_bMultitriggering = value;}

	void SetActive(bool value) {m_bActive = value;}

	virtual void RegisterCollision(IGeometricObject *object);

	STriggerInfo GetTriggerInfo();
	void ApplyTriggerInfo(STriggerInfo s);

	virtual aabbox3d GetTransformedBox();

	void Trigger(CDynamicObject *object, bool bPossessingShip = false);

	virtual void SetTranslation(D3DXVECTOR3 &vec);

private:
	bool m_bMultitriggering;	// If false this trigger is deleted from scene after it has been triggered once
	bool m_bTriggered;			// Just a backup
	bool m_bActive, m_bAEAtObject;
	TRIGGER_TYPE m_triggerType;
	long m_actionID;
	int m_triggeringObjectTypes;

	long m_triggeringObjectID;

	float m_damage;
	DAMAGE_TYPE m_damageType;

	float m_multiTriggerTimer, m_multitriggeringDelay;

	char m_scriptNameMsg[TRIGGER_MESSAGE_LENGTH];
	CActionEvent m_actionEvent;
	SActionEvent m_aeTemplate;

	aabbox3d m_transformedBox;
};

#endif // #ifndef __CTRIGGER_H__
