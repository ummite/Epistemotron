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
	/*double dblMinX = m_arrMasses.GetAt(0).m_X;
	double dblMaxX = m_arrMasses.GetAt(0).m_X;
	double dblMinY = m_arrMasses.GetAt(0).m_Y;
	double dblMaxY = m_arrMasses.GetAt(0).m_Y;
	double dblMinZ = m_arrMasses.GetAt(0).m_Z;
	double dblMaxZ = m_arrMasses.GetAt(0).m_Z;

	for (int i = 0; i < m_arrMasses.GetSize(); i++)
	{
		const Mass& roMass = m_arrMasses.GetAt(i);
		if (roMass.m_MasseKG > dblMaxMass)
		{
			dblMaxMass = roMass.m_MasseKG;
		}

		dblMinX = min(dblMinX, roMass.m_X);
		dblMaxX = max(dblMaxX, roMass.m_X);

		dblMinY = min(dblMinY, roMass.m_Y);
		dblMaxY = max(dblMaxY, roMass.m_Y);

		dblMinZ = min(dblMinZ, roMass.m_Z);
		dblMaxZ = max(dblMaxZ, roMass.m_Z);
	}

	const Mass& roUniverseCenter = m_arrMasses.GetAt(0);
	if ((dblMaxX - roUniverseCenter.m_X) > (roUniverseCenter.m_X - dblMinX))
	{
		dblMinX = roUniverseCenter.m_X - (dblMaxX - roUniverseCenter.m_X);
	}
	else
	{
		dblMaxX = roUniverseCenter.m_X + (roUniverseCenter.m_X - dblMinX);
	}

	if ((dblMaxY - roUniverseCenter.m_Y) > (roUniverseCenter.m_Y - dblMinY))
	{
		dblMinY = roUniverseCenter.m_Y - (dblMaxY - roUniverseCenter.m_Y);
	}
	else
	{
		dblMaxY = roUniverseCenter.m_Y + (roUniverseCenter.m_Y - dblMinY);
	}

	if ((dblMaxZ - roUniverseCenter.m_Z) > (roUniverseCenter.m_Z - dblMinZ))
	{
		dblMinZ = roUniverseCenter.m_Z - (dblMaxZ - roUniverseCenter.m_Z);
	}
	else
	{
		dblMaxZ = roUniverseCenter.m_Z + (roUniverseCenter.m_Z - dblMinZ);
	}*/

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

	CStringA strFilename1;
	strFilename1.Format("Images\\Export%.5d.pgm", s_iFileIteration++);
	CFile oFile1(CString(strFilename1), CFile::modeCreate | CFile::modeReadWrite);

	CStringA strHeaderFormat;
	strHeaderFormat.Format(("P2 %d %d %d \n"), p_iWidth, p_iHeight, iMassStep - 1);
	
	oFile1.Write(strHeaderFormat, strHeaderFormat.GetLength());

	// Knowing min x and max x, divided by 1024 will represent the x step
	// Knowing min y and max y, divided by 768 will represent the y step

	//double dblXStep = (dblMaxX - dblMinX) / p_iWidth;
	//double dblYStep = (dblMaxY - dblMinY) / p_iHeight;
	
	double dblXStep = (1.4142 * 2.0 * dblDistanceMax) / p_iWidth;
	double dblYStep = (1.4142 * 2.0 * dblDistanceMax) / p_iHeight;
	double dblMinX = roUniverseCenter.m_X - (dblXStep * p_iWidth / 2);
	double dblMaxX = roUniverseCenter.m_X + (dblXStep * p_iWidth / 2);
	double dblMinY = roUniverseCenter.m_Y - (dblYStep * p_iHeight / 2);
	double dblMaxY = roUniverseCenter.m_Y + (dblYStep * p_iHeight / 2);

	double dblMassStep = dblMaxMass / iMassStep;

	int* poMatrix = new int[p_iWidth * p_iHeight] {};

	for (int i = 0; i < m_arrMasses.GetSize(); i++)
	{
		const Mass& roMass = m_arrMasses.GetAt(i);
		int x = max(0, ((int)(roMass.m_X - dblMinX) / dblXStep) - 1);
		int y = max(0, ((int)(roMass.m_Y - dblMinY) / dblYStep) - 1);
		poMatrix[x + (y * p_iWidth)] += 15; //1 + (int)(roMass.m_MasseKG / dblMassStep);
	}

	BYTE* buf = new BYTE[3 * p_iWidth * p_iHeight];
	int c = 0;

	for (int y = 0; y < p_iHeight; y++)
	{
		CStringA strLine;
		for (int x = 0; x < p_iWidth; x++)
		{
			int iValue = min(poMatrix[x + (y * p_iWidth)], 15);
			strLine.AppendFormat(("%d "), iValue);	// Could overflow if multiple mass are in the same spot.

			if (iValue > 0)
			{
				iValue = 255;
				buf[c + 0 - 3] = (BYTE)iValue;
				buf[c + 1 - 3] = (BYTE)iValue;
				buf[c + 2 - 3] = (BYTE)iValue;
				buf[c + 0 + 3] = (BYTE)iValue;
				buf[c + 1 + 3] = (BYTE)iValue;
				buf[c + 2 + 3] = (BYTE)iValue;
			}
			buf[c + 0] = (BYTE)iValue;
			buf[c + 1] = (BYTE)iValue;
			buf[c + 2] = (BYTE)iValue;
			c += 3;
		}
		strLine.AppendFormat(("\n"));

		oFile1.Write(strLine.GetBuffer(), strLine.GetLength() );
	}

	// Test
	//ExportBitmap(poMatrix);

	strFilename1.Replace("pgm", "bmp");
	CString strToto = strFilename1.AllocSysString();
	SaveBitmapToFile((BYTE*)buf,
		p_iWidth,
		p_iHeight,
		24,
		0,
		strToto);

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

//void DessinerEtoiles(const CString& elementAAfficher)
//{
//	CDC* dc = GetDC(NULL);
//	CRect rect;
//	GetClientRect( rect );  // Taille dessinable
//
//	CBrush brush;
//	brush.CreateSolidBrush(RGB(255, 255, 255));
//	dc->FillRect(&rect, &brush);
//
//  double minX = 0.0;
//  double minY = 0.0;
//  double minZ = 0.0;
//  double maxX = 0.0;
//  double maxY = 0.0;
//  double maxZ = 0.0;
//
//  AScEtoile::ObtenirDimensionUniversXYZ( minX, maxX, minY, maxY, minZ, maxZ);
//
//  CString supplement;
//  supplement.Format(L" Dimension de l'univers en KM: x:%e y:%e", maxX-minX, maxY-minY);
//
//	if( !elementAAfficher.IsEmpty() )
//	{
//		dc->DrawText( elementAAfficher + supplement, rect, NULL );
//	}
//
//  TRACE("\nRECT:%d %d ", rect.left, rect.right);
//
//
//	//static int compteur = 0;
//
//	//CString texte;
//	//texte.Format( _T("%d"), compteur++ );
//	//if( 0 == (compteur % 1 ) )
//	//{
//	//	dc->DrawText( texte, rect, NULL );
//	//}
//
//
//const double largeurUniversVisible = maxX - minX;
//const double hauteurUniversVisible = maxY - minY;
//
//int marge = 10;
//if (rect.Width() / 2 < marge ||
//	rect.Height() / 2 < marge)
//{
//	marge = 0;
//}
//
//for (int i = 0; i < AScEtoile::ms_Etoiles.GetSize(); i++)
//{
//	AScEtoile* uneEtoile = AScEtoile::ms_Etoiles.GetAt(i);
//	double proportionX = (uneEtoile->m_X - minX) / largeurUniversVisible; // Résultat entre 0 et 1.
//	double proportionY = (uneEtoile->m_Y - minY) / hauteurUniversVisible; // Résultat entre 0 et 1.
//
//	int positionXDansRect = marge + rect.left + ((rect.Width() - (2 * marge)) * proportionX);
//	int positionYDansRect = marge + rect.top + ((rect.Height() - (2 * marge)) * proportionY);
//
//	TRACE(uneEtoile->ObtainTextDetails());
//	TRACE(elementAAfficher);
//	dc->MoveTo(positionXDansRect, positionYDansRect);
//
//	int dist = 20.0 * uneEtoile->m_MasseKG / (double)MASSE_SOLEIL;
//
//	dist = max(dist, 5);
//	CPoint hg(positionXDansRect - dist, positionYDansRect - dist);
//	CPoint bd(positionXDansRect + dist + 2, positionYDansRect + dist + 2);
//	CRect rect(hg, bd);
//
//	dc->Ellipse(rect);
//}
//}
//
//void OnTimer(UINT nIDEvent)
//{
//	static COleDateTime momentPresent(2000, 1, 1, 0, 0, 1);
//	static bool enCours = false;
//	if (true == enCours)
//	{
//		return;
//	}
//
//	enCours = true;
//
//	const int avant = GetTickCount();
//
//	for (int i = 0; i < K_NombrePasParRefresh; i++)
//	{
//		PasDeSimulation(K_PasDeSimulationEnSeconde);
//	}
//
//	momentPresent += COleDateTimeSpan(0, 0, 0, K_PasDeSimulationEnSeconde * K_NombrePasParRefresh);
//
//	const int delais = GetTickCount() - avant;
//	CString tempsRequis;
//	tempsRequis.Format(_T("Temps Requis pour ce pas (%d iterations) %5.5dms   "), K_NombrePasParRefresh, delais);
//	tempsRequis += momentPresent.Format(VAR_DATEVALUEONLY);
//
//	CString heureTailleFixe = CString("            ") + momentPresent.Format(VAR_TIMEVALUEONLY);
//
//	tempsRequis += heureTailleFixe.Right(10);
//
//	int toto = tempsRequis.GetLength();
//
//	GetParent()->SetWindowText(tempsRequis);
//	DessinerEtoiles(tempsRequis);
//
//	CView::OnTimer(nIDEvent);
//	enCours = false;
//}
//
//void OnShowWindow(BOOL bShow, UINT nStatus)
//{
//	CView::OnShowWindow(bShow, nStatus);
//
//	SetTimer(1, K_PauseEntreChaquePas, NULL);
//}
//
//void PasDeSimulation(int in_NombreSeconde)
//{
//	for (int i = 0; i < AScEtoile::ms_Etoiles.GetSize(); i++)
//	{
//		AScEtoile* uneEtoile = (AScEtoile*)AScEtoile::ms_Etoiles.GetAt(i);
//		uneEtoile->EffectuerPasChangementPosition(in_NombreSeconde);
//	}
//
//	// Pour chacune des étoiles, faire le calcul de changement de vitesse par rapport aux autres étoiles.
//	for (int i = 0; i < AScEtoile::ms_Etoiles.GetSize(); i++)
//	{
//		AScEtoile* uneEtoile = (AScEtoile*)AScEtoile::ms_Etoiles.GetAt(i);
//		uneEtoile->EffectuerPasChangementVitesse(in_NombreSeconde);
//	}
//}

void Universe::ExportBitmap()
{
	CBitmap oBitmap;

	oBitmap.SetBitmapDimension(1024, 1024);
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
		0,
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