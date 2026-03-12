#pragma once

//#define GETENV	Environment::GetInstance()

//#define K_PauseEntreChaquePas 10    // Millisecondes
//#define K_NombrePasParRefresh	(3600*1) // Tentative de simuler x temps par refresh

#define K_RayonUniversCreationAleatoireMasses			1e20    //KM
#define G						6.67e-11

#define MASSE_SOLEIL			1.989e30  // KG
#define K_VitesseMAX			300000000   // M/s 

#define K_NombreEtoile			15

//#define K_PasDeSimulationEnSeconde  1 // 1 seconde simul� = 1 pas.

#define	DIST_MIN				1000		// km - minimum distance to skip interaction (avoid self-interaction)
#define SOFTENING_PARAM			1000000.0 // m^2 - Plummer softening parameter to prevent numerical instability
//#define VITESSE_DEPART			1

// �chelle de vitesse est en m�tre par seconde

// �chelle de masse en KG



class Environment
{
private:
  Environment(void);
public:
	
	virtual ~Environment(void);
	static Environment* GetInstance(void);
};