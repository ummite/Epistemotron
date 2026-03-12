// EpistemotronView.cpp : implementation of the CEpistemotronView class
//

#include "pch.h"
#ifndef SHARED_HANDLERS
#include "Epistemotron.h"
#endif

#include "EpistemotronDoc.h"
#include "EpistemotronView.h"
#include "Science/Universe.h"
#include "Science/Environment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEpistemotronView

IMPLEMENT_DYNCREATE(CEpistemotronView, CView)

BEGIN_MESSAGE_MAP(CEpistemotronView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CEpistemotronView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	// Simulation control commands
	ON_COMMAND(ID_SIMULATION_START, &CEpistemotronView::OnSimulationStart)
	ON_COMMAND(ID_SIMULATION_PAUSE, &CEpistemotronView::OnSimulationPause)
	ON_COMMAND(ID_SIMULATION_RESUME, &CEpistemotronView::OnSimulationResume)
	ON_COMMAND(ID_SIMULATION_STOP, &CEpistemotronView::OnSimulationStop)
	ON_COMMAND(ID_SIMULATION_RESET, &CEpistemotronView::OnSimulationReset)
	ON_COMMAND(ID_SIMULATION_SPEED_UP, &CEpistemotronView::OnSimulationSpeedUp)
	ON_COMMAND(ID_SIMULATION_SLOW_DOWN, &CEpistemotronView::OnSimulationSlowDown)
	ON_COMMAND(ID_SIMULATION_STEP, &CEpistemotronView::OnSimulationStep)
END_MESSAGE_MAP()

// CEpistemotronView construction/destruction

CEpistemotronView::CEpistemotronView() noexcept
	: m_simulationState(SimulationState::Stopped)
	, m_timerId(0)
	, m_stepsPerFrame(1)
	, m_stepSizeSec(3600)  // 1 hour per step by default
{
}

CEpistemotronView::~CEpistemotronView()
{
	StopSimulation();  // Ensure timer is stopped
}

BOOL CEpistemotronView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

// CEpistemotronView drawing

void CEpistemotronView::OnDraw(CDC* pDC)
{
	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc)
		return;

	CRect rcClient;
	GetClientRect(&rcClient);

	// Draw background (space - dark blue/black)
	pDC->FillSolidRect(&rcClient, RGB(5, 5, 20));

	// Get the current universe from the document
	Universe* pUniverse = pDoc->m_pCurrentUniverse;
	if (!pUniverse)
	{
		pDC->SetTextAlign(TA_CENTER);
		CString strText = _T("No simulation active.\nPress F5 or use Simulation menu to start.");
		pDC->TextOut(rcClient.CenterPoint().x, rcClient.CenterPoint().y, strText);
		return;
	}

	// Draw 3D projection of the universe
	RenderUniverse3D(pDC, *pUniverse, rcClient);

	// Draw UI overlay
	DrawUIOverlay(pDC, rcClient);
}

void CEpistemotronView::RenderUniverse3D(CDC* pDC, const Universe& universe, const CRect& rcClient)
{
	int centerX = rcClient.Width() / 2;
	int centerY = rcClient.Height() / 2;

	// Projection parameters
	double cameraDistance = 300000000.0;  // 300 million km
	double fov = 50000000.0;  // Field of view in km

	// Collect all masses with their screen positions
	struct MassRenderInfo
	{
		const Mass* mass;
		int screenX;
		int screenY;
		int radius;
		double depth;
		COLORREF color;
	};
	std::vector<MassRenderInfo> masses;

	int massCount = universe.GetMassCount();
	for (int i = 0; i < massCount; i++)
	{
		const Mass& m = universe.GetAt(i);

		// Simple perspective projection (ignore rotation for now)
		double scale = fov / (cameraDistance - m.m_Z);
		int screenX = centerX + (int)(m.m_X * scale);
		int screenY = centerY + (int)(m.m_Y * scale);

		// Radius proportional to cube root of mass (volume)
		int radius = (int)pow(m.m_MasseKG, 1.0/3.0) * 0.0001;
		radius = max(2, min(radius, 50));  // Clamp between 2 and 50 pixels

		// Color based on mass (simplified)
		COLORREF color;
		if (m.m_MasseKG > 1e30)
			color = RGB(255, 255, 200);  // Star (yellow-white)
		else if (m.m_MasseKG > 1e26)
			color = RGB(100, 150, 255);  // Planet (blue)
		else
			color = RGB(200, 200, 200);  // Moon/asteroid (gray)

		MassRenderInfo info;
		info.mass = &m;
		info.screenX = screenX;
		info.screenY = screenY;
		info.radius = radius;
		info.depth = m.m_Z;
		info.color = color;
		masses.push_back(info);
	}

	// Sort by depth (painter's algorithm - draw far objects first)
	std::sort(masses.begin(), masses.end(),
		[](const MassRenderInfo& a, const MassRenderInfo& b) {
			return a.depth > b.depth;
		});

	// Create pen once outside the loop for performance
	CPen solidPen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* oldPen = pDC->SelectObject(&solidPen);

	// Draw each mass - create brushes only, pen is reused
	for (const auto& info : masses)
	{
		CBrush brush(info.color);
		CBrush* oldBrush = pDC->SelectObject(&brush);

		CRect ellipse(
			info.screenX - info.radius,
			info.screenY - info.radius,
			info.screenX + info.radius,
			info.screenY + info.radius
		);
		pDC->Ellipse(&ellipse);

		pDC->SelectObject(oldBrush);
	}

	// Restore pen once after the loop
	pDC->SelectObject(oldPen);
}

void CEpistemotronView::DrawUIOverlay(CDC* pDC, const CRect& rcClient)
{
	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
		return;

	CString statusText;

	// Draw simulation state
	switch (m_simulationState)
	{
	case SimulationState::Running:
		statusText = _T("Status: RUNNING");
		break;
	case SimulationState::Paused:
		statusText = _T("Status: PAUSED");
		break;
	case SimulationState::Stopped:
		statusText = _T("Status: STOPPED");
		break;
	}

	// Draw iteration count
	statusText += _T("\nIteration: ") + CString::Format(_T("%d"), pDoc->m_pCurrentUniverse->m_iIteration);
	statusText += _T("\nStep size: ") + CString::Format(_T("%d sec"), m_stepSizeSec);
	statusText += _T("\nSteps/frame: ") + CString::Format(_T("%d"), m_stepsPerFrame);

	// Draw text in top-left corner
	pDC->SetTextColor(RGB(255, 255, 255));
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(10, 10, statusText);
}

// Simulation control methods

void CEpistemotronView::StartSimulation()
{
	if (m_simulationState == SimulationState::Running)
		return;

	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc)
		return;

	// Initialize universe if not exists
	if (!pDoc->m_pCurrentUniverse)
	{
		ResetSimulation();
	}

	m_simulationState = SimulationState::Running;
	m_timerId = SetTimer(1, 100, nullptr);  // 100ms timer (10 FPS)
}

void CEpistemotronView::PauseSimulation()
{
	if (m_simulationState != SimulationState::Running)
		return;

	m_simulationState = SimulationState::Paused;
	KillTimer(m_timerId);
	m_timerId = 0;
}

void CEpistemotronView::ResumeSimulation()
{
	if (m_simulationState != SimulationState::Paused)
		return;

	m_simulationState = SimulationState::Running;
	m_timerId = SetTimer(1, 100, nullptr);
}

void CEpistemotronView::StopSimulation()
{
	if (m_timerId != 0)
	{
		KillTimer(m_timerId);
		m_timerId = 0;
	}
	m_simulationState = SimulationState::Stopped;
	Invalidate();
}

void CEpistemotronView::ResetSimulation()
{
	StopSimulation();

	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc)
		return;

	// Clean up old universe
	if (pDoc->m_pCurrentUniverse)
	{
		delete pDoc->m_pCurrentUniverse;
		pDoc->m_pCurrentUniverse = nullptr;
	}

	// Create new universe with solar system configuration
	pDoc->m_pCurrentUniverse = new Universe(4);

	{
		Mass& sun = pDoc->m_pCurrentUniverse->GetAt(0);
		sun.m_MasseKG = 1.989e30;  // Solar mass
		sun.m_X = 0;
		sun.m_Y = 0;
		sun.m_Z = 0;
		sun.m_VitesseX = 0;
		sun.m_VitesseY = 0;
		sun.m_VitesseZ = 0;

		Mass& earth = pDoc->m_pCurrentUniverse->GetAt(1);
		earth.m_MasseKG = 5.972e24;  // Earth mass
		earth.m_X = 149598000;  // 1 AU in km
		earth.m_Y = 0;
		earth.m_Z = 0;
		earth.m_VitesseX = 0;
		earth.m_VitesseY = 29780;  // Orbital velocity in m/s
		earth.m_VitesseZ = 0;

		Mass& moon = pDoc->m_pCurrentUniverse->GetAt(2);
		moon.m_MasseKG = 7.342e22;  // Moon mass
		moon.m_X = earth.m_X + 384400;  // Earth-Moon distance
		moon.m_Y = 0;
		moon.m_Z = 0;
		moon.m_VitesseX = 0;
		moon.m_VitesseY = earth.m_VitesseY + 1023;  // Earth + Moon orbital velocity
		moon.m_VitesseZ = 0;

		Mass& mars = pDoc->m_pCurrentUniverse->GetAt(3);
		mars.m_MasseKG = 6.39e23;  // Mars mass
		mars.m_X = 227940000;  // Mars semi-major axis in km
		mars.m_Y = 0;
		mars.m_Z = 0;
		mars.m_VitesseX = 0;
		mars.m_VitesseY = 24070;  // Mars orbital velocity in m/s
		mars.m_VitesseZ = 0;
	}

	pDoc->m_pCurrentUniverse->m_iIteration = 0;
	m_stepsPerFrame = 1;
	m_stepSizeSec = 3600;  // 1 hour per step

	Invalidate();
}

void CEpistemotronView::StepSimulation()
{
	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
		return;

	// Generate next step and properly delete old universe to prevent memory leak
	Universe* pNextUniverse = pDoc->m_pCurrentUniverse->GenerateSimulationStep(m_stepSizeSec);
	delete pDoc->m_pCurrentUniverse;
	pDoc->m_pCurrentUniverse = pNextUniverse;
	Invalidate();
}

void CEpistemotronView::SpeedUp()
{
	m_stepsPerFrame = min(m_stepsPerFrame * 2, 100);
	Invalidate();
}

void CEpistemotronView::SlowDown()
{
	m_stepsPerFrame = max(m_stepsPerFrame / 2, 1);
	Invalidate();
}

// Timer handler

void CEpistemotronView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent != 1 || m_simulationState != SimulationState::Running)
	{
		CView::OnTimer(nIDEvent);
		return;
	}

	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
	{
		StopSimulation();
		return;
	}

	// Execute multiple simulation steps per frame
	for (int i = 0; i < m_stepsPerFrame; i++)
	{
		// Generate next step and properly delete old universe to prevent memory leak
		Universe* pNextUniverse = pDoc->m_pCurrentUniverse->GenerateSimulationStep(m_stepSizeSec);
		delete pDoc->m_pCurrentUniverse;
		pDoc->m_pCurrentUniverse = pNextUniverse;
	}

	Invalidate();  // Trigger redraw
}

// Command handlers

void CEpistemotronView::OnSimulationStart()
{
	StartSimulation();
}

void CEpistemotronView::OnSimulationPause()
{
	PauseSimulation();
}

void CEpistemotronView::OnSimulationResume()
{
	ResumeSimulation();
}

void CEpistemotronView::OnSimulationStop()
{
	StopSimulation();
}

void CEpistemotronView::OnSimulationReset()
{
	ResetSimulation();
}

void CEpistemotronView::OnSimulationSpeedUp()
{
	SpeedUp();
}

void CEpistemotronView::OnSimulationSlowDown()
{
	SlowDown();
}

void CEpistemotronView::OnSimulationStep()
{
	StepSimulation();
}

// Other methods

void CEpistemotronView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CEpistemotronView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CEpistemotronView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CEpistemotronView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CEpistemotronView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CEpistemotronView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CEpistemotronView diagnostics

#ifdef _DEBUG
void CEpistemotronView::AssertValid() const
{
	CView::AssertValid();
}

void CEpistemotronView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEpistemotronDoc* CEpistemotronView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEpistemotronDoc)));
	return (CEpistemotronDoc*)m_pDocument;
}
#endif //_DEBUG

void CEpistemotronView::RefreshView()
{
	Invalidate();  // Trigger a redraw
}
