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

#ifndef __CVESSEL_H__
#define __CVESSEL_H__

#include "DynamicObject.h"
#include "..\\Edgewalker\\ParticleSystem\\ParticleSystemSceneNode.h"

#define BACK_THRUST_COEFFICIENT		-0.3f

#define NUM_GADGETS			2

struct SVesselTemplate
{
	SDynamicObjectTemplate dt;
	float thrust, levitationForce, zeroDistance;
	float levitationCoefficient, turnSpeed;
	
	SExplosion explosion;

	float baseEnergy, baseEnergyRegeneration;

	long
		peIDBrake,			// Particle effects
		peIDAccelerate,
		peIDTurn,

		soundIDBreak,
		soundIDAccelerate,
		soundIDTurn
	;

	D3DXVECTOR3 ammoSpawnAdjustment;

	float unused[12];	// Space reserved for future use to provide compatibility with additions

	void FillDefaults()
	{
		dt.FillDefaults();
		thrust = 15000.f;
		turnSpeed = 2.f;

		levitationCoefficient = -2.f;
		levitationForce = 100.f;
		zeroDistance = 7.f;

		baseEnergy = 100;
		baseEnergyRegeneration = 10;
		explosion.Init();

		peIDBrake = peIDAccelerate = peIDTurn = -1;
		soundIDBreak = soundIDAccelerate = soundIDTurn = -1;

		ammoSpawnAdjustment = D3DXVECTOR3(0.f, 0.f, 0.f);

		for(int i=0;i<12;++i)
			unused[i] = 0.f;
	}
};

class IGadget;

/**
 * CVessel is the interface for any vessel
 * (player ship, enemy ship / tower, etc..)
 */
class CVessel : public CDynamicObject
{
	friend class CPlayer;
public:
	CVessel(long id=-1);
	virtual ~CVessel();

	/// Turns the ship
	/**
	 * Turns the ship to wanted direction
	 * param coefficient = frame time * +-1.f (depending on 
	 * direction to which one wants the vessel to turn)
	 */
	virtual void Turn(float coefficient);

	virtual void TurnManual(float amount);

	/// Thrusts the ship forward
	/**
	 * Thrusts the vessel to the direction the vessel is facing at the
	 * force defined by m_thrust
	 */
	virtual void Thrust();

	/// Thrusts the ship forward at given multiple of the default speed
	virtual void Thrust(float coefficient);

	virtual void SideThrust(bool left);

	/// Thrusts the ship backward
	virtual void BackThrust();

	/// Applies a vertical upward force
	/**
	 * Applies a vertical upward force to the vessel. The size of the
	 * force depends on three factors:
	 * m_levitationForce - the size of force at zero distance
	 * m_zeroDistance - the zero distance from ground below
	 * m_levitationCoefficient - defines the linear falloff of the force as the 
	 *		ground gets further away (or vice versa)
	 * The levitation force is presented by a 2D-line where x-axis is the distance
	 * from the ground and y-axis the force applied to the vessel.
	 * m_levitationCoefficient defines the line's coefficient (as how steep it is)
	 * and m_levitationForce and m_zeroDistance position it
	 */
	virtual void Levitate(float groundDistance);

	virtual void Update(float frameTime);

	virtual void OnDeath();

	virtual void InflictDamage(float amount, bool bEnemy, DAMAGE_TYPE type, CVessel *source = NULL);

	inline float GetEnergy() {return m_energy;}
	inline float GetMaxEnergy() {return m_maxEnergy;}

	bool IsFullEnergy();
	bool IsFullHealth();

	void AddEnergy(float amount);

	/**
	 * True if the vessel either really is an enemy or just seems
	 * to be one (player is possessing it and hasn't aggroed other
	 * enemies)
	 */
	bool IsVisiblyEnemy() {return m_bVisiblyEnemy;}

	/// True if the vessel really is an enemy
	bool IsEnemy() {return m_bIsEnemy;}

	void SetLevitationForce(float value) {m_levitationForce = value;}
	void SetZeroDistance(float value) {m_zeroDistance = value;}
	void SetLevitationCoefficient(float value) {m_levitationCoefficient = value;}

	//inline void SetBaseLevitationForce() {m_levitationForce = m_baseLevitationForce;}
	inline void AddToLevitationForce(float value) {m_levitationForce += value;}

	void SetThrust(float value) {m_thrust = value;}
	inline void AddToThrust(float value) {m_thrust += value;}
	//inline void SetBaseThrust() {m_thrust = m_baseThrust;}

	void ApplyVesselTemplate(SVesselTemplate t, bool bInitiateEffects = true, bool bRemoveOldEffects = false);

	/*void SetAccelerateParticleEmitter(CParticleSystemSceneNode *emitter) {m_peAccelerate = emitter;}
	void SetAccelerateParticleEmitter(CParticleSystemSceneNode *emitter) {m_peBrake = emitter;}
	void SetAccelerateParticleEmitter(CParticleSystemSceneNode *emitter) {m_peTurn = emitter;}/**/
	//void SetWeaponParticleEmitter(CParticleSystemSceneNode *emitter) {m_weaponEmitter = emitter;}

	float GetStealthCoefficient() {return m_stealthCoefficient;}
	void SetStealthCoefficient(float value) {m_stealthCoefficient = value;}

	void SetAlteredDamageType(DAMAGE_TYPE type) {m_alteredDamageType = type;}
	void SetAlteredDamageTypeCoefficient(float value) {m_alteredDamageTypeCoefficient = value;}

	IGadget *GetGadget(int index);
	void SetGadget(int index, IGadget *gadget);

	float GetTurnSpeed() {return m_turnSpeed;}

	float &GetEnergyReference() {return m_energy;}

	virtual void Render(bool isShadowPass);
	virtual void OnPreRender(bool isShadowPass);

	void CreateShadow(bool bForceCreate);

	D3DXVECTOR3 GetAmmoSpawnPosition();

protected:
	void CreateVesselEffects(SVesselTemplate &t);

	IGadget *m_gadgets[NUM_GADGETS];

	float m_turnSpeed, m_thrust;
	float m_levitationForce, m_zeroDistance, m_levitationCoefficient;

	DAMAGE_TYPE m_alteredDamageType;
	float m_alteredDamageTypeCoefficient;

	SExplosion m_explosion;
	
	float m_energy, m_maxEnergy, m_energyRegeneration;

	/// How long time the vessel is stunned
	float m_stunCounter;

	bool m_bVisiblyEnemy;
	bool m_bIsEnemy;

	D3DXVECTOR3 m_ammoSpawnAdjustment;

	CMeshSceneNode *m_shadow;

	/// From 0.f to 1.f, the alpha value of the vessel / aggro range coefficient
	float m_stealthCoefficient;

	IParticleSystemSceneNode 
		*m_peAccelerate,
		*m_peBrake,
		*m_peTurn;

	CSound 
		*m_soundAccelerate,
		*m_soundBreak,
		*m_soundTurn;
};

#endif // #ifndef __CVESSEL_H__
