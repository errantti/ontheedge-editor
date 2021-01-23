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

#ifndef __IPARTICLESYSTEMSCENENODE_H__
#define __IPARTICLESYSTEMSCENENODE_H__

#include "..\\Scene\\ISceneNode.h"

#define MAX_EMITTER_PARTICLES		1000

/**
 * sEMITTER_PARAMS
 * Describes a single particle emitter type
 **/
struct sEMITTER_PARAMS
{
	/**
	 * Particle options
	 **/
	float			timeToLive;			// How long a single particle lives

	D3DXVECTOR3		position;			// Particles' position relative to the emitter's location
	D3DXVECTOR3		positionSpread;		// Position spread (pos.x = rand(+-spread.x) etc)

	D3DXVECTOR3		direction;			// Particles' speed
	D3DXVECTOR3		directionSpread;

	D3DXCOLOR		colorStart;			// Color of the particles at the beginning
	D3DXCOLOR		colorEnd;			// Color of the particles in the end

	float			scaleStart;			// Scale multiplier at the start (or sprite size if using point sprites)
	float			scaleEnd;			// Scale multiplier / pointsize in the end

	float			speedMin;			// Used to determine the exact speed of the particles if 
	float			speedMax;			// exactSpeedDetermination is true (slow)

	float			weightMin;			// Unused
	float			weightMax;			// Unused

	bool	inheritParentSpeed;			// If true then parent object's speed is added to the particle speed
	bool	hasMesh;					// If true then the particles are meshes, otherwise they are pont sprites
	bool	isBillboard;				// If particles are meshes, then this decides whether they are billboarded
	bool	additiveBlend;				// If true particles are blended additively, otherwise modularily
	bool	exactSpeedDetermination;	// See comments on speedMin and speedMax
	bool	replaceOldParticles;		// If true and capacity is full then emit replaces old particles with new

	int				customBlendMode;	// Used to indicate any custom blend settings that need to be set, hard coded values

	char			resourceFilename[128];	// Either fill mesh or texture filename depending on
										// which type the emitter is using

	/**
	 * Emit options
	 **/
	float			startTime;			// Time after which (from creation of particle system) 
										// emitter starts autoemitting

	float			emitFrequency;		// Determines the interval at which particles are emitted.

	bool			autoEmit;			// If true then the emitter automatically emits particles
										// at given frequency

	DWORD			particleCount;		// Number of particles emitted at each Emit-call
	DWORD			maxParticleCount;	// The maximum number of particles the emitter can hold at once

	char			name[64];			// Name of the emitter type

	int				initializerID;		// ID of the particle initializer to use, negative = no initializer
	float			initParams[4];		// Parameters for the particle initializer

	int				handlerID;			// ID of the particle handler to use, negative = no handler
	float			handlerParams[4];	// Parameters for the particle handler

	D3DXVECTOR3		gravity;			// Direction of gravity
	bool			hasGravity;			// True if particles are affected by gravity
	bool			useExternalGravity;	// If true, allows external handler to set the gravity

	int				numRotationMatrices;	// Number of rotation matrices created from mesh particles
	bool			useRotationMatrices;	// True if using rotation matrices
	bool			inheritParentRotation;	// If true, new rotation matrices inherit parent rotation upon creation
	D3DXVECTOR3		minRotationSpeed;	// These values are used to determine 
	D3DXVECTOR3		maxRotationSpeed;	// rotation deltas

	bool			useGravityPoint;	// True if emitter uses gravity point UNUSED
	D3DXVECTOR3		gravityPoint;		// Defines location of the gravity point relative to emitter
	float			gpRadius;			// Radius of the gravity point field
	float			gpPower;			// Power of the gravity poitn field

	void FillDefaults()
	{
		timeToLive			= 1.f;

		position			= D3DXVECTOR3(0.f, 0.f, 0.f);
		positionSpread		= D3DXVECTOR3(0.f, 0.f, 0.f);

		direction			= D3DXVECTOR3(0.f, 0.f, 0.f);
		directionSpread		= D3DXVECTOR3(0.f, 0.f, 0.f);

		colorStart			= D3DXCOLOR(1.f, 1.f, 1.f, 1.0f);
		colorEnd			= D3DXCOLOR(1.f, 1.f, 1.f, 0.f);

		scaleStart			= 3.f;
		scaleEnd			= 3.f;

		speedMin			= 0.f;
		speedMax			= 0.f;

		weightMin			= 0.f;
		weightMax			= 0.f;

		inheritParentSpeed	= false;
		hasMesh				= false;
		isBillboard			= false;
		additiveBlend		= true;
		exactSpeedDetermination = false;

		customBlendMode		= 0;

		resourceFilename[0] = '\0';

		startTime			= 0.f;
		emitFrequency		= 1.f;
		autoEmit			= true;

		particleCount		= 10;
		maxParticleCount	= 100;

		name[0]				= '\0';

		initializerID		= -1;
		handlerID			= -1;
		for(int i=0;i<4;++i) {
			initParams[i] = 0.f;
			handlerParams[i] = 0.f;
		}

		hasGravity			= false;
		gravity				= D3DXVECTOR3(0.f, -10.f, 0.f);

		numRotationMatrices = 2;
		useRotationMatrices = false;
		minRotationSpeed	= D3DXVECTOR3(0.f, 0.f, 0.f);
		maxRotationSpeed	= D3DXVECTOR3(0.f, 0.f, 0.f);

		useGravityPoint		= false;
		gravityPoint		= D3DXVECTOR3(0.f, 0.f, 0.f);
		gpRadius			= 1.f;
		gpPower				= 1.f;
	}
};

/*struct EMIT_PARAMS
{
	DWORD			dwCount;
	
	D3DXVECTOR3		vPosition;
	D3DXVECTOR3		vPositionSpread;

	D3DXVECTOR3		vDirection;
	D3DXVECTOR3		vDirectionSpread;

	D3DXCOLOR		cColor;
	D3DXCOLOR		cColorSpread;

	float			fSize;
	float			fSizeSpread;

	float			fLifeTime;
	float			fLifeTimeSpread;

	float			fWeight;
	float			fWeightSpread;

	float			fFrequency;

	float			fObjectLifetime;

	bool			bGravity;
	bool			bAutoEmit;
	bool			bAutoKillObject;

	void FillDefaults()
	{
		dwCount = 5;
		vPosition = D3DXVECTOR3(0.f, 0.f, 0.f);
		vPositionSpread = vPosition;

		vDirection = D3DXVECTOR3(0.f, 1.f, 0.f);
		vDirectionSpread = D3DXVECTOR3(1.f, 1.f, 1.f);

		cColor = D3DXCOLOR(255,255,255,255);
		cColorSpread = D3DXCOLOR(0,0,0,0);

		fSize = 5.f;
		fSizeSpread = 1.f;

		fLifeTime = 2.f;
		fLifeTimeSpread = 0.5f;

		fWeight = 2.f;
		fWeightSpread = 0.f;

		fFrequency = 0.2f;
		fObjectLifetime = 5.f;
		bGravity = TRUE;
		bAutoEmit = TRUE;
		bAutoKillObject = TRUE;
	}
};*/

class CParticleEmitter;

class IParticleSystemSceneNode : public ISceneNode  
{
public:
	IParticleSystemSceneNode(ISceneNode *parent, ISceneManager *manager, DWORD id, D3DXVECTOR3 pos, bool autoDestruct = FALSE, float duration = 4.f)
		: ISceneNode(parent, manager, id, pos, D3DXVECTOR3(0.f, 0.f, 0.f), D3DXVECTOR3(1.f, 1.f, 1.f)),
		m_bActive(TRUE), m_bAutoDestruct(autoDestruct), m_duration(duration)
	{}
	virtual ~IParticleSystemSceneNode() {}

	virtual void CreateEmitter(sEMITTER_PARAMS &params) = NULL;
	virtual void Release() = NULL;

	virtual aabbox3d &GetBoundingBox() {return m_bbox;}

	virtual void Render(bool isShadowPass) = NULL;
	virtual void Update(float fFrametime) = NULL;

	virtual void Emit() = NULL;
	virtual void InstantEmit() = NULL;
	virtual void InstantEmit(int index) = NULL;

	virtual D3DXVECTOR3 GetSpeed() = NULL;

	bool IsAutoDestructive() {return m_bAutoDestruct;}
	void SetIsAutoDestructive(bool value) {m_bAutoDestruct = value;}

	float GetDuration() {return m_duration;}
	void SetDuration(float value) {m_duration = value;}

	inline bool IsActive() const {return m_bActive;}
	inline void SetActive(bool value) {m_bActive = value;}

protected:
	aabbox3d m_bbox;

	bool m_bActive;

	bool m_bAutoDestruct;
	float m_duration;
};

#endif // #ifndef __IPARTICLESYSTEMSCENENODE_H__
