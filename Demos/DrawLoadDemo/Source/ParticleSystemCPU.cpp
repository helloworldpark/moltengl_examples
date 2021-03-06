/*!****************************************************************************
 @File          ParticleSystemCPU.cpp

 @Title         ParticleSystemCPU

 @Copyright     Copyright (C) by Imagination Technologies Limited.

 @Platform      Independant

 @Description   Particle system implemented using the CPU.
******************************************************************************/

#include "ParticleSystemCPU.h"

static float randf()
{
	return (rand() / (float)RAND_MAX);
}

static void EmitParticle(Particle *pParticle, Emitter *pEmitter, float fSeed)
{	
	// Generate random parameters for spawn position calculations
	float rand_val = randf();
	float u = randf() * pEmitter->fHeight;
	float r = randf() * pEmitter->fRadius;
	PVRTVec4 pos(r * (float) sin(rand_val), u, r * (float) cos(rand_val), 1.0f);

	// Transform according to emitter orientation
	pParticle->position = pEmitter->mTransformation * pos;
	
	// Give it random speed (the origin is assumed to be at the origin, otherwise adapt code below)
	pParticle->velocity = pParticle->position * rand_val * 4.0f;

	// Give it a random size and color
	pParticle->radius = 0.3 + (randf() * 0.6);
	pParticle->color = PVRTVec4(randf(), randf(), randf(), 1.0);
}

/*
 *
 */
ParticleSystemCPU::ParticleSystemCPU() 
{
}


/*
 *
 */
ParticleSystemCPU::~ParticleSystemCPU()
{
}


/*
 *
 */
bool ParticleSystemCPU::Update(float step)
{
	const size_t numParticles = m_aParticles.GetSize();
	for (unsigned int i=0; i < numParticles; i++)
	{
		Particle &particle = m_aParticles[i];		
		particle.lifespan -= step;
		
		if (particle.lifespan < 0.0f)
		{
			EmitParticle(&particle, &m_sEmitter, particle.lifespan);			
			particle.lifespan = randf() * 19.0f + 12.0f;
			continue;
		}
		else
		{
			// calculate the next position
			particle.position = particle.velocity * step + particle.position;
			// and update velocity (very simplified model using typical vertical acceleration)
			particle.velocity.y = particle.velocity.y + step * -0.98f;

			// collide against spheres (very fake, but looks convincing)
			const unsigned int uiNumSpheres = m_aSpheres.GetSize();
			for (unsigned int i=0; i < uiNumSpheres; i++)
			{
				const Sphere sphere = m_aSpheres[i];
				const float radius_sq = m_aSquaredRadii[i];
				// Check whether particle penetrated the sphere
				PVRTVec3 sphere_to_next_pos = particle.position - sphere.aPosition;
				if (sphere_to_next_pos.lenSqr() < radius_sq)
				{			
					// Project it back to sphere surface and do a very simple and fast velocity inversion
					PVRTVec3 dir_to_circle = sphere_to_next_pos.normalized();
					particle.position = PVRTVec3(sphere.aPosition) + dir_to_circle * sphere.fRadius;
					particle.velocity = dir_to_circle * particle.velocity.length() * 0.5f;
					// One collision is enough
					break;
				}
			}
		}

		// Collide against ground plane
		if (particle.position.y < particle.radius)
		{
			particle.position.y = particle.radius;
			particle.velocity.x *= 0.4f;
			particle.velocity.y *= -0.3f;
			particle.velocity.z *= 0.4f;
		}
	}

	return true;
}

/*
 *
 */
bool ParticleSystemCPU::SetEmitter(const Emitter &emitter)
{
	m_sEmitter = emitter;
	return true;
}

bool ParticleSystemCPU::SetParticleMesh(Mesh* particleMesh)
{
	m_ParticleMesh = particleMesh;
	return true;
}


/*
 *
 */
bool ParticleSystemCPU::SetCollisionSpheres(const Sphere *pSpheres, unsigned int uiNumSpheres)
{
	m_aSpheres.Clear();
	m_aSpheres.SetCapacity(uiNumSpheres);

	m_aSquaredRadii.Clear();
	m_aSquaredRadii.SetCapacity(uiNumSpheres);

	for (unsigned int i=0; i < uiNumSpheres; i++)
	{
		m_aSpheres.Append(pSpheres[i]);
		m_aSquaredRadii.Append(pSpheres[i].fRadius * pSpheres[i].fRadius);
	}
	return true;
}

/*
 *
 */
bool ParticleSystemCPU::SetNumberOfParticles(unsigned int numParticles)
{
	if (m_aParticles.GetSize() != numParticles)
	{
		m_aParticles.Clear();
		m_aParticles.Resize(numParticles);		
		for (unsigned int i=0; i < numParticles; i++)
		{
			m_aParticles[i].lifespan = (rand() / (float)RAND_MAX) * 20.0f;
			m_aParticles[i].radius = 0.5f;
			m_aParticles[i].pMesh = m_ParticleMesh;
		}
	}

	return true;
}
