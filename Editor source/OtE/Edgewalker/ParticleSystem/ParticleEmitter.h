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

#ifndef __PARTICLEEMITTER_H__
#define __PARTICLEEMITTER_H__

#include <d3dx9.h>

#include "ParticleCache.h"
#include "IParticleSystemSceneNode.h"
#include "IParticleInitializer.h"
#include "IParticleHandler.h"

#define NUM_ROTATION_MATRICES		4

class CParticleEmitter
{
	friend class IParticleInitializer;
	friend class IParticleHandler;
public:
	struct sPARTICLE
	{
		D3DXVECTOR3		position;
		D3DXVECTOR3		direction;
		D3DXCOLOR		color;
		float			lifetime;
		float			scale;
		int				rotationIndex;
		bool			alive;
	};

	CParticleEmitter(sEMITTER_PARAMS &params, IParticleSystemSceneNode &owner);
	~CParticleEmitter();

	/**
	 * Updates all the particles' positions and other properties
	 * duration = time spent from the moment particle system was created
	 **/
	void Update(float frametime, float duration);

	/**
	 * Can be called every frame but keeps the frequency of the
	 * emits correct
	 **/
	void Emit();
	/**
	 * Instantly emits particles
	 **/
	void InstantEmit();

	/**
	 * Not to be used often. The optimized particle render pipeline is preferred
	 **/
	void Render();

	/**
	 * PopulateParticleVertexBuffer
	 * If this particle emitter is to be rendered this frame then
	 * CParticleCache will call this method and provide a pointer
	 * to a buffer to which all visible emitters are supposed to
	 * write their (point sprite) vertex data
	 * Returns number of particles written to the buffer
	 **/
	int PopulateParticleVertexBuffer(CParticleCache::POINTSPRITEVERTEX *buffer);

	/**
	 * RenderMeshes
	 * Called by CParticleEngine when it has set texture and mesh
	 * already. This method's function is to just apply the correct
	 * transformations and call the mesh rendering function
	 **/
	void RenderMeshes();

	CTexture *GetTexture() {return m_texture;}
	CMesh *GetMesh() {return m_mesh;}
	bool IsMeshEmitter() {return m_params.hasMesh;}
	inline bool IsAdditiveBlend() {return m_params.additiveBlend;}

	inline sEMITTER_PARAMS &GetEmitterParams() {return m_params;}

	inline DWORD GetParticleCount() {return m_numParticles;}

	inline void SetParticleInitializer(IParticleInitializer *init) {m_initializer = init;}
	inline void SetParticleHandler(IParticleHandler *handler) {m_handler = handler;}

	inline IParticleSystemSceneNode *GetOwner() {return &m_owner;}

private:
	bool ApplyEmitterParams(sEMITTER_PARAMS &params);
	void Release();

	void InitRotationMatrices();

	void ApplyGravity(float frametime);
	void ApplyPointGravity(float frametime);
	void UpdateRotationMatrices(float frametime);

	IParticleSystemSceneNode &m_owner;
	sEMITTER_PARAMS m_params;
	CTexture *m_texture;
	CMesh *m_mesh;
	D3DMATERIAL9 m_material;
	bool m_bAdditive;

	float m_emitDelay;

	sPARTICLE *m_particles;
	DWORD m_numParticles,		// Number of live particles
		m_capacity,				// Capacity of the particle buffer
		m_oldestIndex,			// Index of the oldest particle (and the first at the same time)
		m_newestIndex;			// Index of the youngest particle (and the last at the same time)

	D3DXCOLOR	m_colorShift;	// Optimization: precomputed value used when
	float		m_scaleShift;	// interpolating start -> end values

	IParticleInitializer *m_initializer;
	IParticleHandler *m_handler;

	IApplication *m_pApp;

	int	m_numRotMatrices;		// The number of rotation matrices if lesser than NUM_ROTATIONMATRICES

	D3DXMATRIX				m_mParticleMatrix[NUM_ROTATION_MATRICES];
	float					m_fParticleMatrixAngle[NUM_ROTATION_MATRICES][3];
	float					m_fParticleMatrixAngleDeltas[NUM_ROTATION_MATRICES][3];
};

#endif // #ifndef __PARTICLEEMITTER_H__