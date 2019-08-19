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
	m_X = (rand() - rand()) * (rand() - rand()) / 100.0 ;
	m_Y = (rand() - rand()) * (rand() - rand()) / 100.0 ;
	m_Z = (rand() - rand()) * (rand() - rand()) / 100.0 ;
	m_MasseKG = 1000 + (20000 * rand());
}

// Nouvelle Position en KM = (Vitesse en M/S * in_NombreSeconde) / 1000
// On divise en 1000 car Metre en KM. StepSize in seconds.
void Mass::EffectuerPasChangementPosition(int p_iStepSize)
{
	m_X += (m_VitesseX * p_iStepSize) / 1000.0;
	m_Y += (m_VitesseY * p_iStepSize) / 1000.0;
	m_Z += (m_VitesseZ * p_iStepSize) / 1000.0;
}

//
void Mass::EffectuerPasChangementVitesse(const Universe& p_roUniverse, int p_iStepSize)	// StepSize in seconds.
{
	double dblImpactSpeedX = 0.0;
	double dblImpactSpeedY = 0.0;
	double dblImpactSpeedZ = 0.0;

	// We should change speed before position, so that way we will know we are not the same mass by position.
	for (int i = 0; i < p_roUniverse.m_arrMasses.GetSize(); i++)
	{
		const Mass& roMass = p_roUniverse.m_arrMasses.GetAt(i);

		const double dblDistance = Distance(roMass);
		if(dblDistance < DIST_MIN)	// fix the issue of same position ourself.
		{
			continue;
		}

		// F = Gm1m2/d2 (mass in kg and distance in meter, since we store in km, we must multiply by 1000 each distance)
		// F = MA
		// F/M = A
		// A x T = speed change.
		// Force divisé par notre masse pour simplifier le calcul plus bas en accélération

		const double dblForce = (G * roMass.m_MasseKG * m_MasseKG) / (dblDistance * dblDistance * 1000000);
		const double dblAcceleration = dblForce / m_MasseKG;

		const double deltaX = roMass.m_X - m_X;
		const double deltaY = roMass.m_Y - m_Y;
		const double deltaZ = roMass.m_Z - m_Z;

		// La proportion de l'accélération en X * nombre de seconde = variation de vitesse à appliquer à cette masse.
		const double variationVitesseX = (deltaX*deltaX/(dblDistance * dblDistance)) * dblAcceleration * p_iStepSize;
		const double variationVitesseY = (deltaY*deltaY/(dblDistance * dblDistance)) * dblAcceleration * p_iStepSize;
		const double variationVitesseZ = (deltaZ*deltaZ/(dblDistance * dblDistance)) * dblAcceleration * p_iStepSize;

		// The other mass is "higher" in value than us, so we want to go in its direction, we accelerate through this axis.
		if (deltaX > 0)
		{
			dblImpactSpeedX += variationVitesseX;
		}
		else
		{
			dblImpactSpeedX -= variationVitesseX;
		}

		if (deltaY > 0)
		{
			dblImpactSpeedY += variationVitesseY;
		}
		else
		{
			dblImpactSpeedY -= variationVitesseY;
		}

		if (deltaZ > 0)
		{
			dblImpactSpeedZ += variationVitesseZ;
		}
		else
		{
			dblImpactSpeedZ -= variationVitesseZ;
		}
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