// EpistemotronView.cpp : implementation of the CEpistemotronView class
//

#include "pch.h"

// ============================================================================
// Named Constants (eliminating magic numbers)
// ============================================================================

// Simulation defaults
constexpr int DEFAULT_STEP_SIZE_SEC = 3600;              // 1 hour per step
constexpr double DEFAULT_CAMERA_DISTANCE_KM = 300000000.0;  // 300 million km
constexpr double DEFAULT_FIELD_OF_VIEW_KM = 50000000.0;    // 50 million km
constexpr double DEFAULT_MASS_SCALE_FACTOR = 0.0001;      // Mass radius scaling

// Camera bounds
constexpr double MIN_CAMERA_DISTANCE_KM = 10000.0;       // 10k km minimum
constexpr double MAX_CAMERA_DISTANCE_KM = 1e12;          // 1T km maximum
constexpr double MIN_FIELD_OF_VIEW_KM = 1000.0;          // 1k km minimum
constexpr double MAX_FIELD_OF_VIEW_KM = 1e11;            // 100B km maximum
constexpr double CAMERA_Z_BUFFER = 1000.0;               // Buffer behind camera (km)

// Rendering
constexpr int MIN_BODY_RADIUS = 2;                       // Minimum pixel radius
constexpr int MAX_BODY_RADIUS = 50;                      // Maximum pixel radius
constexpr int MAX_TRAIL_POINTS = 1000;                   // Max points per trail
constexpr double ZOOM_FACTOR = 1.1;                      // Zoom per wheel tick

// Camera rotation
constexpr double PI = 3.14159265358979323846;            // Pi constant
constexpr double ROTATION_SENSITIVITY = 0.005;           // Rotation per pixel drag
constexpr double MAX_PITCH_RADIANS = PI / 2 - 0.1;       // Max pitch (near 90 degrees)
constexpr double MAX_YAW_RADIANS = PI;                   // Max yaw (180 degrees)
constexpr double MAX_ROLL_RADIANS = PI / 4;              // Max roll (45 degrees)

// Mass classification thresholds (kg)
constexpr double STAR_MASS_THRESHOLD = 1e30;             // Star vs planet
constexpr double PLANET_MASS_THRESHOLD = 1e26;           // Planet vs moon/asteroid

// Simulation
constexpr int MAX_STEPS_PER_FRAME = 100;                 // Maximum steps per frame
constexpr UINT_PTR TIMER_ID = 1;                          // Timer identifier
constexpr UINT TIMER_INTERVAL_MS = 100;                  // 10 FPS target

// Scenario step sizes (seconds)
constexpr int STEP_SIZE_SOLAR_SYSTEM = 3600;             // 1 hour
constexpr int STEP_SIZE_BINARY_STAR = 86400;             // 1 day
constexpr int STEP_SIZE_THREE_BODY = 3600;               // 1 hour
constexpr int STEP_SIZE_GALAXY = 86400 * 30;             // 1 month

// UI colors
constexpr COLORREF COLOR_SPACE_BG = RGB(5, 5, 20);        // Background space color
constexpr COLORREF COLOR_TEXT = RGB(255, 255, 200);       // UI text color
constexpr COLORREF COLOR_OUTLINE = RGB(0, 0, 0);          // Body outline color
constexpr COLORREF COLOR_STAR = RGB(255, 255, 200);       // Star body color
constexpr COLORREF COLOR_PLANET = RGB(100, 150, 255);     // Planet body color
constexpr COLORREF COLOR_ASTEROID = RGB(200, 200, 200);   // Moon/asteroid color
constexpr COLORREF COLOR_STAR_TRAIL = RGB(200, 200, 150); // Star trail color
constexpr COLORREF COLOR_PLANET_TRAIL = RGB(50, 100, 200);// Planet trail color
constexpr COLORREF COLOR_ASTEROID_TRAIL = RGB(100, 100, 100); // Asteroid trail color
constexpr COLORREF COLOR_INVALID = (COLORREF)-1;              // Invalid color sentinel

#ifndef SHARED_HANDLERS
#include "Epistemotron.h"
#endif

#include "EpistemotronDoc.h"
#include "EpistemotronView.h"
#include "SimConfigDlg.h"
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
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	// Simulation control commands
	ON_COMMAND(ID_SIMULATION_START, &CEpistemotronView::OnSimulationStart)
	ON_COMMAND(ID_SIMULATION_PAUSE, &CEpistemotronView::OnSimulationPause)
	ON_COMMAND(ID_SIMULATION_RESUME, &CEpistemotronView::OnSimulationResume)
	ON_COMMAND(ID_SIMULATION_STOP, &CEpistemotronView::OnSimulationStop)
	ON_COMMAND(ID_SIMULATION_RESET, &CEpistemotronView::OnSimulationReset)
	ON_COMMAND(ID_SIMULATION_SPEED_UP, &CEpistemotronView::OnSimulationSpeedUp)
	ON_COMMAND(ID_SIMULATION_SLOW_DOWN, &CEpistemotronView::OnSimulationSlowDown)
	ON_COMMAND(ID_SIMULATION_STEP, &CEpistemotronView::OnSimulationStep)
	ON_COMMAND(ID_SIMULATION_CONFIG, &CEpistemotronView::OnSimulationConfig)
	// Scenario selection commands
	ON_COMMAND(ID_SCENARIO_SOLAR_SYSTEM, &CEpistemotronView::OnScenarioSolarSystem)
	ON_COMMAND(ID_SCENARIO_BINARY_STAR, &CEpistemotronView::OnScenarioBinaryStar)
	ON_COMMAND(ID_SCENARIO_THREE_BODY, &CEpistemotronView::OnScenarioThreeBody)
	ON_COMMAND(ID_SCENARIO_GALAXY, &CEpistemotronView::OnScenarioGalaxy)
	ON_COMMAND(ID_SCENARIO_NEXT, &CEpistemotronView::OnScenarioNext)
END_MESSAGE_MAP()

// CEpistemotronView construction/destruction

CEpistemotronView::CEpistemotronView() noexcept
	: m_simulationState(SimulationState::Stopped)
	, m_timerId(0)
	, m_stepsPerFrame(1)
	, m_stepSizeSec(DEFAULT_STEP_SIZE_SEC)
	, m_cameraDistance(DEFAULT_CAMERA_DISTANCE_KM)
	, m_fieldOfView(DEFAULT_FIELD_OF_VIEW_KM)
	, m_massScaleFactor(DEFAULT_MASS_SCALE_FACTOR)
	, m_panOffsetX(0.0)
	, m_panOffsetY(0.0)
	, m_rotationPitch(0.0)
	, m_rotationYaw(0.0)
	, m_rotationRoll(0.0)
	, m_bDragging(FALSE)
	, m_bRotating(FALSE)
	, m_bRolling(FALSE)
	, m_bShowTrails(TRUE)
	, m_integratorType(IntegratorType::SymplecticEuler)
	, m_currentScenario(ScenarioType::SolarSystem)
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
	// Validate DC parameter
	if (pDC == nullptr)
		return;

	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc)
		return;

	CRect rcClient;
	GetClientRect(&rcClient);

	// Validate client area
	int width = rcClient.Width();
	int height = rcClient.Height();
	if (width <= 0 || height <= 0)
		return;

	// Double buffering to prevent flickering
	CDC memDC;
	CBitmap memBitmap;
	CBitmap* pOldBitmap = nullptr;

	if (!memDC.CreateCompatibleDC(pDC))
	{
		TRACE(_T("Failed to create compatible DC for double buffering\n"));
		return;
	}

	if (!memBitmap.CreateCompatibleBitmap(pDC, width, height))
	{
		TRACE(_T("Failed to create compatible bitmap for double buffering\n"));
		memDC.DeleteDC();
		return;
	}

	pOldBitmap = memDC.SelectObject(&memBitmap);
	if (pOldBitmap == nullptr)
	{
		TRACE(_T("Failed to select bitmap into DC\n"));
		memBitmap.DeleteObject();
		memDC.DeleteDC();
		return;
	}

	// Draw background (space - dark blue/black)
	memDC.FillSolidRect(&rcClient, COLOR_SPACE_BG);

	// Get the current universe from the document
	Universe* pUniverse = pDoc->m_pCurrentUniverse;
	if (!pUniverse)
	{
		memDC.SetTextAlign(TA_CENTER);
		CString strText = _T("No simulation active.\nPress F5 or use Simulation menu to start.");
		memDC.TextOut(rcClient.CenterPoint().x, rcClient.CenterPoint().y, strText);
	}
	else
	{
		// Draw 3D projection of the universe
		RenderUniverse3D(&memDC, *pUniverse, rcClient);

		// Draw UI overlay
		DrawUIOverlay(&memDC, rcClient);
	}

	// Blit to screen
	pDC->BitBlt(0, 0, width, height, &memDC, 0, 0, SRCCOPY);

	// Cleanup (restore original bitmap and release resources)
	memDC.SelectObject(pOldBitmap);
	memBitmap.DeleteObject();
	memDC.DeleteDC();
}

void CEpistemotronView::RenderUniverse3D(CDC* pDC, const Universe& universe, const CRect& rcClient)
{
	// Validate parameters
	if (pDC == nullptr)
		return;

	if (m_cameraDistance <= 0 || m_fieldOfView <= 0)
	{
		TRACE(_T("Invalid camera parameters: distance=%.0f, fov=%.0f\n"),
		      m_cameraDistance, m_fieldOfView);
		return;
	}

	int centerX = rcClient.Width() / 2;
	int centerY = rcClient.Height() / 2;

	// Projection parameters (adjustable via camera controls)
	double cameraDistance = m_cameraDistance;  // Default: 300 million km
	double fov = m_fieldOfView;  // Default: 50 million km

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
	if (massCount <= 0)
		return;

	// Pre-allocate vector to avoid reallocations
	masses.reserve(static_cast<size_t>(massCount));
	for (int i = 0; i < massCount; i++)
	{
		const Mass& m = universe.GetAt(i);

		// Apply camera rotation to position
		double rotatedX = m.m_X;
		double rotatedY = m.m_Y;
		double rotatedZ = m.m_Z;

		// Apply yaw rotation (around Y-axis)
		double cosYaw = cos(m_rotationYaw);
		double sinYaw = sin(m_rotationYaw);
		double tempX = rotatedX * cosYaw - rotatedZ * sinYaw;
		double tempZ = rotatedX * sinYaw + rotatedZ * cosYaw;
		rotatedX = tempX;
		rotatedZ = tempZ;

		// Apply pitch rotation (around X-axis)
		double cosPitch = cos(m_rotationPitch);
		double sinPitch = sin(m_rotationPitch);
		double tempY = rotatedY * cosPitch - rotatedZ * sinPitch;
		rotatedZ = rotatedY * sinPitch + rotatedZ * cosPitch;
		rotatedY = tempY;

		// Apply roll rotation (around Z-axis)
		double cosRoll = cos(m_rotationRoll);
		double sinRoll = sin(m_rotationRoll);
		tempX = rotatedX * cosRoll - rotatedY * sinRoll;
		tempY = rotatedX * sinRoll + rotatedY * cosRoll;
		rotatedX = tempX;
		rotatedY = tempY;

		// Skip objects behind camera (prevent division by zero and visual glitches)
		if (rotatedZ >= cameraDistance - CAMERA_Z_BUFFER)
			continue;

		// Perspective projection with frustum culling
		double scale = fov / (cameraDistance - rotatedZ);

		// Apply pan offset (in km) scaled to screen coordinates
		// Pan offset is in km, need to convert to screen pixels using scale
		double panX = m_panOffsetX * scale;
		double panY = m_panOffsetY * scale;

		int screenX = centerX + (int)(rotatedX * scale) + (int)panX;
		int screenY = centerY - (int)(rotatedY * scale) - (int)panY;  // Invert Y for correct screen coords

		// Radius proportional to cube root of mass (volume) with mass scaling
		int radius = static_cast<int>(pow(m.m_MasseKG, 1.0/3.0) * m_massScaleFactor);
		radius = max(MIN_BODY_RADIUS, min(radius, MAX_BODY_RADIUS));

		// Color based on mass (simplified)
		COLORREF color;
		if (m.m_MasseKG > STAR_MASS_THRESHOLD)
			color = COLOR_STAR;
		else if (m.m_MasseKG > PLANET_MASS_THRESHOLD)
			color = COLOR_PLANET;
		else
			color = COLOR_ASTEROID;

		MassRenderInfo info;
		info.mass = &m;
		info.screenX = screenX;
		info.screenY = screenY;
		info.radius = radius;
		info.depth = rotatedZ;
		info.color = color;
		masses.push_back(info);
	}

	// Sort by depth (painter's algorithm - draw far objects first)
	std::sort(masses.begin(), masses.end(),
		[](const MassRenderInfo& a, const MassRenderInfo& b) {
			return a.depth > b.depth;
		});

	// Draw orbit trails first (behind bodies)
	if (m_bShowTrails)
	{
		RenderTrailForMasses(pDC, universe, centerX, centerY, cameraDistance, fov, m_panOffsetX, m_panOffsetY);
	}

	// Draw each mass with filled ellipse
	// Create a reusable brush and pen to minimize GDI object creation
	CBrush fillBrush;
	CPen outlinePen(PS_SOLID, 1, COLOR_OUTLINE);
	CPen* oldPen = pDC->SelectObject(&outlinePen);

	// Track last color to minimize brush recreation
	COLORREF lastColor = COLOR_INVALID;
	BOOL brushValid = FALSE;  // Track if brush has been created

	for (const auto& info : masses)
	{
		CRect ellipse(
			info.screenX - info.radius,
			info.screenY - info.radius,
			info.screenX + info.radius,
			info.screenY + info.radius
		);

		// Only recreate brush when color changes
		if (info.color != lastColor)
		{
			if (brushValid)
			{
				fillBrush.DeleteObject();
			}
			if (!fillBrush.CreateSolidBrush(info.color))
			{
				TRACE(_T("Failed to create brush for color %u\n"), info.color);
				continue;
			}
			brushValid = TRUE;
			lastColor = info.color;
		}

		CBrush* oldBrush = pDC->SelectObject(&fillBrush);

		// Draw filled ellipse
		pDC->Ellipse(&ellipse);

		// Restore previous brush
		pDC->SelectObject(oldBrush);
	}

	// Restore previous pen and cleanup
	pDC->SelectObject(oldPen);
	outlinePen.DeleteObject();
	if (brushValid)
	{
		fillBrush.DeleteObject();
	}
}

// Render orbit trails for all masses
void CEpistemotronView::RenderTrailForMasses(CDC* pDC, const Universe& universe,
	int centerX, int centerY, double cameraDistance, double fov,
	double panX, double panY)
{
	// Validate DC parameter
	if (pDC == nullptr)
		return;

	int massCount = universe.GetMassCount();
	if (massCount <= 0)
		return;

	// Pre-allocate screen points buffer (max trail size estimate)
	// Using vector to avoid repeated allocations and ensure exception safety
	std::vector<CPoint> screenPoints;
	screenPoints.reserve(MAX_TRAIL_POINTS);

	// Pre-calculate rotation trig values once (same for all masses and trail points)
	// This avoids calling cos/sin thousands of times per frame
	const double cosYaw = cos(m_rotationYaw);
	const double sinYaw = sin(m_rotationYaw);
	const double cosPitch = cos(m_rotationPitch);
	const double sinPitch = sin(m_rotationPitch);
	const double cosRoll = cos(m_rotationRoll);
	const double sinRoll = sin(m_rotationRoll);

	// Create three pens upfront for different body types
	CPen starPen(PS_SOLID, 1, COLOR_STAR_TRAIL);
	CPen planetPen(PS_SOLID, 1, COLOR_PLANET_TRAIL);
	CPen asteroidPen(PS_SOLID, 1, COLOR_ASTEROID_TRAIL);

	for (int i = 0; i < massCount; i++)
	{
		const Mass& m = universe.GetAt(i);

		// Skip if no trail data
		if (m.m_trail.empty())
			continue;

		// Select pen based on body type
		const CPen* selectedPen = nullptr;
		if (m.m_MasseKG > STAR_MASS_THRESHOLD)
			selectedPen = &starPen;
		else if (m.m_MasseKG > PLANET_MASS_THRESHOLD)
			selectedPen = &planetPen;
		else
			selectedPen = &asteroidPen;

		CPen* oldPen = pDC->SelectObject(const_cast<CPen*>(selectedPen));

		// Clear and convert trail points to screen coordinates
		screenPoints.clear();

		for (size_t j = 0; j < m.m_trail.size(); j++)
		{
			const TrailPoint& tp = m.m_trail[j];

			// Apply camera rotation to trail point (using pre-calculated trig values)
			double rotatedX = tp.x;
			double rotatedY = tp.y;
			double rotatedZ = tp.z;

			// Apply yaw rotation (around Y-axis)
			double tempX = rotatedX * cosYaw - rotatedZ * sinYaw;
			double tempZ = rotatedX * sinYaw + rotatedZ * cosYaw;
			rotatedX = tempX;
			rotatedZ = tempZ;

			// Apply pitch rotation (around X-axis)
			double tempY = rotatedY * cosPitch - rotatedZ * sinPitch;
			rotatedZ = rotatedY * sinPitch + rotatedZ * cosPitch;
			rotatedY = tempY;

			// Apply roll rotation (around Z-axis)
			tempX = rotatedX * cosRoll - rotatedY * sinRoll;
			tempY = rotatedX * sinRoll + rotatedY * cosRoll;
			rotatedX = tempX;
			rotatedY = tempY;

			// Skip points behind camera (with safety margin)
			if (rotatedZ >= cameraDistance - CAMERA_Z_BUFFER)
				continue;

			// Perspective projection with division-by-zero protection
			double zDistance = cameraDistance - rotatedZ;
			if (zDistance <= 0)
				continue;

			double scale = fov / zDistance;

			// Apply pan offset
			double panXScreen = panX * scale;
			double panYScreen = panY * scale;

			int screenX = centerX + static_cast<int>(rotatedX * scale) + static_cast<int>(panXScreen);
			int screenY = centerY - static_cast<int>(rotatedY * scale) - static_cast<int>(panYScreen);

			screenPoints.emplace_back(screenX, screenY);
		}

		// Draw the trail as a polyline if we have enough points
		if (screenPoints.size() >= 2)
		{
			pDC->Polyline(screenPoints.data(), static_cast<int>(screenPoints.size()));
		}

		// Restore previous pen
		pDC->SelectObject(oldPen);
	}

	// Clean up pens
	starPen.DeleteObject();
	planetPen.DeleteObject();
	asteroidPen.DeleteObject();
}

void CEpistemotronView::DrawUIOverlay(CDC* pDC, const CRect& rcClient)
{
	// Validate DC parameter
	if (pDC == nullptr)
		return;

	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
		return;

	Universe* pUniverse = pDoc->m_pCurrentUniverse;
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

	// Draw simulation stats
	statusText += _T("\nIteration: ");
	CString temp;
	temp.Format(_T("%d"), pUniverse->m_iIteration);
	statusText += temp;

	statusText += _T("\nBodies: ");
	temp.Format(_T("%d"), pUniverse->GetMassCount());
	statusText += temp;

	statusText += _T("\nStep: ");
	temp.Format(_T("%d sec"), m_stepSizeSec);
	statusText += temp;

	statusText += _T("\nSteps/frame: ");
	temp.Format(_T("%d"), m_stepsPerFrame);
	statusText += temp;

	// Draw energy conservation statistics
	double totalEnergy = pUniverse->GetTotalEnergy();
	double kineticEnergy = pUniverse->GetTotalKineticEnergy();
	double potentialEnergy = pUniverse->GetTotalPotentialEnergy();

	statusText += _T("\n---");
	statusText += _T("\nEnergy Conservation:");
	statusText += _T("\n  Total: ") + FormatScientific(totalEnergy) + _T(" J");
	statusText += _T("\n  Kinetic: ") + FormatScientific(kineticEnergy) + _T(" J");
	statusText += _T("\n  Potential: ") + FormatScientific(potentialEnergy) + _T(" J");

	// Draw momentum conservation statistics
	double linearMom = pUniverse->GetTotalLinearMomentumMagnitude();
	double angularMom = pUniverse->GetTotalAngularMomentumMagnitude();

	statusText += _T("\n---");
	statusText += _T("\nMomentum Conservation:");
	statusText += _T("\n  Linear: ") + FormatScientific(linearMom) + _T(" kg*m/s");
	statusText += _T("\n  Angular: ") + FormatScientific(angularMom) + _T(" kg*m²/s");

	// Draw camera stats
	statusText += _T("\n---");
	statusText += _T("\nCamera dist: ");
	temp.Format(_T("%.0f km"), m_cameraDistance);
	statusText += temp;

	statusText += _T("\nFOV: ");
	temp.Format(_T("%.0f km"), m_fieldOfView);
	statusText += temp;

	statusText += _T("\nPan: (");
	temp.Format(_T("%.0f, %.0f) km"), m_panOffsetX, m_panOffsetY);
	statusText += temp;

	statusText += _T("\nRotation: (");
	temp.Format(_T("%.1f, %.1f, %.1f) deg"),
		m_rotationPitch * 180.0 / PI, m_rotationYaw * 180.0 / PI, m_rotationRoll * 180.0 / PI);
	statusText += temp;

	// Draw integrator info
	statusText += _T("\n---");
	statusText += _T("\nIntegrator: ") + GetIntegratorName();

	// Draw trail status
	statusText += _T("\n---");
	statusText += _T("\nTrails: ") + CString(m_bShowTrails ? _T("ON") : _T("OFF"));

	// Draw controls help
	statusText += _T("\n---");
	statusText += _T("\nControls:");
	statusText += _T("\n  Mouse wheel: Zoom");
	statusText += _T("\n  +/- keys: Zoom");
	statusText += _T("\n  Left drag: Pan");
	statusText += _T("\n  Right drag: Rotate");
	statusText += _T("\n  Middle drag: Roll");
	statusText += _T("\n  I key: Toggle integrator");
	statusText += _T("\n  T key: Toggle trails");
	statusText += _T("\n  R key: Reset rotation");
	statusText += _T("\n  ESC: Reset camera");

	// Draw text in top-left corner
	pDC->SetTextColor(COLOR_TEXT);
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(10, 10, statusText);
}

// Helper function to format large/small numbers in scientific notation
CString CEpistemotronView::FormatScientific(double value)
{
	CString result;
	if (value >= 0)
		result.Format(_T("%.3e"), value);
	else
		result.Format(_T("-%.3e"), -value);
	return result;
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

	m_timerId = SetTimer(TIMER_ID, TIMER_INTERVAL_MS, nullptr);
	if (m_timerId == 0)
	{
		TRACE(_T("Failed to start simulation timer\n"));
		return;  // Timer failed, don't change state
	}
	m_simulationState = SimulationState::Running;
}

void CEpistemotronView::PauseSimulation()
{
	if (m_simulationState != SimulationState::Running)
		return;

	if (!KillTimer(m_timerId))
	{
		TRACE(_T("Failed to kill timer %Iu\n"), m_timerId);
	}
	m_timerId = 0;
	m_simulationState = SimulationState::Paused;
}

void CEpistemotronView::ResumeSimulation()
{
	if (m_simulationState != SimulationState::Paused)
		return;

	m_timerId = SetTimer(TIMER_ID, TIMER_INTERVAL_MS, nullptr);
	if (m_timerId == 0)
	{
		TRACE(_T("Failed to resume simulation timer\n"));
		return;  // Timer failed, don't change state
	}
	m_simulationState = SimulationState::Running;
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

	// Create new universe and load the current scenario
	pDoc->m_pCurrentUniverse = new Universe(1);  // Temporary, will be resized by LoadScenario
	pDoc->m_pCurrentUniverse->LoadScenario(m_currentScenario);
	pDoc->m_pCurrentUniverse->m_iIteration = 0;

	m_stepsPerFrame = 1;
	// Set step size based on scenario
	switch (m_currentScenario)
	{
	case ScenarioType::SolarSystem:
		m_stepSizeSec = STEP_SIZE_SOLAR_SYSTEM;
		break;
	case ScenarioType::BinaryStar:
		m_stepSizeSec = STEP_SIZE_BINARY_STAR;
		break;
	case ScenarioType::ThreeBody:
		m_stepSizeSec = STEP_SIZE_THREE_BODY;
		break;
	case ScenarioType::Galaxy:
	default:
		m_stepSizeSec = STEP_SIZE_GALAXY;
		break;
	}

	Invalidate();
}

void CEpistemotronView::StepSimulation()
{
	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
		return;

	// Generate next step using selected integrator
	Universe* pNextUniverse = nullptr;
	if (m_integratorType == IntegratorType::VelocityVerlet)
	{
		pNextUniverse = new Universe(pDoc->m_pCurrentUniverse->GetMassCount());
		pNextUniverse->SimulateFromVelocityVerlet(*pDoc->m_pCurrentUniverse, m_stepSizeSec);
	}
	else
	{
		pNextUniverse = pDoc->m_pCurrentUniverse->GenerateSimulationStep(m_stepSizeSec);
	}

	delete pDoc->m_pCurrentUniverse;
	pDoc->m_pCurrentUniverse = pNextUniverse;
	Invalidate();
}

void CEpistemotronView::SpeedUp()
{
	m_stepsPerFrame = min(m_stepsPerFrame * 2, MAX_STEPS_PER_FRAME);
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
	if (nIDEvent != TIMER_ID || m_simulationState != SimulationState::Running)
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
		// Generate next step using selected integrator
		Universe* pNextUniverse = nullptr;
		if (m_integratorType == IntegratorType::VelocityVerlet)
		{
			pNextUniverse = new Universe(pDoc->m_pCurrentUniverse->GetMassCount());
			pNextUniverse->SimulateFromVelocityVerlet(*pDoc->m_pCurrentUniverse, m_stepSizeSec);
		}
		else
		{
			pNextUniverse = pDoc->m_pCurrentUniverse->GenerateSimulationStep(m_stepSizeSec);
		}

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

void CEpistemotronView::OnSimulationConfig()
{
	CSimConfigDlg dlg(this);
	// Note: Dialog resource IDD_SIM_CONFIG_DLG must be created in Visual Studio
	// Required controls: IDC_EDIT_NUM_BODIES, IDC_EDIT_STEP_SIZE, IDC_EDIT_STEPS_PER_FRAME,
	// IDC_CHECK_RANDOM, IDC_EDIT_RANDOM_RADIUS, IDC_EDIT_DESCRIPTION, IDC_BTN_RANDOMIZE
	int result = dlg.DoModal();
	if (result == IDOK)
	{
		CSimConfigDlg::SimConfig config = dlg.GetConfig();
		m_stepSizeSec = config.stepSizeSec;
		m_stepsPerFrame = config.stepsPerFrame;
		// TODO: Apply other configuration settings (numBodies, random positions, etc.)
		Invalidate();
	}
	else if (result == -1)  // -1 indicates dialog creation failed
	{
		AfxMessageBox(_T("Configuration dialog not available.\n\n"
		                 "The dialog resource (IDD_SIM_CONFIG_DLG) needs to be created in Visual Studio.\n"
		                 "See TODO.md for details."));
	}
}

// Scenario selection implementations

void CEpistemotronView::LoadScenarioSolarSystem()
{
	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
		return;

	pDoc->m_pCurrentUniverse->LoadScenario(ScenarioType::SolarSystem);
	m_stepSizeSec = STEP_SIZE_SOLAR_SYSTEM;
	m_currentScenario = ScenarioType::SolarSystem;
}

void CEpistemotronView::LoadScenarioBinaryStar()
{
	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
		return;

	pDoc->m_pCurrentUniverse->LoadScenario(ScenarioType::BinaryStar);
	m_stepSizeSec = STEP_SIZE_BINARY_STAR;
	m_currentScenario = ScenarioType::BinaryStar;
}

void CEpistemotronView::LoadScenarioThreeBody()
{
	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
		return;

	pDoc->m_pCurrentUniverse->LoadScenario(ScenarioType::ThreeBody);
	m_stepSizeSec = STEP_SIZE_THREE_BODY;
	m_currentScenario = ScenarioType::ThreeBody;
}

void CEpistemotronView::LoadScenarioGalaxy()
{
	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
		return;

	pDoc->m_pCurrentUniverse->LoadScenario(ScenarioType::Galaxy);
	m_stepSizeSec = STEP_SIZE_GALAXY;
	m_currentScenario = ScenarioType::Galaxy;
}

void CEpistemotronView::CycleScenario()
{
	// Cycle through scenarios
	switch (m_currentScenario)
	{
	case ScenarioType::SolarSystem:
		LoadScenarioBinaryStar();
		break;
	case ScenarioType::BinaryStar:
		LoadScenarioThreeBody();
		break;
	case ScenarioType::ThreeBody:
		LoadScenarioGalaxy();
		break;
	case ScenarioType::Galaxy:
	default:
		LoadScenarioSolarSystem();
		break;
	}

	// Display current scenario name in a tooltip-like message
	CString msg;
	msg.Format(_T("Loaded scenario: %s"), GetScenarioName());
	AfxMessageBox(msg, MB_ICONINFORMATION);
}

CString CEpistemotronView::GetScenarioName() const
{
	switch (m_currentScenario)
	{
	case ScenarioType::SolarSystem:
		return _T("Solar System (Sun, Earth, Moon, Mars)");
	case ScenarioType::BinaryStar:
		return _T("Binary Star System");
	case ScenarioType::ThreeBody:
		return _T("Three-Body Problem (Figure-8 Orbit)");
	case ScenarioType::Galaxy:
		return _T("Galaxy-like (Central Black Hole + 50 Stars)");
	default:
		return _T("Unknown Scenario");
	}
}

// Scenario message handlers

void CEpistemotronView::OnScenarioSolarSystem()
{
	LoadScenarioSolarSystem();
	Invalidate();
}

void CEpistemotronView::OnScenarioBinaryStar()
{
	LoadScenarioBinaryStar();
	Invalidate();
}

void CEpistemotronView::OnScenarioThreeBody()
{
	LoadScenarioThreeBody();
	Invalidate();
}

void CEpistemotronView::OnScenarioGalaxy()
{
	LoadScenarioGalaxy();
	Invalidate();
}

void CEpistemotronView::OnScenarioNext()
{
	CycleScenario();
	Invalidate();
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

void CEpistemotronView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// End rotating if in rotation mode
	if (m_bRotating)
	{
		m_bRotating = FALSE;
		ReleaseCapture();  // Release mouse capture
	}
	else
	{
		// Show context menu
		ClientToScreen(&point);
		OnContextMenu(this, point);
	}

	CView::OnRButtonUp(nFlags, point);
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

// Camera control handlers

BOOL CEpistemotronView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 0)
	{
		// Zoom in (move camera closer)
		m_cameraDistance *= (1.0 / ZOOM_FACTOR);
		m_fieldOfView *= (1.0 / ZOOM_FACTOR);
	}
	else
	{
		// Zoom out (move camera farther)
		m_cameraDistance *= ZOOM_FACTOR;
		m_fieldOfView *= ZOOM_FACTOR;
	}

	// Clamp camera distance to reasonable bounds
	m_cameraDistance = max(MIN_CAMERA_DISTANCE_KM, min(m_cameraDistance, MAX_CAMERA_DISTANCE_KM));
	m_fieldOfView = max(MIN_FIELD_OF_VIEW_KM, min(m_fieldOfView, MAX_FIELD_OF_VIEW_KM));

	Invalidate();
	return TRUE;  // Handled
}

void CEpistemotronView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_ADD:
	case VK_OEM_PLUS:
		// Plus key: zoom in
		m_cameraDistance *= (1.0 / ZOOM_FACTOR);
		m_fieldOfView *= (1.0 / ZOOM_FACTOR);
		Invalidate();
		break;

	case VK_SUBTRACT:
	case VK_OEM_MINUS:
		// Minus key: zoom out
		m_cameraDistance *= ZOOM_FACTOR;
		m_fieldOfView *= ZOOM_FACTOR;
		Invalidate();
		break;

	case VK_ESCAPE:
		// Reset camera to default position
		m_cameraDistance = DEFAULT_CAMERA_DISTANCE_KM;
		m_fieldOfView = DEFAULT_FIELD_OF_VIEW_KM;
		m_panOffsetX = 0.0;
		m_panOffsetY = 0.0;
		m_rotationPitch = 0.0;
		m_rotationYaw = 0.0;
		m_rotationRoll = 0.0;
		Invalidate();
		break;

	case 'I':
	case 'i':
		// Toggle integrator
		if (m_integratorType == IntegratorType::SymplecticEuler)
		{
			m_integratorType = IntegratorType::VelocityVerlet;
		}
		else
		{
			m_integratorType = IntegratorType::SymplecticEuler;
		}
		Invalidate();
		break;

	case 'T':
	case 't':
		// Toggle orbit trails
		m_bShowTrails = !m_bShowTrails;
		Invalidate();
		break;

	case 'N':
	case 'n':
		// Cycle to next scenario
		CycleScenario();
		Invalidate();
		break;

	case 'R':
	case 'r':
		// Reset rotation
		m_rotationPitch = 0.0;
		m_rotationYaw = 0.0;
		m_rotationRoll = 0.0;
		Invalidate();
		break;

	default:
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
		break;
	}
}

CString CEpistemotronView::GetIntegratorName() const
{
	switch (m_integratorType)
	{
	case IntegratorType::SymplecticEuler:
		return _T("Symplectic Euler");
	case IntegratorType::VelocityVerlet:
		return _T("Velocity Verlet");
	default:
		return _T("Unknown");
	}
}

// Mouse handlers for camera panning

void CEpistemotronView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Start dragging for pan
	m_bDragging = TRUE;
	m_lastMousePos = point;
	SetCapture();  // Capture mouse events even when cursor leaves window

	CView::OnLButtonDown(nFlags, point);
}

void CEpistemotronView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// End dragging
	m_bDragging = FALSE;
	ReleaseCapture();  // Release mouse capture

	CView::OnLButtonUp(nFlags, point);
}

void CEpistemotronView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		// Calculate mouse delta in pixels
		int deltaX = point.x - m_lastMousePos.x;
		int deltaY = point.y - m_lastMousePos.y;

		// Convert pixel delta to km based on current FOV and window size
		CRect rcClient;
		GetClientRect(&rcClient);

		// Pixels per km at current zoom level
		double pixelsPerKm = rcClient.Width() / m_fieldOfView;

		// Update pan offset (negative deltaY because screen Y is inverted)
		m_panOffsetX -= deltaX / pixelsPerKm;
		m_panOffsetY += deltaY / pixelsPerKm;

		// Update last position
		m_lastMousePos = point;

		// Redraw with new pan offset
		Invalidate();
	}

	if (m_bRotating)
	{
		// Calculate mouse delta in pixels
		int deltaX = point.x - m_lastMousePos.x;
		int deltaY = point.y - m_lastMousePos.y;

		if (m_bRolling)
		{
			// Middle button: control roll (rotation around Z-axis)
			m_rotationRoll += deltaX * ROTATION_SENSITIVITY;
			m_rotationRoll = max(-MAX_ROLL_RADIANS, min(m_rotationRoll, MAX_ROLL_RADIANS));
		}
		else
		{
			// Right button: control pitch and yaw
			// deltaY controls pitch (up/down rotation around X-axis)
			m_rotationPitch += deltaY * ROTATION_SENSITIVITY;
			// deltaX controls yaw (left/right rotation around Y-axis)
			m_rotationYaw -= deltaX * ROTATION_SENSITIVITY;

			// Clamp rotation angles to prevent gimbal lock and extreme views
			m_rotationPitch = max(-MAX_PITCH_RADIANS, min(m_rotationPitch, MAX_PITCH_RADIANS));
			m_rotationYaw = max(-MAX_YAW_RADIANS, min(m_rotationYaw, MAX_YAW_RADIANS));
		}

		// Update last position
		m_lastMousePos = point;

		// Redraw with new rotation
		Invalidate();
	}

	CView::OnMouseMove(nFlags, point);
}

// Mouse handlers for camera rotation (right button)

void CEpistemotronView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// Start rotating for camera view
	m_bRotating = TRUE;
	m_lastMousePos = point;
	SetCapture();  // Capture mouse events even when cursor leaves window

	CView::OnRButtonDown(nFlags, point);
}

// Mouse handlers for camera roll (middle button)

void CEpistemotronView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// Start rotating for roll
	m_bRotating = TRUE;
	m_lastMousePos = point;
	m_bRolling = TRUE;  // Flag to indicate we're doing roll, not pitch/yaw
	SetCapture();  // Capture mouse events even when cursor leaves window

	CView::OnMButtonDown(nFlags, point);
}

void CEpistemotronView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// End rotating
	m_bRotating = FALSE;
	m_bRolling = FALSE;
	ReleaseCapture();  // Release mouse capture

	CView::OnMButtonUp(nFlags, point);
}
