#include "pch.h"

#include "Simulator.h"
#include "Mass.h"
#include "Environment.h"
#include "Universe.h"

#include "../EpistemotronView.h"

Simulator::Simulator()
{
}

Simulator::~Simulator()
{
}

void Simulator::Test(CEpistemotronView* ce)
{
	int iStepSize = 60*60*24;	// One day

	//Universe oOriginalUniverse(3);

	//{
	//	Mass& roSun = oOriginalUniverse.m_arrMasses.GetAt(0);
	//	roSun.m_MasseKG = 2e30;

	//	Mass& roEarth = oOriginalUniverse.m_arrMasses.GetAt(1);
	//	roEarth.m_MasseKG = 5.98e24;
	//	roEarth.m_X = 146000000;	// 146 million km
	//	roEarth.m_VitesseY = 29720;	// m/s

	//	Mass& roMoon = oOriginalUniverse.m_arrMasses.GetAt(2);
	//	roMoon.m_MasseKG = 7.342e22;	//terre.m_MasseKG/810;
	//	roMoon.m_X = roEarth.m_X + 384400;
	//	//The Moon orbits Earth at a speed of (3,683 kilometers per hour) = 
	//	roMoon.m_VitesseY = roEarth.m_VitesseY + 1023;
	//}

	Universe oOriginalUniverse(4);

	{
		Mass& roSun = oOriginalUniverse.m_arrMasses.GetAt(0);
		roSun.m_MasseKG = 2e30;

		Mass& roEarth = oOriginalUniverse.m_arrMasses.GetAt(1);
		roEarth.m_MasseKG = 5.98e24;
		roEarth.m_X = 146000000;	// 146 million km
		roEarth.m_VitesseY = 29720;	// m/s

		Mass& roMoon = oOriginalUniverse.m_arrMasses.GetAt(2);
		roMoon.m_MasseKG = 7.342e22;	//terre.m_MasseKG/810;
		roMoon.m_X = roEarth.m_X + 384400;
		//The Moon orbits Earth at a speed of (3,683 kilometers per hour) = 
		roMoon.m_VitesseY = roEarth.m_VitesseY + 1023;

		Mass& roMoon2 = oOriginalUniverse.m_arrMasses.GetAt(3);
		roMoon2.m_MasseKG = 7.342e22;	//terre.m_MasseKG/810;
		roMoon2.m_X = roEarth.m_X + 3844000;
		//The Moon orbits Earth at a speed of (3,683 kilometers per hour) = 
		roMoon2.m_VitesseY = roEarth.m_VitesseY + 1023;

		//Mass& roMoon3 = oOriginalUniverse.m_arrMasses.GetAt(4);
		//roMoon.m_MasseKG = 7.342e22;	//terre.m_MasseKG/810;
		//roMoon.m_X = roEarth.m_X - 3844000;
		////The Moon orbits Earth at a speed of (3,683 kilometers per hour) = 
		//roMoon.m_VitesseY = roEarth.m_VitesseY + 1023;
	}

	oOriginalUniverse.ExportPPM();

	Universe* poCurrentUniverse = oOriginalUniverse.GenerateSimulationStep(iStepSize);
	poCurrentUniverse->ExportPPM();

	while (true)
	{
		Universe* poWhileUniverse = poCurrentUniverse->GenerateSimulationStep(iStepSize);

		if (poWhileUniverse->m_iIteration % (1) == 0)	// Each Day
		{
			poWhileUniverse->ExportPPM();
		}

		delete poCurrentUniverse;
		poCurrentUniverse = poWhileUniverse;
	}

	poCurrentUniverse->ExportPPM();
	delete poCurrentUniverse;
}
