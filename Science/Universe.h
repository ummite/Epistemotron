#pragma once

#include "Mass.h"

// ScenarioType enum is now defined in framework.h (via pch.h) for global visibility
// This ensures consistent use across all modules (EpistemotronView, Simulator, etc.)

class Universe
{
private:
	CArray<Mass, Mass> m_arrMasses;

	// Private helper methods for scenario loading
	void LoadSolarSystem();
	void LoadBinaryStar();
	void LoadThreeBody();
	void LoadGalaxy();

public:
	Universe();  // Default constructor
	Universe(int p_iMasses);
	virtual ~Universe();

	void Randomize();

	// Load preset scenario configurations
	void LoadScenario(ScenarioType p_scenarioType);

	Universe* GenerateSimulationStep(int p_iStepSize);
	Mass& GetAt(int p_iPos) { return m_arrMasses.GetAt(p_iPos); }
	const Mass& GetAt(int p_iPos) const { return m_arrMasses.GetAt(p_iPos); }
	int GetMassCount() const { return static_cast<int>(m_arrMasses.GetSize()); }
	void ExportPPM(int p_iWidth = 800, int p_iHeight = 600);

	// Optimized symplectic Euler integration for better energy conservation
	void SimulateFrom(const Universe& p_roUniverse, int p_iStepSize);

	// Velocity Verlet integration (even better energy conservation)
	// More accurate than symplectic Euler for oscillatory systems like orbits
	void SimulateFromVelocityVerlet(const Universe& p_roUniverse, int p_iStepSize);

	// Energy conservation statistics
	double GetTotalKineticEnergy() const;      // Sum of all KE (joules)
	double GetTotalPotentialEnergy() const;    // Sum of all pairwise PE (joules)
	double GetTotalEnergy() const;             // KE + PE (joules)

	// Linear momentum conservation
	void GetTotalLinearMomentum(double& out_px, double& out_py, double& out_pz) const;
	double GetTotalLinearMomentumMagnitude() const;  // |P| in kg*m/s

	// Angular momentum conservation
	void GetTotalAngularMomentum(double& out_Lx, double& out_Ly, double& out_Lz) const;
	double GetTotalAngularMomentumMagnitude() const;  // |L| in kg*m^2/s

	void SaveBitmapToFile(BYTE* pBitmapBits,
		LONG lWidth,
		LONG lHeight,
		WORD wBitsPerPixel,
		const unsigned long& padding_size,
		LPCTSTR lpszFileName);

	// Iteration counter (public for simulation control)
	int m_iIteration;
};

