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

#ifndef __CSHIELDGADGET_H__
#define __CSHIELDGADGET_H__

#include "IGadget.h"
#include "..\\ParticleEffects.h"

/**
 * Template for shield gadgets
 */
struct SShieldGadgetTemplate : public SCustomGadgetTemplate
{
	DAMAGE_TYPE type;
	float coefficient, energyConsumption;
	long particleEffectID;

	void FillDefaults()
	{
		type = DAMAGE_TYPE_ENERGY;
		coefficient = 0.9f;
		energyConsumption = 15.f;
		particleEffectID = -1;
	}
};

/**
 * Gadget type that shields the user against
 * some type of damage reducing the amount of
 * damage received
 */
class CShieldGadget : public IGadget
{
public:
	CShieldGadget(SShieldGadgetTemplate &t, SGadgetTemplate &gt);
	~CShieldGadget();

	void ApplyTemplate(SShieldGadgetTemplate &t, SGadgetTemplate &gt);

	//bool Equip(CVessel *vessel, float &energy);
	void UnEquip();
	void Update(float frameTime);

	bool Activate();

	bool IsActive() {return m_bActive;}

protected:
	void DeActivate();
	void SetDescription();

	SShieldGadgetTemplate m_template;

	SParticleSystemInfo *m_hitEffect;
	IParticleSystemSceneNode *m_node;

	bool m_bActive;
};

#endif // #ifndef __CSHIELDGADGET_H__