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

#ifndef __CKINETICGADGET_H__
#define __CKINETICGADGET_H__

#include "IGadget.h"
#include "..\\ParticleEffects.h"

struct SKineticGadgetTemplate : public SCustomGadgetTemplate
{
	float force, energyConsumption, firingPace;
	float angle, radius;
	long particleEffectID;

	void FillDefaults()
	{
		force = 4000.f;
		energyConsumption = 15.f;
		firingPace = 0.4f;
		angle = 1.7f;
		radius = 40.f;
		particleEffectID = -1;
	}
};

/**
 * Gadget type that sends waves of kinetic
 * energy from the vessel bumping backwards
 * anything that it hits
 */
class CKineticGadget : public IGadget
{
public:
	CKineticGadget(SKineticGadgetTemplate &t, SGadgetTemplate &gt);
	~CKineticGadget();

	void ApplyTemplate(SKineticGadgetTemplate &t, SGadgetTemplate &gt);

	//bool Equip(CVessel *vessel, float &energy);
	//void UnEquip();
	void Update(float frameTime);

	bool Activate();

protected:
	void SetDescription();

	SKineticGadgetTemplate m_template;

	SParticleSystemInfo *m_activateEffect;

	float m_activationTimer;
};

#endif // #ifndef __CKINETICGADGET_H__