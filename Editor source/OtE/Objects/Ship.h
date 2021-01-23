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

#ifndef __CSHIP_H__
#define __CSHIP_H__

#include "Vessel.h"
#include "..\\Gadgets\\GadgetDatabase.h"
#include "..\\Items\\ShipPatch.h"

#define GADGET_CHANGE_TIME	2.f


struct SShipTemplate
{
	SVesselTemplate vt;
	float maxLoad, boostForce, boostEnergyConsumption;

	void FillDefaults()
	{
		vt.FillDefaults();
		boostEnergyConsumption = 100;
		boostForce = 30000.f;
		maxLoad = 300.f;
	}
};

class CShip : public CVessel
{
	friend class CPlayer;
	friend class CShipPatch;
public:
	CShip(long id=-1);
	virtual ~CShip();

	void ApplyTemplate(SShipTemplate t, bool bInitiateEffects = true, bool bRemoveOldEffects = false);

	virtual void Update(float frameTime);
	virtual bool BoostedThrust(float frameTime);

	virtual void Kill();

	void UpdateGadgets(bool g1Down, bool g2Down);

	//virtual SObjectInstanceInfo GetObjectInfo();

	inline float GetMaxLoad() {return m_maxLoad;}
	inline float GetLoad() {return m_load;}

	bool ApplyPatch(long id);

	void RemoveGadgets();

	void ReAttachPatches();

	std::vector<CShipPatch> *GetShipPatches() {return &m_patches;}

	IGadget *GetEquippedGadget(bool slot) {return m_gadgets[(int)slot];}

	static unsigned int GetShipCount() {return m_shipCount;}

protected:
	// mass of the ship's bulk, the mass of active gadgets is added to this to
	// caluculate the complete mass of the ship
	//float m_shipMass;
	// the maximum sum of equipped gadgets' masses
	float m_maxLoad;
	float m_load;

	float m_boostedThrust;
	float m_boostEnergyConsumption;

	float m_gadgetChangeTimeCounter;
	bool m_bChangingGadget;

	bool m_bG2DownPrev;
	float m_gadget2ActivationTimer;

	std::vector<CShipPatch> m_patches;

	static unsigned int m_shipCount;
};

#endif // #ifndef __CSHIP_H__
