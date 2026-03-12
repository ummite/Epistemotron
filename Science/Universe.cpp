#include "pch.h"

#include "Universe.h"
#include "Mass.h"
#include "Environment.h"
#include <vector>


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
		// Use operator[] instead of GetAt() to get non-const reference
		m_arrMasses[i].Randomize();
	}
}

// Export universe visualization as PPM/BMP
void Universe::ExportPPM(int p_iWidth, int p_iHeight)
{
	if (m_arrMasses.GetSize() < 1)
	{
		return;  // Nothing to export
	}

	const Mass& roUniverseCenter = m_arrMasses.GetAt(0);

	// Find maximum distance from center for zoom calculation
	double dblDistanceMax = 1.0;
	for (int i = 1; i < m_arrMasses.GetSize(); i++)
	{
		double dblNewDistance = roUniverseCenter.Distance(m_arrMasses.GetAt(i));
		if (dblNewDistance > dblDistanceMax)
		{
			dblDistanceMax = dblNewDistance;
		}
	}

	const int iMassStep = 16;
	const double ZOOM_FACTOR = 1.4142;  // sqrt(2) for aspect ratio

	double dblXStep = (ZOOM_FACTOR * 2.0 * dblDistanceMax) / p_iWidth;
	double dblYStep = (ZOOM_FACTOR * 2.0 * dblDistanceMax) / p_iHeight;

	// Use RAII containers to prevent memory leaks
	std::vector<int> pixelMatrix(static_cast<size_t>(p_iWidth) * p_iHeight, 0);
	std::vector<BYTE> buf(static_cast<size_t>(p_iWidth) * p_iHeight * 3, 0);

	for (int i = 0; i < m_arrMasses.GetSize(); i++)
	{
		const Mass& roMass = m_arrMasses.GetAt(i);
		int x = (roMass.m_X - roUniverseCenter.m_X) / dblXStep + p_iWidth / 2;
		int y = (roMass.m_Y - roUniverseCenter.m_Y) / dblYStep + p_iHeight / 2;

		if (x >= 0 && x < p_iWidth && y >= 0 && y < p_iHeight)
		{
			pixelMatrix[static_cast<size_t>(x) + y * p_iWidth] += 15;
		}
	}

	for (int y = 0; y < p_iHeight; y++)
	{
		for (int x = 0; x < p_iWidth; x++)
		{
			int iValue = pixelMatrix[static_cast<size_t>(x) + y * p_iWidth];
			iValue = (iValue > 0) ? 255 : 0;  // Binary: mass present or not

			size_t idx = (static_cast<size_t>(x) + y * p_iWidth) * 3;
			buf[idx] = static_cast<BYTE>(iValue);
			buf[idx + 1] = static_cast<BYTE>(iValue);
			buf[idx + 2] = static_cast<BYTE>(iValue);
		}
	}

	CString outputPath = _T("c:\\temp\\t.bmp");
	SaveBitmapToFile(buf.data(), p_iWidth, p_iHeight, 24, 0, outputPath);
}

void Universe::SimulateFrom(const Universe& p_roUniverse, int p_iStepSize)
{
	// Symplectic Euler integration: update position FIRST, then velocity
	// This preserves energy better than standard Euler (velocity-first)
	// For each mass, we use the OLD positions (from p_roUniverse) to compute forces,
	// then update velocities, then update positions using new velocities.

	// Step 1: Update velocities based on current positions (from previous universe state)
	for (int i = 0; i < m_arrMasses.GetSize(); i++)
	{
		Mass& roMass = m_arrMasses.GetAt(i);
		roMass.EffectuerPasChangementVitesse(p_roUniverse, p_iStepSize);
	}

	// Step 2: Update positions using the NEW velocities (symplectic order)
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
	// 0x4D = M 0�42 = B storing in reverse order to match with endian  
	bfh.bfType = 0x4D42;
	//bfh.bfType = 'B'+('M' << 8); 

	// <<8 used to shift �M� to end  */  

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