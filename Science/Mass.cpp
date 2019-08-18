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
// On divise en 1000 car Metre en KM.
// StepSize in seconds.
void Mass::EffectuerPasChangementPosition(int p_iStepSize)
{
	//TRACE(_T("Position X before: %f "), m_X);
	m_X += (m_VitesseX * p_iStepSize) / 1000.0;
	//TRACE(_T("after: %f\n"), m_X);

	//TRACE(_T("Position Y before: %f "), m_Y);
	m_Y += (m_VitesseY * p_iStepSize) / 1000.0;
	//TRACE(_T("after: %f\n"), m_Y);

	m_Z += (m_VitesseZ * p_iStepSize) / 1000.0;
}

//
void Mass::EffectuerPasChangementVitesse(const Universe& p_roUniverse, int p_iStepSize)	// StepSize in seconds.
{
	// Redo in  from here
	//int iMassImpact = 0;	// TODO later

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
		
		//
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

	// End Redo in  from here ************************************************************

//  // On additionne le précalcul des variation de position qui ont été calculés précédemment, car les changement de positions ne doivent pas être fait en cours de calcul
//  // des forces car si on change la position on altères les forces sur les autres masses en cours de traitement d'un pas.
//
//  // WARNING
//
//  // J'ai laissé tombé le précalcul pour l'instant.  On peut changer la position et vitesse dans en cours de calcul de pas. On va légèrement altérer
//  // la qualité du calcul mais considérant qu'on a un pas suffisamment petit, c'est négligeable.
//  // Éventuellement il faudrait faire les "pas" en 2 temps : calcul de la variation de vitesse pour tous .... reviser note car maybe ok la.
//
//	//m_X += dxProchain;
//	//m_Y += dyProchain;
//	//m_Z += dzProchain;
//
//	//dxProchain = m_VitesseX;
//	//dyProchain = m_VitesseY;
//	//dzProchain = m_VitesseZ;
//
//  // Les whiles suivants sont SEULEMENT pour faire du pacman lorsque l'on passe d'un côté à l'autre et ne devront pas être la en version finale.
//	/*while( m_X < 0 )
//	{
//		m_X += (double)K_RayonUnivers;
//	}
//	while( m_Y < 0 )
//	{
//		m_Y += (double)K_RayonUnivers;
//	}
//	while( m_Z < 0 )
//	{
//		m_Z += (double)K_RayonUnivers;
//	}
//
//	while( m_X > (double)K_RayonUnivers )
//	{
//		m_X -= K_RayonUnivers;
//	}
//	while( m_Y > (double)K_RayonUnivers )
//	{
//		m_Y -= (double)K_RayonUnivers;
//	}
//	while( m_Z > (double)K_RayonUnivers )
//	{
//		m_Z -= (double)K_RayonUnivers;
//	}*/
//
//  const int nbMasses = ms_Etoiles.GetSize();
// 
//	for( int i = 0 ; i < nbMasses ; i++ )
//	{
//		Mass* etoileDistante = ms_Etoiles.GetAt(i);
//    if( etoileDistante == this)
//    {
//      continue;
//    }
//    if (etoileDistante->m_MasseKG <= 0.0)
//    {
//      continue; // Optimisation pour masse nulle, servant de point repères.
//    }
//
//		const double distance = CalculerDistance( *etoileDistante );
//		if( distance < DIST_MIN )
//		{
//			continue;
//		}
//
//		// F = Gm1m2/d2
//    // F = MA -> Calcul de l'accélération résultante.
//
//		// Force divisé par notre masse pour simplifier le calcul plus bas en accélération
//
//    const double force = (G * etoileDistante->m_MasseKG * m_MasseKG) / (distance*distance*1000000);
//    const double acceleration = force / m_MasseKG;
//
//    // Simplification de force et acceleration ensemble.
//    // Optimisation tout à fait possible partout.
//    // Multiplie par 1 million car nous devons travailler en mètre.
//		//const double acceleration = (G * etoileDistante->m_MasseKG) / (distance*distance*1000000);
//
//		const double deltaX = etoileDistante->m_X - m_X;
//		const double deltaY = etoileDistante->m_Y - m_Y;
//    const double deltaZ = etoileDistante->m_Z - m_Z;
//
//    // La proportion de l'accélération en X * nombre de seconde = variation de vitesse à appliquer à cette masse.
//    const double variationVitesseX = (deltaX*deltaX/(distance*distance)) * acceleration * in_NombreSeconde;
//    const double variationVitesseY = (deltaY*deltaY/(distance*distance)) * acceleration * in_NombreSeconde;
//    const double variationVitesseZ = (deltaZ*deltaZ/(distance*distance)) * acceleration * in_NombreSeconde;
//
//
//    const double vitesseAvantX = m_VitesseX;
//
//    if( deltaX < 0 )
//    {
//      m_VitesseX -= variationVitesseX;
//    }
//    else
//    {
//      m_VitesseX += variationVitesseX;
//    }    
//
//    if (deltaY < 0)
//    {
//      m_VitesseY -= variationVitesseY;
//    }
//    else
//    {
//      m_VitesseY += variationVitesseY;
//    }
//
//    if (deltaZ < 0)
//    {
//      m_VitesseZ -= variationVitesseZ;
//    }
//    else
//    {
//      m_VitesseZ += variationVitesseZ;
//    }
//
//  /*    // La force est répartie sur la vitesse de cette façon:
//
//		     /|
//  5 (force) / | 4 (dy)
//		   /	| 
//		  /---|
//		   3(dx)
//
//		   dx et dy correspondent à la difference entre x et y des 2 etoiles.
//		   Si la force résultante a été calculée comme étant 100, alors la force sur y
//		   sera de 80, et la force sur x sera de 60, car les proportions sur le vecteur référence
//		   sont conservées.
//  */
//	  }
//  return;
//	if( m_VitesseX > K_VitesseMAX )
//	{
//		m_VitesseX = K_VitesseMAX;
//	}
//	if( m_VitesseY > K_VitesseMAX )
//	{
//		m_VitesseY = K_VitesseMAX;
//	}
//
//	if( m_VitesseX < -K_VitesseMAX )
//	{
//		m_VitesseX = -K_VitesseMAX;
//	}
//	if( m_VitesseY < -K_VitesseMAX )
//	{
//		m_VitesseY = -K_VitesseMAX;
//	}
}

//void Mass::InitialiseMonde(int qteeEtoile)
//{
//  const double K_Distance_Terre_Lune = 384403.0;  //KM
//  // 2 en rotations
//  {
//    Mass* terre = new Mass();
//    terre->m_MasseKG = 5.9736e24;
//    terre->m_VitesseY = -10;
//    ms_Etoiles.Add( terre );
//      
//    Mass* lune = new Mass();
//    lune->m_MasseKG = terre->m_MasseKG/81.0;
//    lune->m_X = K_Distance_Terre_Lune; //km
//    lune->m_VitesseY = 877; // pense que c'est proche en mètre / seconde
//
//    ms_Etoiles.Add( lune );
//
//
//	Mass* lune2 = new Mass();
//	lune2->m_MasseKG = terre->m_MasseKG / 81.0;
//	lune2->m_X = K_Distance_Terre_Lune*2; //km
//	lune2->m_VitesseY = 377; // pense que c'est proche en mètre / seconde
//
//	ms_Etoiles.Add(lune2);
//
//  }
//
//  const int K_GrandeDistance = 10.0 * K_Distance_Terre_Lune;
//  
//  Mass* bidon = new Mass();
//	bidon->m_MasseKG = 0;
//	bidon->m_X = K_GrandeDistance;
//	bidon->m_Y = K_GrandeDistance;
//  bidon->m_Z = 0.0;
//	bidon->m_VitesseX = 0;
//	bidon->m_VitesseY = 0;	  // Terre : 29,79 km/s -> 297900.0 m/s
//  bidon->m_VitesseZ = 0;
//	ms_Etoiles.Add( bidon );
//
//  Mass* bidon2 = new Mass();
//	bidon2->m_MasseKG = 0;
//	bidon2->m_X = K_GrandeDistance;
//	bidon2->m_Y = -K_GrandeDistance;
//  bidon2->m_Z = 0.0;
//	bidon2->m_VitesseX = 0;
//	bidon2->m_VitesseY = 0;	  // Terre : 29,79 km/s -> 297900.0 m/s
//  bidon2->m_VitesseZ = 0;
//	ms_Etoiles.Add( bidon2 );
//
//
//  Mass* bidon3 = new Mass();
//	bidon3->m_MasseKG = 0;
//	bidon3->m_X = -K_GrandeDistance;
//	bidon3->m_Y = K_GrandeDistance;
//  bidon3->m_Z = 0.0;
//	bidon3->m_VitesseX = 0;
//	bidon3->m_VitesseY = 0;	  // Terre : 29,79 km/s -> 297900.0 m/s
//  bidon3->m_VitesseZ = 0;
//	ms_Etoiles.Add( bidon3 );
//
//  Mass* bidon4 = new Mass();
//	bidon4->m_MasseKG = 0;
//	bidon4->m_X = -K_GrandeDistance;
//	bidon4->m_Y = -K_GrandeDistance;
//  bidon4->m_Z = 0.0;
//	bidon4->m_VitesseX = 0;
//	bidon4->m_VitesseY = 0;	  // Terre : 29,79 km/s -> 297900.0 m/s
//  bidon4->m_VitesseZ = 0;
//	ms_Etoiles.Add( bidon4 );
//
//
//return;
//  for(int i = 0 ; i < 10 ; i++ )
//  {
//    Mass* planeteRandom = new Mass();
//    planeteRandom->Randomize();
//    ms_Etoiles.Add( planeteRandom );
//  }
//
//
//  return;
//
//	Mass* soleil = new Mass();
//	soleil->m_MasseKG = 1.989e30;
//	ms_Etoiles.Add( soleil );
//
//  Mass* pluton = new Mass();
//	pluton->m_MasseKG = 1.29e22;
//	pluton->m_X = 5.913e9;
//	pluton->m_Y = 5.913e9;
//  pluton->m_Z = 0.0;
//	pluton->m_VitesseX = 500.0;	  // m/s
//	pluton->m_VitesseY = 0.0;
//  pluton->m_VitesseZ = 0.0;
//	ms_Etoiles.Add( pluton );
//  
//
//  Mass* terre = new Mass();
//	terre->m_MasseKG = 5.9736e24;
//	terre->m_X = 149597870.691;
//	terre->m_Y = 0.0; // distance trouvée
//  terre->m_Z = 0.0;
//	terre->m_VitesseX = 0.0;
//	terre->m_VitesseY = 297900.0;	  // Terre : 29,79 km/s -> 297900.0 m/s
//  terre->m_VitesseZ = 0.0;
//	ms_Etoiles.Add( terre );
//
//

	/*for( int i = 0 ; i < qteeEtoile ; i++ )
	{
		ms_Etoiles.Add( new Mass() );
	}*/
/*
	Mass* uneEtoile = new Mass();
	uneEtoile->m_Masse = 1000000;
	uneEtoile->m_X = K_TailleUnivers / 4;
	uneEtoile->m_Y = K_TailleUnivers / 4;
	uneEtoile->m_VitesseX = 110;
	uneEtoile->m_VitesseY = 110;
	ms_Etoiles.Add( uneEtoile );

	uneEtoile = new Mass();
	uneEtoile->m_Masse = 100000;
	uneEtoile->m_X = K_TailleUnivers / 4;
	uneEtoile->m_Y = K_TailleUnivers / 3;
	uneEtoile->m_VitesseX = 1000000;
	uneEtoile->m_VitesseY = 0;
	ms_Etoiles.Add( uneEtoile );
*/

	/*Mass* uneEtoile = new Mass();
	uneEtoile->m_Masse = 1000000;
	uneEtoile->m_X = K_TailleUnivers / 4;
	uneEtoile->m_Y = K_TailleUnivers / 4;
	uneEtoile->m_VitesseX = 110;
	uneEtoile->m_VitesseY = 110;
	ms_Etoiles.Add( uneEtoile );*/
//
//}

// sqrt( deltax carre + deltay carre + deltaz carre) en KM
double Mass::Distance(const Mass& p_roMass) const
{
	const double dx = p_roMass.m_X - m_X;
	const double dy = p_roMass.m_Y - m_Y;
	const double dz = p_roMass.m_Z - m_Z;

	return sqrt( dx*dx + dy*dy + dz*dz );
}

//void Mass::ObtenirDimensionUniversXYZ(double& out_xMin, double& out_xMax, double& out_yMin, double& out_yMax, double& out_zMin, double& out_zMax)
//{
//  Mass* etoileReference = ms_Etoiles.GetAt(0);
//  if (NULL == etoileReference)
//  {
//    return;
//  }
// 
//  out_xMin = out_xMax = etoileReference->m_X;
//  out_yMin = out_yMax = etoileReference->m_Y;
//  out_zMin = out_zMax = etoileReference->m_Z;
//
//  for( int i = 1 ; i < ms_Etoiles.GetSize() ; i++ )
//	{
//		Mass* etoileDistante = ms_Etoiles.GetAt(i);
//
//    if (etoileDistante->m_X < out_xMin)
//    {
//      out_xMin = etoileDistante->m_X;
//    }
//
//    if (etoileDistante->m_X > out_xMax)
//    {
//      out_xMax = etoileDistante->m_X;
//    }
//
//    if (etoileDistante->m_Y < out_yMin)
//    {
//      out_yMin = etoileDistante->m_Y;
//    }
//
//    if (etoileDistante->m_Y > out_yMax)
//    {
//      out_yMax = etoileDistante->m_Y;
//    }
//
//    if (etoileDistante->m_Z < out_zMin)
//    {
//      out_zMin = etoileDistante->m_Z;
//    }
//
//    if (etoileDistante->m_Z > out_zMax)
//    {
//      out_zMax = etoileDistante->m_Z;
//    }
//  }
//}
//
//CString Mass::ObtainTextDetails(void)
//{
//  CString retour;
//
//  //FULL
//  //retour.Format("x:%e y:%e z:%e vx:%e vy:%e vz:%e m:%eKG ", m_X, m_Y, m_Z, m_VitesseX, m_VitesseY, m_VitesseZ, m_MasseKG);
//  
//  //POS X,Y Only
//  retour.Format(L"x:%e y:%e ", m_X, m_Y);
//  return retour;
//}

CString Mass::Trace(const Universe& p_roUniverse)
{
	CString strTrace;
	strTrace.Format(_T("Iteration : %d, x:%f y:%f  Speed: X:%f Y:%f\n"), p_roUniverse.m_iIteration,  m_X, m_Y, m_VitesseX, m_VitesseY);
	return strTrace;
}