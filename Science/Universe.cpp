#include "pch.h"

#include "Universe.h"
#include "Mass.h"
#include "Environment.h"


Universe::Universe()
{
}

Universe::Universe(int m_iMasses) :
	m_iIteration(0)
{
	m_arrMasses.SetSize(m_iMasses);
}

Universe::~Universe()
{
}

Universe* Universe::GenerateSimulationStep(int p_iStepSize)
{
	Universe* poUniverse = new Universe(m_arrMasses.GetSize());
	poUniverse->m_iIteration = m_iIteration + 1;
	poUniverse->m_arrMasses.Copy(m_arrMasses);
	poUniverse->SimulateFrom(*this, p_iStepSize);
	return poUniverse;
}

void Universe::Randomize()
{
	for (int i = 0; i < m_arrMasses.GetSize(); i++)
	{
		m_arrMasses.GetAt(i).Randomize();
	}
}

// https://www.bing.com/search?q=ppm+file+format&form=EDGEAR&qs=PF&cvid=598417671ea14ed98145ab6b673f64af&cc=CA&setlang=en-US&DAF0=1
void Universe::ExportPPM(int p_iWidth, int p_iHeight)
{
	// We will take mass at position 0 as the center of the universe.

	double dblMaxMass = 0;

	const Mass& roUniverseCenter = m_arrMasses.GetAt(0);

	double dblDistanceMax = 1;
	for (int i = 1; i < m_arrMasses.GetSize(); i++)
	{
		double dblNewDistance = roUniverseCenter.Distance(m_arrMasses.GetAt(i));
		if (dblNewDistance > dblDistanceMax)
		{
			dblDistanceMax = dblNewDistance;
		}
	}

	/*
P2
# Shows the word "FEEP" (example from Netpbm man page on PGM)
24 7
15
0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
0  3  3  3  3  0  0  7  7  7  7  0  0 11 11 11 11  0  0 15 15 15 15  0
0  3  0  0  0  0  0  7  0  0  0  0  0 11  0  0  0  0  0 15  0  0 15  0
0  3  3  3  0  0  0  7  7  7  0  0  0 11 11 11  0  0  0 15 15 15 15  0
0  3  0  0  0  0  0  7  0  0  0  0  0 11  0  0  0  0  0 15  0  0  0  0
0  3  0  0  0  0  0  7  7  7  7  0  0 11 11 11 11  0  0 15  0  0  0  0
0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
*/
	const int iMassStep = 16;

	static int s_iFileIteration = 0;

	//CStringA strFilename1;
	//strFilename1.Format("Images\\Export%.5d.pgm", s_iFileIteration++);
	//CFile oFile1(CString(strFilename1), CFile::modeCreate | CFile::modeReadWrite);

	//CStringA strHeaderFormat;
	//strHeaderFormat.Format(("P2 %d %d %d \n"), p_iWidth, p_iHeight, iMassStep - 1);
	
	//oFile1.Write(strHeaderFormat, strHeaderFormat.GetLength());

	// Knowing min x and max x, divided by 1024 will represent the x step
	// Knowing min y and max y, divided by 768 will represent the y step

	//double dblXStep = (dblMaxX - dblMinX) / p_iWidth;
	//double dblYStep = (dblMaxY - dblMinY) / p_iHeight;

	double dblZoom = 1.4142;

	double dblXStep = (dblZoom * 2.0 * dblDistanceMax) / p_iWidth;
	double dblYStep = (dblZoom * 2.0 * dblDistanceMax) / p_iHeight;
	double dblMinX = roUniverseCenter.m_X - (dblXStep * p_iWidth / 2);
	double dblMaxX = roUniverseCenter.m_X + (dblXStep * p_iWidth / 2);
	double dblMinY = roUniverseCenter.m_Y - (dblYStep * p_iHeight / 2);
	double dblMaxY = roUniverseCenter.m_Y + (dblYStep * p_iHeight / 2);

	double dblMassStep = dblMaxMass / iMassStep;

	int* poMatrix = new int[p_iWidth * p_iHeight * 2] {};

	for (int i = 0; i < m_arrMasses.GetSize(); i++)
	{
		const Mass& roMass = m_arrMasses.GetAt(i);
		int x = max(0, ((int)(roMass.m_X - dblMinX) / dblXStep) - 1);
		int y = max(0, ((int)(roMass.m_Y - dblMinY) / dblYStep) - 1);
		poMatrix[x + (y * p_iWidth)] += 15; //1 + (int)(roMass.m_MasseKG / dblMassStep);
	}

	BYTE* buf = new BYTE[3 * p_iWidth * p_iHeight * 2] {};
	int c = 0;

	for (int y = 0; y < p_iHeight; y++)
	{
		for (int x = 0; x < p_iWidth; x++)
		{
			int iValue = min(poMatrix[x + (y * p_iWidth)], 15);

			if (iValue > 0)
			{
				iValue = 255;
			}
			buf[c + 0] = (BYTE)iValue;
			buf[c + 1] = (BYTE)iValue;
			buf[c + 2] = (BYTE)iValue;
			c += 3;
		}
	}

	CString test = _T("c:\\temp\\t.bmp");
	SaveBitmapToFile((BYTE*)buf,
		p_iWidth,
		p_iHeight,
		24,
		0,
		test);
	delete[] buf;
	delete[] poMatrix;
}

void Universe::SimulateFrom(const Universe& p_roUniverse, int p_iStepSize)
{
	// We must change speed before position, so we will know how to not interfere with ourself.  May be optimized by passing the mass 'ordering' or by using that loop in universe instead.
	// We also can optimize to do both acceleration "from" and "to" by only analyzing mass "higher" than us in array order, optimization todo.
	for (int i = 0; i < m_arrMasses.GetSize(); i++)
	{
		Mass& roMass = m_arrMasses.GetAt(i);
		roMass.EffectuerPasChangementVitesse(p_roUniverse, p_iStepSize);
	}
	for (int i = 0; i < m_arrMasses.GetSize(); i++)
	{
		Mass& roMass = m_arrMasses.GetAt(i);
		roMass.EffectuerPasChangementPosition(p_iStepSize);
	}
}

void Universe::SaveBitmapToFile(BYTE* pBitmapBits,
	LONG lWidth,
	LONG lHeight,
	WORD wBitsPerPixel,
	const unsigned long& padding_size,
	LPCTSTR lpszFileName)
{
	// Some basic bitmap parameters  
	unsigned long headers_size = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER);

	unsigned long pixel_data_size = lHeight * ((lWidth * (wBitsPerPixel / 8)) + padding_size);

	BITMAPINFOHEADER bmpInfoHeader = { 0 };

	// Set the size  
	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);

	// Bit count  
	bmpInfoHeader.biBitCount = wBitsPerPixel;

	// Use all colors  
	bmpInfoHeader.biClrImportant = 0;

	// Use as many colors according to bits per pixel  
	bmpInfoHeader.biClrUsed = 0;

	// Store as un Compressed  
	bmpInfoHeader.biCompression = BI_RGB;

	// Set the height in pixels  
	bmpInfoHeader.biHeight = lHeight;

	// Width of the Image in pixels  
	bmpInfoHeader.biWidth = lWidth;

	// Default number of planes  
	bmpInfoHeader.biPlanes = 1;

	// Calculate the image size in bytes  
	bmpInfoHeader.biSizeImage = pixel_data_size;

	BITMAPFILEHEADER bfh = { 0 };

	// This value should be values of BM letters i.e 0x4D42  
	// 0x4D = M 0×42 = B storing in reverse order to match with endian  
	bfh.bfType = 0x4D42;
	//bfh.bfType = 'B'+('M' << 8); 

	// <<8 used to shift ‘M’ to end  */  

	// Offset to the RGBQUAD
	bfh.bfOffBits = headers_size;

	// Total size of image including size of headers
	bfh.bfSize = headers_size + pixel_data_size;

	// Create the file in disk to write
	HANDLE hFile = CreateFile(lpszFileName,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// Return if error opening file
	if (!hFile) return;

	DWORD dwWritten = 0;

	// Write the File header
	WriteFile(hFile,
		&bfh,
		sizeof(bfh),
		&dwWritten,
		NULL);

	// Write the bitmap info header  
	WriteFile(hFile,
		&bmpInfoHeader,
		sizeof(bmpInfoHeader),
		&dwWritten,
		NULL);

	// Write the RGB Data  
	WriteFile(hFile,
		pBitmapBits,
		bmpInfoHeader.biSizeImage,
		&dwWritten,
		NULL);

	// Close the file handle
	CloseHandle(hFile);
}