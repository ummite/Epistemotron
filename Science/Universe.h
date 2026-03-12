#pragma once

#include "Mass.h"

class Universe
{
private:
	CArray<Mass, Mass> m_arrMasses;

public:
	Universe();  // Default constructor
	Universe(int p_iMasses);
	virtual ~Universe();

	void Randomize();
	Universe* GenerateSimulationStep(int p_iStepSize);
	Mass& GetAt(int p_iPos) { return m_arrMasses.GetAt(p_iPos); }
	const Mass& GetAt(int p_iPos) const { return m_arrMasses.GetAt(p_iPos); }
	int GetMassCount() const { return m_arrMasses.GetSize(); }
	void ExportPPM(int p_iWidth = 800, int p_iHeight = 600);

	// Optimized simulation using Newton's 3rd law to halve computations
	void SimulateFrom(const Universe& p_roUniverse, int p_iStepSize);

	// Compute accelerations for all bodies (optimized O(n²/2) version)
	void ComputeAccelerations(const Universe& p_roUniverse, double* accelArray);

	void SaveBitmapToFile(BYTE* pBitmapBits,
		LONG lWidth,
		LONG lHeight,
		WORD wBitsPerPixel,
		const unsigned long& padding_size,
		LPCTSTR lpszFileName);

	// Iteration counter (public for simulation control)
	int m_iIteration;
};

