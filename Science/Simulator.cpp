#include "pch.h"

#include "Simulator.h"
#include "Mass.h"
#include "Environment.h"
#include "Universe.h"


Simulator::Simulator()
{

}

Simulator::~Simulator()
{

}

void Simulator::Test()
{
	int iStepSize = 60*60;	// One hours

	/*Universe oOriginalUniverse(K_NombreEtoile);
	oOriginalUniverse.Randomize();
*/

	Universe oOriginalUniverse(2);

	Mass& roSun = oOriginalUniverse.m_arrMasses.GetAt(0);
	roSun.m_MasseKG = 2e30;

	Mass& roEarth = oOriginalUniverse.m_arrMasses.GetAt(1);
	roEarth.m_MasseKG = 5.98e24;
	roEarth.m_X = 146000000;	// 146 million km
	roEarth.m_VitesseY = -29720;	// m/s

	//Mass& roMoon = oOriginalUniverse.m_arrMasses.GetAt(1);
	//roMoon.m_MasseKG = 7.342e22;	//terre.m_MasseKG/81.0;
	//roMoon.m_X = K_Distance_Terre_Lune; //km
	//roMoon.m_VitesseY = 1023;	// 3683 km / h is around 1023.056 m/s

		//Mass& lune2 = oOriginalUniverse.m_arrMasses.GetAt(2);
		//lune2.m_MasseKG = terre.m_MasseKG / 81.0;
		//lune2.m_X = K_Distance_Terre_Lune * 2;
		//lune2.m_VitesseY = 377;

		//Mass* lune2 = new Mass();
		//lune2->m_MasseKG = terre->m_MasseKG / 81.0;
		//lune2->m_X = K_Distance_Terre_Lune*2; //km
		//lune2->m_VitesseY = 377; // pense que c'est proche en mètre / seconde

	oOriginalUniverse.ExportPPM();

	TRACE(oOriginalUniverse.m_arrMasses.GetAt(0).Trace(oOriginalUniverse));
	TRACE(oOriginalUniverse.m_arrMasses.GetAt(1).Trace(oOriginalUniverse));

	Universe* poCurrentUniverse = oOriginalUniverse.GenerateSimulationStep(iStepSize);
	poCurrentUniverse->ExportPPM();

	//while (poCurrentUniverse->m_iIteration < 2 * 24 * 365)
	while (true)
	{
		//CString strTrace;
		//strTrace.Format(_T("%d\n"), poCurrentUniverse->m_iIteration);
		//TRACE(strTrace);
		Universe* poWhileUniverse = poCurrentUniverse->GenerateSimulationStep(iStepSize);
		
		// Try...
		poWhileUniverse->m_arrMasses.GetAt(0).m_X = 0;
		poWhileUniverse->m_arrMasses.GetAt(0).m_Y = 0;

		if (poWhileUniverse->m_iIteration % (24) == 0)
		{
			TRACE(poWhileUniverse->m_arrMasses.GetAt(0).Trace(*poWhileUniverse));

			TRACE(poWhileUniverse->m_arrMasses.GetAt(1).Trace(*poWhileUniverse));
			poWhileUniverse->ExportPPM();
		}

		delete poCurrentUniverse;
		poCurrentUniverse = poWhileUniverse;

		::Sleep(0);
	}
	::Sleep(234234);

	poCurrentUniverse->ExportPPM();
	delete poCurrentUniverse;

	AfxMessageBox(_T("End"));
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