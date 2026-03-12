#include "pch.h"

#include "Simulator.h"
#include "Mass.h"
#include "Environment.h"
#include "Universe.h"

#include "../EpistemotronView.h"

// Simulation parameters
const int SECONDS_PER_DAY = 60 * 60 * 24;
const int MAX_ITERATIONS = 365 * 10;  // 10 years of simulation
const int EXPORT_INTERVAL = 365;  // Export every year

Simulator::Simulator()
{
}

Simulator::~Simulator()
{
}

void Simulator::Test(CEpistemotronView* ce)
{
	int iStepSize = SECONDS_PER_DAY;

	Universe oOriginalUniverse(4);

	{
		Mass& roSun = oOriginalUniverse.m_arrMasses.GetAt(0);
		roSun.m_MasseKG = 2e30;

		Mass& roEarth = oOriginalUniverse.m_arrMasses.GetAt(1);
		roEarth.m_MasseKG = 5.98e24;
		roEarth.m_X = 146000000;	// 146 million km
		roEarth.m_VitesseY = 29720;	// m/s

		Mass& roMoon = oOriginalUniverse.m_arrMasses.GetAt(2);
		roMoon.m_MasseKG = 7.342e22;
		roMoon.m_X = roEarth.m_X + 384400;
		roMoon.m_VitesseY = roEarth.m_VitesseY + 1023;

		Mass& roMoon2 = oOriginalUniverse.m_arrMasses.GetAt(3);
		roMoon2.m_MasseKG = 7.342e22;
		roMoon2.m_X = roEarth.m_X + 3844000;
		roMoon2.m_VitesseY = roEarth.m_VitesseY + 1023;
	}

	oOriginalUniverse.ExportPPM();

	Universe* poCurrentUniverse = oOriginalUniverse.GenerateSimulationStep(iStepSize);
	poCurrentUniverse->ExportPPM();

	while (poCurrentUniverse->m_iIteration < MAX_ITERATIONS)
	{
		Universe* poNextUniverse = poCurrentUniverse->GenerateSimulationStep(iStepSize);

		if (poNextUniverse->m_iIteration % EXPORT_INTERVAL == 0)
		{
			poNextUniverse->ExportPPM();
		}

		delete poCurrentUniverse;
		poCurrentUniverse = poNextUniverse;
	}

	poCurrentUniverse->ExportPPM();
	delete poCurrentUniverse;
}
