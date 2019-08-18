#pragma once

class Universe;

class Mass
{
public:
	Mass(void);
	virtual ~Mass(void);

  // La position est en KM par rapport à un "zéro".
	double m_X; // Position in KM, so double value will be a little smaller.
	double m_Y; // Position in KM, so double value will be a little smaller.
	double m_Z; // Position in KM, so double value will be a little smaller.

	double m_VitesseX;  // Speed in meter per second
	double m_VitesseY;  // Speed in meter per second
	double m_VitesseZ;  // Speed in meter per second

	double m_MasseKG;     // Masse en KG.

	void Randomize(void);

	double Distance(const Mass& p_roMass) const;
	
	//static void ObtenirDimensionUniversXYZ(double& out_xMin, double& out_xMax, double& out_yMin, double& out_yMax, double& out_zMin, double& out_zMax);
	void EffectuerPasChangementPosition(int p_iStepSize);	// In seconds
	void EffectuerPasChangementVitesse(const Universe& p_roUniverse, int p_iStepSize);	// In seconds

	CString Trace(const Universe& p_roUniverse);

};
