
// EpistemotronDoc.cpp : implementation of the CEpistemotronDoc class
//

#include "pch.h"
#ifndef SHARED_HANDLERS
#include "Epistemotron.h"
#endif

#include "EpistemotronDoc.h"
#include "Science/Universe.h"
#include "Science/Environment.h"  // For K_NombreEtoile

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEpistemotronDoc

IMPLEMENT_DYNCREATE(CEpistemotronDoc, CDocument)

BEGIN_MESSAGE_MAP(CEpistemotronDoc, CDocument)
END_MESSAGE_MAP()


// CEpistemotronDoc construction/destruction

CEpistemotronDoc::CEpistemotronDoc() noexcept
	: m_pCurrentUniverse(nullptr)
{
}

CEpistemotronDoc::~CEpistemotronDoc()
{
	if (m_pCurrentUniverse)
	{
		delete m_pCurrentUniverse;
		m_pCurrentUniverse = nullptr;
	}
}

// Copy constructor - deep copy of universe
CEpistemotronDoc::CEpistemotronDoc(const CEpistemotronDoc& src)
	: CDocument(), m_pCurrentUniverse(nullptr)
{
	// Deep copy the universe if source has one
	if (src.m_pCurrentUniverse != nullptr)
	{
		int nMassCount = src.m_pCurrentUniverse->GetMassCount();
		m_pCurrentUniverse = new Universe(nMassCount);
		m_pCurrentUniverse->m_iIteration = src.m_pCurrentUniverse->m_iIteration;

		// Copy each mass properties
		for (int i = 0; i < nMassCount; i++)
		{
			const Mass& srcMass = src.m_pCurrentUniverse->GetAt(i);
			Mass& dstMass = m_pCurrentUniverse->GetAt(i);

			dstMass.m_X = srcMass.m_X;
			dstMass.m_Y = srcMass.m_Y;
			dstMass.m_Z = srcMass.m_Z;
			dstMass.m_VitesseX = srcMass.m_VitesseX;
			dstMass.m_VitesseY = srcMass.m_VitesseY;
			dstMass.m_VitesseZ = srcMass.m_VitesseZ;
			dstMass.m_MasseKG = srcMass.m_MasseKG;
		}
	}
}

// Copy assignment operator - deep copy with self-assignment check, exception-safe
CEpistemotronDoc& CEpistemotronDoc::operator=(const CEpistemotronDoc& src)
{
	if (this != &src)  // Self-assignment check
	{
		// Create new universe first (exception-safe: old data preserved if allocation fails)
		Universe* pNewUniverse = nullptr;

		if (src.m_pCurrentUniverse != nullptr)
		{
			int nMassCount = src.m_pCurrentUniverse->GetMassCount();
			pNewUniverse = new Universe(nMassCount);
			pNewUniverse->m_iIteration = src.m_pCurrentUniverse->m_iIteration;

			// Copy each mass properties
			for (int i = 0; i < nMassCount; i++)
			{
				const Mass& srcMass = src.m_pCurrentUniverse->GetAt(i);
				Mass& dstMass = pNewUniverse->GetAt(i);

				dstMass.m_X = srcMass.m_X;
				dstMass.m_Y = srcMass.m_Y;
				dstMass.m_Z = srcMass.m_Z;
				dstMass.m_VitesseX = srcMass.m_VitesseX;
				dstMass.m_VitesseY = srcMass.m_VitesseY;
				dstMass.m_VitesseZ = srcMass.m_VitesseZ;
				dstMass.m_MasseKG = srcMass.m_MasseKG;
			}
		}

		// Now delete old universe and replace with new (strong exception guarantee)
		delete m_pCurrentUniverse;
		m_pCurrentUniverse = pNewUniverse;
	}
	return *this;
}

// Move constructor - transfer ownership
CEpistemotronDoc::CEpistemotronDoc(CEpistemotronDoc&& src) noexcept
	: CDocument(), m_pCurrentUniverse(nullptr)
{
	m_pCurrentUniverse = src.m_pCurrentUniverse;
	src.m_pCurrentUniverse = nullptr;
}

// Move assignment operator - transfer ownership with self-assignment check
CEpistemotronDoc& CEpistemotronDoc::operator=(CEpistemotronDoc&& src) noexcept
{
	if (this != &src)  // Self-assignment check
	{
		delete m_pCurrentUniverse;
		m_pCurrentUniverse = src.m_pCurrentUniverse;
		src.m_pCurrentUniverse = nullptr;
	}
	return *this;
}

BOOL CEpistemotronDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// Clear old universe to prevent memory leak in SDI mode
	if (m_pCurrentUniverse != nullptr)
	{
		delete m_pCurrentUniverse;
		m_pCurrentUniverse = nullptr;
	}

	// Create fresh universe and load the default Solar System scenario
	m_pCurrentUniverse = new Universe(1);  // Temporary, will be resized by LoadScenario
	m_pCurrentUniverse->LoadScenario(ScenarioType::SolarSystem);

	// Notify views of new document state
	UpdateAllViews(nullptr);

	return TRUE;
}




// CEpistemotronDoc serialization

void CEpistemotronDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// Save universe state
		if (m_pCurrentUniverse != nullptr)
		{
			// Save iteration count
			ar << m_pCurrentUniverse->m_iIteration;

			// Save mass count
			int nMassCount = m_pCurrentUniverse->GetMassCount();
			ar << nMassCount;

			// Save each mass properties
			for (int i = 0; i < nMassCount; i++)
			{
				const Mass& mass = m_pCurrentUniverse->GetAt(i);
				ar << mass.m_X << mass.m_Y << mass.m_Z;      // Position (km)
				ar << mass.m_VitesseX << mass.m_VitesseY << mass.m_VitesseZ;  // Velocity (m/s)
				ar << mass.m_MasseKG;  // Mass (kg)
			}
		}
		else
		{
			// No universe - save empty state
			int nEmpty = -1;
			ar << nEmpty;
		}
	}
	else
	{
		// Load universe state
		int nIteration;
		ar >> nIteration;

		// Check for empty state marker
		if (nIteration == -1)
		{
			return;  // Empty document
		}

		// Delete old universe if exists
		if (m_pCurrentUniverse != nullptr)
		{
			delete m_pCurrentUniverse;
			m_pCurrentUniverse = nullptr;
		}

		// Read mass count
		int nMassCount;
		ar >> nMassCount;

		if (nMassCount <= 0)
		{
			return;  // Invalid or empty
		}

		// Create new universe with specified mass count
		m_pCurrentUniverse = new Universe(nMassCount);
		m_pCurrentUniverse->m_iIteration = nIteration;

		// Load each mass properties
		for (int i = 0; i < nMassCount; i++)
		{
			Mass& mass = m_pCurrentUniverse->GetAt(i);
			ar >> mass.m_X >> mass.m_Y >> mass.m_Z;      // Position (km)
			ar >> mass.m_VitesseX >> mass.m_VitesseY >> mass.m_VitesseZ;  // Velocity (m/s)
			ar >> mass.m_MasseKG;  // Mass (kg)
		}

		// Notify views of data change
		UpdateAllViews(nullptr);
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CEpistemotronDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
	fontDraw.DeleteObject();  // Properly cleanup GDI resource
}

// Support for Search Handlers
void CEpistemotronDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CEpistemotronDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CEpistemotronDoc diagnostics

#ifdef _DEBUG
void CEpistemotronDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEpistemotronDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEpistemotronDoc commands
