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
	int iStepSize = 60*60;	// One hours

	/*Universe oOriginalUniverse(K_NombreEtoile);
	oOriginalUniverse.Randomize();
*/

	Universe oOriginalUniverse(2);

	Mass& roSun = oOriginalUniverse.m_arrMasses.GetAt(0);
	roSun.m_MasseKG = 2e30;

	Mass& roEarth = oOriginalUniverse.m_arrMasses.GetAt(1);
	roEarth.m_MasseKG = 5.98e24;
	roEarth.m_X = 146000000;	// 146 million km
	roEarth.m_VitesseY = -29720;	// m/s

	//Mass& roMoon = oOriginalUniverse.m_arrMasses.GetAt(1);
	//roMoon.m_MasseKG = 7.342e22;	//terre.m_MasseKG/81.0;
	//roMoon.m_X = K_Distance_Terre_Lune; //km
	//roMoon.m_VitesseY = 1023;	// 3683 km / h is around 1023.056 m/s

		//Mass& lune2 = oOriginalUniverse.m_arrMasses.GetAt(2);
		//lune2.m_MasseKG = terre.m_MasseKG / 81.0;
		//lune2.m_X = K_Distance_Terre_Lune * 2;
		//lune2.m_VitesseY = 377;

		//Mass* lune2 = new Mass();
		//lune2->m_MasseKG = terre->m_MasseKG / 81.0;
		//lune2->m_X = K_Distance_Terre_Lune*2; //km
		//lune2->m_VitesseY = 377; // pense que c'est proche en mètre / seconde

	oOriginalUniverse.ExportPPM();

	Universe* poCurrentUniverse = oOriginalUniverse.GenerateSimulationStep(iStepSize);
	poCurrentUniverse->ExportPPM();

	while (true)
	{
		Universe* poWhileUniverse = poCurrentUniverse->GenerateSimulationStep(iStepSize);

		if (poWhileUniverse->m_iIteration % (24) == 0)
		{
			poWhileUniverse->ExportPPM();
		}

		delete poCurrentUniverse;
		poCurrentUniverse = poWhileUniverse;
	}

	poCurrentUniverse->ExportPPM();
	delete poCurrentUniverse;
}
