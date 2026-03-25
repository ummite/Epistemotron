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

	// Custom scenario support
	void Clear();  // Clear all bodies
	void AddBody(const Mass& body);  // Add a body to the universe
	/// Export universe visualization as BMP file
	/// @param p_iWidth Image width in pixels (default: 800)
	/// @param p_iHeight Image height in pixels (default: 600)
	/// @param p_path Output file path (default: L"c:\\temp\\t.bmp")
	void ExportPPM(int p_iWidth = 800, int p_iHeight = 600, const wchar_t* p_path = nullptr);

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

	// Collision counter - tracks total collisions since universe creation
	int m_totalCollisions;

	// Largest collision mass (for statistics)
	double m_largestCollisionMass;

	// Collision processing toggle (set from View)
	BOOL m_bEnableCollisions;

	// Last collision positions for visual feedback (cleared after reading)
	struct CollisionEvent
	{
		double x, y, z;  // Position in km
		COLORREF color;  // Suggested color based on mass
	};
	std::vector<CollisionEvent> m_lastCollisions;

	// ============================================================================
	// Collision Detection and Handling
	// ============================================================================

	/// Process collisions between all bodies and merge colliding pairs
	/// Uses conservation of mass and momentum to compute merged body properties
	/// @param enableCollisions If false, skip collision processing
	/// @return Number of collisions that occurred in this step
	int ProcessCollisionsSimple(BOOL enableCollisions = TRUE);

	/// Get total collision count
	/// @return Total number of collisions that have occurred
	int GetTotalCollisions() const { return m_totalCollisions; }

	/// Get and clear last collision events for visual feedback
	/// @return Vector of collision events (positions and colors)
	std::vector<CollisionEvent> ConsumeLastCollisions();

	/// Get the largest collision mass
	/// @return Mass in kg of the largest collision
	double GetLargestCollisionMass() const { return m_largestCollisionMass; }

	// ============================================================================
	// Save/Load Simulation State
	// ============================================================================

	/// Save universe state to a file
	/// @param filename Path to save file
	/// @return TRUE on success, FALSE on failure
	BOOL SaveState(LPCTSTR filename);

	/// Load universe state from a file
	/// @param filename Path to load file
	/// @return TRUE on success, FALSE on failure
	BOOL LoadState(LPCTSTR filename);
};

