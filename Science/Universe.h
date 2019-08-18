#pragma once

#include "Mass.h"

class Universe
{
private:
	Universe();

public:
	Universe(int p_iMasses);
	virtual ~Universe();

	void Universe::Randomize();
	Universe* GenerateSimulationStep(int p_iStepSize);
	Mass& GetAt(int p_iPos)										{ return m_arrMasses.GetAt(p_iPos); }
	void ExportPPM(int p_iWidth = 512, int p_iHeight = 512);
	void ExportBitmap();

	void SimulateFrom(const Universe& p_roUniverse, int p_iStepSize);

	void SaveBitmapToFile(BYTE* pBitmapBits,
		LONG lWidth,
		LONG lHeight,
		WORD wBitsPerPixel,
		const unsigned long& padding_size,
		LPCTSTR lpszFileName);

	CArray<Mass, Mass> m_arrMasses;
	int m_iIteration;
};

