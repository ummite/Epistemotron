#include "pch.h"
#include "../framework.h"

#include "Mass.h"
#include "math.h"
#include "Environment.h"
#include "Universe.h"

Mass::Mass(void)
: m_X(0.0),
m_Y(0.0),
m_Z(0.0),
m_VitesseX(0.0),
m_VitesseY(0.0),
m_VitesseZ(0.0),
m_MasseKG(0.0)
{
}

Mass::~Mass(void)
{
}

void Mass::Randomize(void)
{
	// Random position within +/- 146 million km
	m_X = (rand() / static_cast<double>(RAND_MAX) - 0.5) * 146000000.0;
	m_Y = (rand() / static_cast<double>(RAND_MAX) - 0.5) * 146000000.0;
	m_Z = (rand() / static_cast<double>(RAND_MAX) - 0.5) * 146000000.0;

	// Random velocity between -29720 and +29720 m/s
	m_VitesseX = (rand() / static_cast<double>(RAND_MAX) - 0.5) * 29720.0;
	m_VitesseY = (rand() / static_cast<double>(RAND_MAX) - 0.5) * 29720.0;
	m_VitesseZ = (rand() / static_cast<double>(RAND_MAX) - 0.5) * 29720.0;

	// Earth mass as default
	m_MasseKG = 5.98e24;
}

// Nouvelle Position en KM = (Vitesse en M/S * in_NombreSeconde) / 1000
// On divise en 1000 car Metre en KM. StepSize in seconds.
void Mass::EffectuerPasChangementPosition(int p_iStepSize)
{
	m_X += (m_VitesseX * p_iStepSize) / 1000.0;
	m_Y += (m_VitesseY * p_iStepSize) / 1000.0;
	m_Z += (m_VitesseZ * p_iStepSize) / 1000.0;
}

// Update velocity based on gravitational interactions with all other masses
void Mass::EffectuerPasChangementVitesse(const Universe& p_roUniverse, int p_iStepSize)
{
	double dblImpactSpeedX = 0.0;
	double dblImpactSpeedY = 0.0;
	double dblImpactSpeedZ = 0.0;

	for (int i = 0; i < p_roUniverse.m_arrMasses.GetSize(); i++)
	{
		const Mass& roMass = p_roUniverse.m_arrMasses.GetAt(i);

		const double deltaX = roMass.m_X - m_X;
		const double deltaY = roMass.m_Y - m_Y;
		const double deltaZ = roMass.m_Z - m_Z;

		const double dblDistance = sqrt(deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ);

		if (dblDistance < DIST_MIN)
		{
			continue;	// Skip if too close (e.g., same mass)
		}

		// F = G*m1*m2/d^2 (distance in meters, so multiply km by 1000)
		// a = F/m = G*m_other/d^2
		// Using Plummer softening: replace d^2 with (d^2 + epsilon^2) to prevent
		// numerical instability when bodies get very close
		const double dblDistanceMeters = dblDistance * 1000.0;
		const double dblDistanceSquared = dblDistanceMeters * dblDistanceMeters + SOFTENING_PARAM;
		const double dblAcceleration = G * roMass.m_MasseKG / dblDistanceSquared;

		// Direction cosines (normalized vector toward other mass)
		const double dirX = deltaX / dblDistance;
		const double dirY = deltaY / dblDistance;
		const double dirZ = deltaZ / dblDistance;

		// Velocity change = acceleration * direction * time step
		dblImpactSpeedX += dirX * dblAcceleration * p_iStepSize;
		dblImpactSpeedY += dirY * dblAcceleration * p_iStepSize;
		dblImpactSpeedZ += dirZ * dblAcceleration * p_iStepSize;
	}

	m_VitesseX += dblImpactSpeedX;
	m_VitesseY += dblImpactSpeedY;
	m_VitesseZ += dblImpactSpeedZ;
}

// sqrt( deltax carre + deltay carre + deltaz carre) en KM
double Mass::Distance(const Mass& p_roMass) const
{
	const double dx = p_roMass.m_X - m_X;
	const double dy = p_roMass.m_Y - m_Y;
	const double dz = p_roMass.m_Z - m_Z;

	return sqrt( dx*dx + dy*dy + dz*dz );
}


CString Mass::Trace(const Universe& p_roUniverse)
{
	CString strTrace;
	strTrace.Format(_T("Iteration : %d, x:%f y:%f  Speed: X:%f Y:%f\n"), p_roUniverse.m_iIteration,  m_X, m_Y, m_VitesseX, m_VitesseY);
	return strTrace;
}