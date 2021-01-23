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

#include "ParticleEmitter.h"
#include "..\\ResourceManager.h"
#include "..\\Scene\\CameraSceneNode.h"
#include <d3d9.h>

CParticleEmitter::CParticleEmitter(sEMITTER_PARAMS &params, IParticleSystemSceneNode &owner)
	: m_particles(NULL), m_owner(owner)
{
	m_pApp = IApplication::GetApp();

	if(!ApplyEmitterParams(params))
	{
		CPrinter::log("Failed creating particle emitter");
	}
}

CParticleEmitter::~CParticleEmitter()
{
	Release();
}

bool CParticleEmitter::ApplyEmitterParams(sEMITTER_PARAMS &params)
{
	Release();

	m_params = params;
	m_numParticles = 0;
	m_emitDelay = 0.f;

	if(params.useRotationMatrices)
	{
		m_numRotMatrices = params.numRotationMatrices;
		if(m_numRotMatrices < 0)
			m_numRotMatrices = 1;
		if(m_numRotMatrices > NUM_ROTATION_MATRICES)
			m_numRotMatrices = NUM_ROTATION_MATRICES;
		InitRotationMatrices();
	}

	m_capacity = params.maxParticleCount;
	if(m_capacity < 1)
		m_capacity = 1;
	if(m_capacity > MAX_EMITTER_PARTICLES)
		m_capacity = MAX_EMITTER_PARTICLES;

	m_particles = new sPARTICLE[m_capacity];
	if(m_particles == NULL)
	{
		CPrinter::log("Error creating particle array: out of memory");
		return false;
	}
	for(DWORD i=0; i<m_capacity; ++i)
		m_particles[i].alive = false;

	if(params.particleCount > m_capacity)
		params.particleCount = m_capacity;

	m_oldestIndex = 0;
	m_newestIndex = 0;

	// Compute shift-values
	float onePerTTL = 1.f / params.timeToLive;
	m_scaleShift = (params.scaleEnd - params.scaleStart) * onePerTTL;
	m_colorShift.r = (params.colorEnd.r - params.colorStart.r) * onePerTTL;
	m_colorShift.g = (params.colorEnd.g - params.colorStart.g) * onePerTTL;
	m_colorShift.b = (params.colorEnd.b - params.colorStart.b) * onePerTTL;
	m_colorShift.a = (params.colorEnd.a - params.colorStart.a) * onePerTTL;
	/* Now the interpolation can be calculated each frame as follows:
	 * scale += m_scaleShift * frametime;
	 */

	// TODO: add initializer and handler lookup
	m_initializer = NULL;
	m_handler = NULL;

	// TODO: fetch texture / mesh pointer
	if(params.hasMesh)
		m_mesh = CResourceManager::GetManager().GetMesh(params.resourceFilename);
	else
		m_texture = CResourceManager::GetManager().GetTexture(params.resourceFilename);

	return true;
}

void CParticleEmitter::Release()
{
	if(m_particles != NULL)
	{
		delete [] m_particles;
		m_particles = NULL;
	}
	m_capacity = 0;
	m_numParticles = 0;
}

void CParticleEmitter::Update(float frametime, float duration)
{
	if(m_handler != NULL)
	{
		if(m_handler->RequiresCompleteControl())
		{
			// Skip the hardcoded update if the external handler
			// requres total control over the update procedure
			m_handler->UpdateParticles(*(this), frametime);
			return;
		}
	}
	// Optimization:
	// m_particles array is a circular buffer where the
	// new particle is added next to the last one and
	// if last one was the last of the array then to the
	// beginning of the array
	// Thus all the particles are in one block if you
	// consider the array 'circular'.
	// By making a couple of if-checks we can basically
	// loop through all the live particles without
	// having a single if-statement inside loops

	DWORD i, limit;
	sPARTICLE *part = &m_particles[m_oldestIndex];

	if(m_numParticles)
	{
	if(m_newestIndex<m_oldestIndex)
		limit = m_capacity;
	else
		limit = m_newestIndex+1;
	}
	else
		limit = m_oldestIndex = m_newestIndex = 0;


	for(i = m_oldestIndex; i<limit;)
	{
		part = &m_particles[i];
		if(part->lifetime > 0.f)
		{
			// Update the position and all the other properties of the particle
			part->lifetime -= frametime;

			part->scale += m_scaleShift * frametime;
			part->color.r += m_colorShift.r * frametime;
			part->color.g += m_colorShift.g * frametime;
			part->color.b += m_colorShift.b * frametime;
			part->color.a += m_colorShift.a * frametime;

			part->position.x += part->direction.x * frametime;
			part->position.y += part->direction.y * frametime;
			part->position.z += part->direction.z * frametime;
		}
		else
		{
			if(part->alive)
			{
				--m_numParticles;
				part->alive = false;
			}
			++m_oldestIndex;
		}/**/

		++i;
		if(i == m_capacity)
		{
			if(m_newestIndex < m_oldestIndex)
			{
				i = 0;
				limit = m_newestIndex;
				if(m_oldestIndex >= m_capacity)
					m_oldestIndex = 0;
			}
		}
	}

	if(m_emitDelay > 0.f)
		m_emitDelay -= frametime;
	// Autoemit
	if(m_params.autoEmit)
	{
		if(m_params.startTime <= duration)
			Emit();
	}

	if(m_oldestIndex >= m_capacity)
		m_oldestIndex = 0;

	if(m_handler != NULL)
		m_handler->UpdateParticles(*(this), frametime);

	if(m_params.hasMesh)
	{
		if(m_params.useRotationMatrices)
			UpdateRotationMatrices(frametime);
	}
	if(m_params.hasGravity)
		ApplyGravity(frametime);
	if(m_params.useGravityPoint)
		ApplyPointGravity(frametime);
}

void CParticleEmitter::ApplyGravity(float frametime)
{
	DWORD i, limit;
	D3DXVECTOR3 vTemp;

	// compute the one frame gravity vector
	vTemp.x = m_params.gravity.x * frametime;
	vTemp.y = m_params.gravity.y * frametime;
	vTemp.z = m_params.gravity.z * frametime;

	// Optimization:
	// m_particles array is a circular buffer where the
	// new particle is added next to the last one and
	// if last one was the last of the array then to the
	// beginning of the array
	// Thus all the particles are in one block if you
	// consider the array 'circular'.
	// By making a couple of if-checks we can basically
	// loop through all the live particles without
	// having a single if-statement inside loops

	sPARTICLE *pPARTICLE = &m_particles[m_oldestIndex];

	if(m_newestIndex<m_oldestIndex)
		limit = m_capacity;
	else
		limit = m_newestIndex+1;

	for (i=m_oldestIndex; i<limit; ++i)
	{
		// Weight to be added later if it's needed
		/*pPARTICLE->direction.x += vTemp.x * pPARTICLE->weight;
		pPARTICLE->direction.y += vTemp.y * pPARTICLE->weight;
		pPARTICLE->direction.z += vTemp.z * pPARTICLE->weight;*/

		pPARTICLE->direction.x += vTemp.x;
		pPARTICLE->direction.y += vTemp.y;
		pPARTICLE->direction.z += vTemp.z;
		++pPARTICLE;
	}

	if(m_newestIndex < m_oldestIndex)
	{
		limit = m_newestIndex+1;
		pPARTICLE = &m_particles[0];
		for (i=0; i<limit; ++i)
		{
			// Weight to be added later if it's needed
			/*pPARTICLE->direction.x += vTemp.x * pPARTICLE->weight;
			pPARTICLE->direction.y += vTemp.y * pPARTICLE->weight;
			pPARTICLE->direction.z += vTemp.z * pPARTICLE->weight;*/

			pPARTICLE->direction.x += vTemp.x;
			pPARTICLE->direction.y += vTemp.y;
			pPARTICLE->direction.z += vTemp.z;
			++pPARTICLE;
		}
	}
}

void CParticleEmitter::ApplyPointGravity(float frametime)
{
	/*DWORD i;
	D3DXVECTOR3 vTemp;
	float fLengthSq, fTemp;

	for (i=0; i<m_dwNumParticles; i++)
	{
		PARTICLE *pParticle = &m_pParticles[i];

		if (pParticle->fLifeTime > 0.0f)
		{
			// compute vector from gravitation point
			// into the particle
			vTemp.x = vPosition.x - pParticle->vPosition.x;
			vTemp.y = vPosition.y - pParticle->vPosition.y;
			vTemp.z = vPosition.z - pParticle->vPosition.z;

			// compute the squared distance
			fLengthSq = vTemp.x*vTemp.x + vTemp.y*vTemp.y + vTemp.z*vTemp.z;

			if (fLengthSq < 0.0001f)
			{
				continue;
			}

			if (fLengthSq < fRadius*fRadius)
			{
				// particle is inside of the gravity
				// range

				// compute the actual distance
				fLengthSq = sqrtf(fLengthSq);

				fTemp = 1.0f - (fLengthSq / fRadius);
				fTemp *= fPower;
				fTemp *= pParticle->fWeight;
				fTemp *= fFrametime;

				pParticle->vDirection.x += vTemp.x / fLengthSq * fTemp;
				pParticle->vDirection.y += vTemp.y / fLengthSq * fTemp;
				pParticle->vDirection.z += vTemp.z / fLengthSq * fTemp;
			}
		}
	}*/
}

void CParticleEmitter::UpdateRotationMatrices(float frametime)
{
	int i,j;
	for (i=0; i<m_numRotMatrices; ++i)
	{
		for (j=0; j<3; ++j)
		{
			// increase the matrix angles by rotation
			// deltas
			m_fParticleMatrixAngle[i][j] +=
				m_fParticleMatrixAngleDeltas[i][j] * frametime;
		}

		D3DXMatrixRotationYawPitchRoll(	&m_mParticleMatrix[i],
										m_fParticleMatrixAngle[i][0],
										m_fParticleMatrixAngle[i][1],
										m_fParticleMatrixAngle[i][2]);
	}
}

void CParticleEmitter::InitRotationMatrices()
{
	int i,j;
	for (i=0; i<m_numRotMatrices; ++i)
	{
		for (j=0; j<3; ++j)
		{
			m_fParticleMatrixAngle[i][j] = 0.0f;
			m_fParticleMatrixAngleDeltas[i][j] = 
				m_pApp->RandFloat(m_params.minRotationSpeed[j], m_params.maxRotationSpeed[j]);
		}

		D3DXMatrixIdentity(&m_mParticleMatrix[i]);
		if(m_params.inheritParentRotation)
		{
			D3DXMATRIX parent = m_owner.GetAbsoluteTransformation();

			// Copy the 3x3 (rotation / scale) part of the parent matrix
			m_mParticleMatrix[i]._11 = parent._11;
			m_mParticleMatrix[i]._12 = parent._12;
			m_mParticleMatrix[i]._13 = parent._13;
			m_mParticleMatrix[i]._21 = parent._21;
			m_mParticleMatrix[i]._22 = parent._22;
			m_mParticleMatrix[i]._23 = parent._23;
			m_mParticleMatrix[i]._31 = parent._31;
			m_mParticleMatrix[i]._32 = parent._32;
			m_mParticleMatrix[i]._33 = parent._33;
		}
	}
}

void CParticleEmitter::Emit()
{
	if(m_emitDelay <= 0.f)
		InstantEmit();
}

void CParticleEmitter::InstantEmit()
{
	if(m_numParticles == m_capacity)
		return;

	DWORD addcount = m_params.particleCount;
	if(!m_params.replaceOldParticles)
	{
		if(m_numParticles + addcount > m_capacity)
			addcount = m_capacity - m_numParticles;
	}

	if(addcount < 1)
		return;

	if(m_numParticles == 0)
	{
		m_oldestIndex = 0;	
		m_newestIndex = -1;
	}

	m_emitDelay = m_params.emitFrequency;

	m_numParticles += addcount;
	if(m_numParticles > m_capacity)
		m_numParticles = m_capacity;

	DWORD arrayIndex = m_newestIndex+1;
	if(arrayIndex >= m_capacity)
		arrayIndex = 0;
	sPARTICLE *pParticle = &m_particles[arrayIndex];

	const D3DXMATRIX &transform = m_owner.GetAbsoluteTransformation();

	D3DXVECTOR3 parentSpeed = D3DXVECTOR3(0.f, 0.f, 0.f);
	if(m_params.inheritParentSpeed)
	{
		parentSpeed = m_owner.GetSpeed();
	}

	bool replaced = false;
	for(DWORD i=0; i < addcount; ++i)
	{
		if(pParticle->alive)
			replaced = true;
		else
			pParticle->alive = true;

		// initialise the sPARTICLE position
		pParticle->position = m_params.position;

		pParticle->position.x += m_pApp->RandFloat(-m_params.positionSpread.x, m_params.positionSpread.x);
		pParticle->position.y += m_pApp->RandFloat(-m_params.positionSpread.y, m_params.positionSpread.y);
		pParticle->position.z += m_pApp->RandFloat(-m_params.positionSpread.z, m_params.positionSpread.z);

		// transform the sPARTICLE position from local coordinate system
		// into the world coordinate system
		D3DXVec3TransformCoord(	&pParticle->position,
								&pParticle->position,
								&transform);

		
		// initialise the sPARTICLE direction
		pParticle->direction = m_params.direction;
		pParticle->direction.x += m_pApp->RandFloat(-m_params.directionSpread.x, m_params.directionSpread.x);
		pParticle->direction.y += m_pApp->RandFloat(-m_params.directionSpread.y, m_params.directionSpread.y);
		pParticle->direction.z += m_pApp->RandFloat(-m_params.directionSpread.z, m_params.directionSpread.z);

		// transform the sPARTICLE direction into the world coordinate
		// system
		D3DXVec3TransformNormal(	&pParticle->direction,
									&pParticle->direction,
									&transform);

		if(m_params.inheritParentSpeed)
			pParticle->direction += parentSpeed;

		if(m_params.exactSpeedDetermination)
		{
			D3DXVec3Normalize(&pParticle->direction, &pParticle->direction);
			pParticle->direction *= m_pApp->RandFloat(m_params.speedMin, m_params.speedMax);
		}

		if(m_params.useRotationMatrices)
			pParticle->rotationIndex = m_pApp->RandInt(0, m_numRotMatrices-1);

		// initialise the life time
		pParticle->lifetime = m_params.timeToLive;

		// initialise the particle size
		pParticle->scale = m_params.scaleStart;

		// initialize the particle color
		pParticle->color = m_params.colorStart;

		m_newestIndex = arrayIndex;
		++arrayIndex;
		if(arrayIndex >= m_capacity)
		{
			arrayIndex = 0;
			pParticle = &m_particles[0];
		}
		else
			++pParticle;
	}
	if(replaced)
		m_oldestIndex = arrayIndex;
}

int CParticleEmitter::PopulateParticleVertexBuffer(CParticleCache::POINTSPRITEVERTEX *buffer)
{
	if(m_params.hasMesh || m_numParticles == 0)
		return 0;

	CParticleCache::POINTSPRITEVERTEX *iterator = buffer;

	DWORD i, limit;

	if(m_newestIndex<m_oldestIndex)
		limit = m_capacity;
	else
		limit = m_newestIndex+1;

	sPARTICLE *pPARTICLE = &m_particles[m_oldestIndex];

	// Use the circular buffer optimization trick again to
	// avoid in-loop if-statements

	for (i=m_oldestIndex; i<limit; ++i)
	{
		iterator->vPosition = pPARTICLE->position;
		iterator->pointSize = pPARTICLE->scale;
		iterator->dwDiffuse = pPARTICLE->color;
		++iterator;
		++pPARTICLE;
	}

	if(m_newestIndex < m_oldestIndex)
	{
		limit = m_newestIndex+1;
		pPARTICLE = &m_particles[0];
		for (i=0; i<limit; ++i)
		{
			iterator->vPosition = pPARTICLE->position;
			iterator->pointSize = pPARTICLE->scale;
			iterator->dwDiffuse = pPARTICLE->color;
			++iterator;
			++pPARTICLE;
		}
	}

	return m_numParticles;
}

void CParticleEmitter::RenderMeshes()
{
	if(!m_params.hasMesh || m_numParticles == 0)
		return;

	// Apply transform
	DWORD i, limit;

	if(m_newestIndex<m_oldestIndex)
		limit = m_capacity;
	else
		limit = m_newestIndex+1;

	sPARTICLE *pParticle = &m_particles[m_oldestIndex];

	// Use the circular buffer optimization trick again to
	// avoid in-loop if-statements

	D3DXMATRIX mScale;
	D3DXMATRIX mParticle;
	const D3DXMATRIX &transform = m_owner.GetAbsoluteTransformation();

	IDirect3DDevice9 *pDevice = GetDXApp()->GetDevice();

	if(m_params.additiveBlend)
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	else
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	for (i=m_oldestIndex; i<limit;)
	{
		pParticle = &m_particles[i];

		if(m_params.isBillboard)
		{
			mParticle = ISceneManager::GetManager()->GetActiveCamera()->GetBillboardMatrix();
		}
		else
		{
			if(m_params.useRotationMatrices)
				mParticle = m_mParticleMatrix[pParticle->rotationIndex];
			else
				D3DXMatrixIdentity(&mParticle);
		}

		if(pParticle->scale != 1.f)
		{
			D3DXMatrixScaling(	&mScale, pParticle->scale, pParticle->scale, pParticle->scale);
			mParticle *= mScale;
		}

		// set the particle position
		mParticle._41 = pParticle->position.x;
		mParticle._42 = pParticle->position.y;
		mParticle._43 = pParticle->position.z;

		pDevice->SetTransform(D3DTS_WORLD, &mParticle);

		m_material.Ambient.r = pParticle->color.r;
		m_material.Ambient.g = pParticle->color.g;
		m_material.Ambient.b = pParticle->color.b;
		m_material.Ambient.a = pParticle->color.a;
		//m_material.Diffuse = m_material.Specular = m_material.Ambient;
		m_material.Diffuse = m_material.Emissive = m_material.Ambient;
		pDevice->SetMaterial(&m_material);

		// Render the mesh
		m_mesh->RenderWithoutTexture();

		++i;
		if(i == m_capacity)
		{
			if(m_newestIndex < m_oldestIndex)
			{
				i = 0;
				limit = m_newestIndex;
				if(m_oldestIndex >= m_capacity)
					m_oldestIndex = 0;
			}
		}
	}
}

void CParticleEmitter::Render()
{
	/*
	LPDIRECT3DDEVICE9 pDevice = GetDXApp()->GetDevice();
	if(m_activeParticles == 0)
		return;

	// set identity matrix as a world transform
	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);
	pDevice->SetTransform(D3DTS_WORLD, &m);

	DWORD i;
	float fPhase = 0.0f;
	float fTemp = 0.0f;
	CTexture *pLastTexture = NULL;
	DWORD r,g,b,a;

	// set the 3d device settings for the point sprites
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// set the point sprite settings
	pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);

	pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&fTemp));
	pDevice->SetRenderState(D3DRS_POINTSCALE_A, *((DWORD*)&fTemp));
	pDevice->SetRenderState(D3DRS_POINTSCALE_B, *((DWORD*)&fTemp));
	fTemp = 1.0f;
	pDevice->SetRenderState(D3DRS_POINTSCALE_C, *((DWORD*)&fTemp));

	// set the rendering FVF
	pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	// if we only have one texture for the sPARTICLE
	// (no animation), set it to device before the
	// rendering loop
	if (m_dwNumTextures == 1)
	{
		//pDevice->SetTexture(0, m_pTextures[0]);
		GetDXApp()->SetTexture(m_pTextures[0]);
	}

	for (i=0; i<m_dwNumParticles; i++)
	{
		sPARTICLE *pPARTICLE = &m_pParticles[i];

		if (pPARTICLE->fLifeTime > 0.0f)
		{
			// render the sPARTICLE

			// compute the life time phase, this is 0.0 when at
			// the beginning of the sPARTICLE life, and 1.0 when
			// at the end of the sPARTICLE life.
			fPhase = pPARTICLE->fLifeTime / pPARTICLE->fMaxLifeTime;

			if (m_dwNumTextures > 1)
			{
				// compute the texture index
				DWORD dwIndex = (DWORD)((float)m_dwNumTextures * fPhase);

				// set the texture into the device, but only
				// when it has really changed
				if (m_pTextures[dwIndex] != pLastTexture)
				{
					//pDevice->SetTexture(0, m_pTextures[dwIndex]);
					GetDXApp()->SetTexture(m_pTextures[dwIndex]);
					pLastTexture = m_pTextures[dwIndex];
				}
			}

			POINTSPRITEVERTEX	sprite;
			sprite.vPosition = pPARTICLE->vPosition;

			sprite.dwDiffuse = pPARTICLE->dwColor;
			if ((m_dwFlags & EMITTERFLAGS_FADE))
			{
				a = ((sprite.dwDiffuse & 0xff000000) >> 24);
				r = ((sprite.dwDiffuse & 0x00ff0000) >> 16);
				g = ((sprite.dwDiffuse & 0x0000ff00) >> 8);
				b = ((sprite.dwDiffuse & 0x000000ff));

				sprite.dwDiffuse = (DWORD)(fPhase * 255.0f);

				a = ((a * sprite.dwDiffuse) >> 8);
				r = ((r * sprite.dwDiffuse) >> 8);
				g = ((g * sprite.dwDiffuse) >> 8);
				b = ((b * sprite.dwDiffuse) >> 8);

				sprite.dwDiffuse = (a << 24) | (r << 16) | (g << 8) | (b);
			}

			// compute the sPARTICLE size
			fTemp = pPARTICLE->fSize;
			if ((m_dwFlags & EMITTERFLAGS_SCALE))
			{
				fTemp *= fPhase;
			}


			// compute the sPARTICLE color
			sprite.dwDiffuse = pPARTICLE->dwColor;
			if ((m_dwFlags & EMITTERFLAGS_FADE))
			{
				D3DXCOLOR color(sprite.dwDiffuse);
				color.r *= fPhase;
				color.g *= fPhase;
				color.b *= fPhase;
				color.a *= fPhase;
				sprite.dwDiffuse = color;
			}


			pDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&pPARTICLE->fSize));

			pDevice->DrawPrimitiveUP(	D3DPT_POINTLIST,
										1,
										&sprite,
										16);

		}

	}

	// set the basic 3d device settings
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// set the point sprite settings
	pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	*/
}