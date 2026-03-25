// EpistemotronView.cpp : implementation of the CEpistemotronView class
//

#include "pch.h"
#include "MainFrm.h"  // For CMainFrame forward reference

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
constexpr int SELECTION_RING_WIDTH = 3;                  // Selection ring width in pixels

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
constexpr COLORREF COLOR_SELECTION_RING = RGB(255, 255, 0); // Selection ring (yellow)
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
	// Recording commands
	ON_COMMAND(ID_RECORDING_START, &CEpistemotronView::OnRecordingStart)
	ON_COMMAND(ID_RECORDING_STOP, &CEpistemotronView::OnRecordingStop)
	// Save/Load state commands
	ON_COMMAND(ID_STATE_SAVE, &CEpistemotronView::OnStateSave)
	ON_COMMAND(ID_STATE_LOAD, &CEpistemotronView::OnStateLoad)
	// Export commands
	ON_COMMAND(ID_EXPORT_FRAME, &CEpistemotronView::OnExportFrame)
	ON_COMMAND(ID_EXPORT_SEQUENCE, &CEpistemotronView::OnExportSequence)
	// Character input handler
	ON_WM_CHAR()
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
	, m_bEnableCollisions(TRUE)
	, m_bPauseOnCollision(FALSE)
	, m_bShowHelp(FALSE)
	, m_integratorType(IntegratorType::SymplecticEuler)
	, m_lastFrameTime(0)
	, m_frameCount(0)
	, m_currentFps(0.0)
	, m_currentScenario(ScenarioType::SolarSystem)
	, m_pOldBitmap(nullptr)
	, m_memBitmapWidth(0)
	, m_memBitmapHeight(0)
	, m_selectedBodyIndex(-1)
	, m_initialTotalEnergy(0.0)
	, m_initialLinearMomentum(0.0)
	, m_initialAngularMomentum(0.0)
	, m_totalCollisions(0)
	, m_collisionsThisFrame(0)
	, m_lastCollisionTime(0)
	, m_largestCollisionMass(0.0)
	, m_bRecording(FALSE)
	, m_collisionFlashes()
	, m_recordFrameCount(0)
{
}

CEpistemotronView::~CEpistemotronView()
{
	// Stop recording if active
	if (m_bRecording)
	{
		TRACE(_T("Stopping recording on destroy: %d frames saved\n"), m_recordFrameCount);
		m_bRecording = FALSE;
	}

	StopSimulation();  // Ensure timer is stopped

	// Clean up double buffer resources
	if (m_memBitmap.m_hObject != nullptr)
	{
		m_memBitmap.DeleteObject();
	}
	if (m_memDC.m_hDC != nullptr)
	{
		m_memDC.DeleteDC();
	}
}

BOOL CEpistemotronView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

// ============================================================================
// Collision Flash Methods
// ============================================================================

// Add a collision flash at the specified position
void CEpistemotronView::AddCollisionFlash(double x, double y, double z, COLORREF color)
{
	m_collisionFlashes.emplace_back(x, y, z, color, 3);  // Flash for 3 frames
}

// Update collision flashes (decrement remaining frames, remove expired)
void CEpistemotronView::UpdateCollisionFlashes()
{
	for (auto it = m_collisionFlashes.begin(); it != m_collisionFlashes.end(); )
	{
		if (--(it->remainingFrames) <= 0)
		{
			it = m_collisionFlashes.erase(it);
		}
		else
		{
			++it;
		}
	}
}

// Render collision flashes with 3D projection
void CEpistemotronView::RenderCollisionFlashes(CDC* pDC, int centerX, int centerY,
	double cameraDistance, double fov, double panX, double panY)
{
	if (m_collisionFlashes.empty())
		return;

	// Pre-compute trigonometric values for camera rotation
	const double cosYaw = cos(m_rotationYaw);
	const double sinYaw = sin(m_rotationYaw);
	const double cosPitch = cos(m_rotationPitch);
	const double sinPitch = sin(m_rotationPitch);
	const double cosRoll = cos(m_rotationRoll);
	const double sinRoll = sin(m_rotationRoll);

	// Create a brush for flash rendering
	CBrush flashBrush;
	CPen flashPen(PS_SOLID, 2, RGB(255, 255, 255));  // White outline

	for (const auto& flash : m_collisionFlashes)
	{
		// Apply camera rotation
		double rotatedX = flash.x;
		double rotatedY = flash.y;
		double rotatedZ = flash.z;

		// Yaw rotation (around Y-axis)
		double tempX = rotatedX * cosYaw - rotatedZ * sinYaw;
		double tempZ = rotatedX * sinYaw + rotatedZ * cosYaw;
		rotatedX = tempX;
		rotatedZ = tempZ;

		// Pitch rotation (around X-axis)
		double tempY = rotatedY * cosPitch - rotatedZ * sinPitch;
		rotatedZ = rotatedY * sinPitch + rotatedZ * cosPitch;
		rotatedY = tempY;

		// Roll rotation (around Z-axis)
		tempX = rotatedX * cosRoll - rotatedY * sinRoll;
		rotatedY = rotatedX * sinRoll + rotatedY * cosRoll;
		rotatedX = tempX;
		rotatedY = tempY;

		// Skip if behind camera
		if (rotatedZ >= cameraDistance - CAMERA_Z_BUFFER)
			continue;

		// Perspective projection
		double scale = fov / (cameraDistance - rotatedZ);

		// Apply pan offset
		int screenX = centerX + (int)(rotatedX * scale) + (int)panX;
		int screenY = centerY - (int)(rotatedY * scale) - (int)panY;

		// Flash size based on remaining frames (shrinks as it fades)
		int flashRadius = 15 * flash.remainingFrames;

		// Create flash rect (circle)
		CRect flashRect(
			screenX - flashRadius,
			screenY - flashRadius,
			screenX + flashRadius,
			screenY + flashRadius
		);

		// Create colored brush
		if (!flashBrush.CreateSolidBrush(flash.color))
			continue;

		CPen* oldPen = pDC->SelectObject(&flashPen);
		CBrush* oldBrush = pDC->SelectObject(&flashBrush);

		// Draw flash circle
		pDC->Ellipse(&flashRect);

		// Restore GDI objects
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);

		flashBrush.DeleteObject();
	}

	flashPen.DeleteObject();
}

// ============================================================================
// Double Buffering
// ============================================================================

// Double buffering helper - reuses pre-allocated DC/bitmap
BOOL CEpistemotronView::EnsureDoubleBuffer(CDC* pDC, int width, int height)
{
	// Check if we need to create or resize the buffer
	BOOL needResize = (m_memBitmapWidth != width || m_memBitmapHeight != height);

	if (needResize)
	{
		// Clean up existing resources
		if (m_pOldBitmap != nullptr)
		{
			m_memDC.SelectObject(m_pOldBitmap);
			m_pOldBitmap = nullptr;
		}
		if (m_memBitmap.m_hObject != nullptr)
		{
			m_memBitmap.DeleteObject();
		}
		if (m_memDC.m_hDC != nullptr)
		{
			m_memDC.DeleteDC();
		}

		// Create new DC and bitmap
		if (!m_memDC.CreateCompatibleDC(pDC))
		{
			TRACE(_T("Failed to create compatible DC for double buffering\n"));
			return FALSE;
		}

		if (!m_memBitmap.CreateCompatibleBitmap(pDC, width, height))
		{
			TRACE(_T("Failed to create compatible bitmap for double buffering\n"));
			m_memDC.DeleteDC();
			return FALSE;
		}

		m_memBitmapWidth = width;
		m_memBitmapHeight = height;
	}

	// Select bitmap into DC
	m_pOldBitmap = m_memDC.SelectObject(&m_memBitmap);
	if (m_pOldBitmap == nullptr)
	{
		TRACE(_T("Failed to select bitmap into DC\n"));
		return FALSE;
	}

	return TRUE;
}

// CEpistemotronView drawing

void CEpistemotronView::OnDraw(CDC* pDC)
{
	// Validate DC parameter
	if (pDC == nullptr)
		return;

	// FPS calculation
	UINT64 currentTime = GetTickCount64();
	m_frameCount++;
	if (m_lastFrameTime > 0)
	{
		UINT64 elapsed = currentTime - m_lastFrameTime;
		if (elapsed >= 1000)  // Calculate FPS every second
		{
			m_currentFps = static_cast<double>(m_frameCount) * 1000.0 / static_cast<double>(elapsed);
			m_frameCount = 0;
			m_lastFrameTime = currentTime;
		}
	}
	else
	{
		m_lastFrameTime = currentTime;
	}

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

	// Double buffering to prevent flickering (reuses pre-allocated DC/bitmap)
	if (!EnsureDoubleBuffer(pDC, width, height))
	{
		return;  // Error already traced in EnsureDoubleBuffer
	}

	// Draw background (space - dark blue/black)
	m_memDC.FillSolidRect(&rcClient, COLOR_SPACE_BG);

	// Get the current universe from the document
	Universe* pUniverse = pDoc->m_pCurrentUniverse;
	if (!pUniverse)
	{
		m_memDC.SetTextAlign(TA_CENTER);
		CString strText = _T("No simulation active.\nPress F5 or use Simulation menu to start.");
		m_memDC.TextOut(rcClient.CenterPoint().x, rcClient.CenterPoint().y, strText);
	}
	else
	{
		// Update collision flash effects
		UpdateCollisionFlashes();

		// Draw 3D projection of the universe
		RenderUniverse3D(&m_memDC, *pUniverse, rcClient);

		// Draw UI overlay
		DrawUIOverlay(&m_memDC, rcClient);
	}

	// Blit to screen
	pDC->BitBlt(0, 0, width, height, &m_memDC, 0, 0, SRCCOPY);

	// Save current frame if recording
	if (m_bRecording)
	{
		SaveCurrentFrame();
	}

	// Restore original bitmap (don't delete - reuse next frame)
	if (m_pOldBitmap != nullptr)
	{
		m_memDC.SelectObject(m_pOldBitmap);
		m_pOldBitmap = nullptr;
	}
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
		int index;        // Index in universe for selection
		double depth;
		COLORREF color;
	};
	std::vector<MassRenderInfo> masses;

	int massCount = universe.GetMassCount();
	if (massCount <= 0)
		return;

	// Pre-compute trigonometric values for camera rotation (outside loop)
	const double cosYaw = cos(m_rotationYaw);
	const double sinYaw = sin(m_rotationYaw);
	const double cosPitch = cos(m_rotationPitch);
	const double sinPitch = sin(m_rotationPitch);
	const double cosRoll = cos(m_rotationRoll);
	const double sinRoll = sin(m_rotationRoll);

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
		info.index = i;
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

		// Draw selection ring for selected body
		if (info.index == m_selectedBodyIndex)
		{
			CRect selectionRing(
				info.screenX - info.radius - SELECTION_RING_WIDTH,
				info.screenY - info.radius - SELECTION_RING_WIDTH,
				info.screenX + info.radius + SELECTION_RING_WIDTH,
				info.screenY + info.radius + SELECTION_RING_WIDTH
			);
			CPen selectionPen(PS_SOLID, SELECTION_RING_WIDTH, COLOR_SELECTION_RING);
			CPen* oldSelPen = pDC->SelectObject(&selectionPen);
			CBrush nullBrush;
			nullBrush.CreateSolidBrush(RGB(0, 0, 0));
			CBrush* oldSelBrush = pDC->SelectObject(&nullBrush);
			pDC->Ellipse(&selectionRing);
			pDC->SelectObject(oldSelBrush);
			nullBrush.DeleteObject();
			pDC->SelectObject(oldSelPen);
			selectionPen.DeleteObject();
		}
	}

	// Restore previous pen and cleanup
	pDC->SelectObject(oldPen);
	outlinePen.DeleteObject();
	if (brushValid)
	{
		fillBrush.DeleteObject();
	}

	// Render collision flash effects (on top of everything)
	RenderCollisionFlashes(pDC, centerX, centerY, cameraDistance, fov, m_panOffsetX, m_panOffsetY);
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

	// Draw FPS
	statusText += _T("\nFPS: ");
	CString fpsStr;
	fpsStr.Format(_T("%.1f"), m_currentFps);
	statusText += fpsStr;

	// Draw simulation stats
	statusText += _T("\nIteration: ");
	CString temp;
	temp.Format(_T("%d"), pUniverse->m_iIteration);
	statusText += temp;

	statusText += _T("\nBodies: ");
	temp.Format(_T("%d"), pUniverse->GetMassCount());
	statusText += temp;

	statusText += _T("\nCollisions: ");
	temp.Format(_T("%d"), pUniverse->GetTotalCollisions());
	statusText += temp;

	// Show collision status
	statusText += _T(" [");
	statusText += m_bEnableCollisions ? _T("ON") : _T("OFF");
	statusText += _T("]");

	// Show time since last collision
	if (m_lastCollisionTime > 0 && m_bEnableCollisions)
	{
		UINT64 secondsSince = (GetTickCount64() - m_lastCollisionTime) / 1000;
		if (secondsSince < 60)
		{
			statusText += _T(" (");
			CString lastStr;
			lastStr.Format(_T("%llum ago"), secondsSince);
			statusText += lastStr;
			statusText += _T(")");
		}
	}

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
	statusText += _T("\nEnergy:");
	statusText += _T("\n  Kinetic:   ") + FormatScientific(kineticEnergy) + _T(" J");
	statusText += _T("\n  Potential: ") + FormatScientific(potentialEnergy) + _T(" J");
	statusText += _T("\n  Total:     ") + FormatScientific(totalEnergy) + _T(" J");

	// Calculate and display drift percentage
	double driftPercent = 0.0;
	if (m_initialTotalEnergy != 0.0)
	{
		driftPercent = (totalEnergy - m_initialTotalEnergy) / std::fabs(m_initialTotalEnergy) * 100.0;
	}
	CString driftStr;
	if (driftPercent >= 0)
	{
		driftStr.Format(_T("+%.4f%%"), driftPercent);
	}
	else
	{
		driftStr.Format(_T("%.4f%%"), driftPercent);
	}
	statusText += _T("\n  Drift:     ") + driftStr;

	// Draw momentum conservation statistics
	double linearMom = pUniverse->GetTotalLinearMomentumMagnitude();
	double angularMom = pUniverse->GetTotalAngularMomentumMagnitude();

	statusText += _T("\n---");
	statusText += _T("\nMomentum Conservation:");
	statusText += _T("\n  Linear: ") + FormatScientific(linearMom) + _T(" kg*m/s");
	statusText += _T("\n  Angular: ") + FormatScientific(angularMom) + _T(" kg*m²/s");

	// Calculate and display momentum drift percentages
	if (m_initialLinearMomentum != 0.0)
	{
		double linearDrift = (linearMom - m_initialLinearMomentum) / std::fabs(m_initialLinearMomentum) * 100.0;
		CString linearDriftStr;
		linearDriftStr.Format(_T("%+.4f%%"), linearDrift);
		statusText += _T("\n  Linear Drift: ") + linearDriftStr;
	}

	if (m_initialAngularMomentum != 0.0)
	{
		double angularDrift = (angularMom - m_initialAngularMomentum) / std::fabs(m_initialAngularMomentum) * 100.0;
		CString angularDriftStr;
		angularDriftStr.Format(_T("%+.4f%%"), angularDrift);
		statusText += _T("\n  Angular Drift: ") + angularDriftStr;
	}

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

	// Draw recording status
	statusText += _T("\n---");
	statusText += GetRecordStatus();

	// Draw selected body info
	if (m_selectedBodyIndex >= 0)
	{
		statusText += _T("\n---");
		statusText += m_selectedBodyInfo;
	}

	// Draw controls help
	statusText += _T("\n---");
	statusText += _T("\nControls:");
	statusText += _T("\n  Mouse wheel: Zoom");
	statusText += _T("\n  +/- keys: Zoom");
	statusText += _T("\n  Left drag: Pan");
	statusText += _T("\n  Right drag: Rotate");
	statusText += _T("\n  Middle drag: Roll");
	statusText += _T("\n  Space: Pause/Resume");
	statusText += _T("\n  S: Reset simulation");
	statusText += _T("\n  I: Toggle integrator");
	statusText += _T("\n  T: Toggle trails");
	statusText += _T("\n  N: Next scenario");
	statusText += _T("\n  R: Reset rotation");
	statusText += _T("\n  E: Toggle recording");
	statusText += _T("\n  1-4: Scenario select");
	statusText += _T("\n  ESC: Reset camera");
	statusText += _T("\n  F1/? : This help");
	statusText += _T("\n  Left click: Select body");
	statusText += _T("\n  Ctrl+click: Deselect");

	// Draw text in top-left corner
	pDC->SetTextColor(COLOR_TEXT);
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(10, 10, statusText);

	// Draw expanded help overlay if enabled
	if (m_bShowHelp)
	{
		const int helpX = rcClient.left + 10;
		const int helpY = rcClient.top + 400;  // Below the main status
		const int helpWidth = 400;
		const int helpHeight = 300;

		// Draw semi-transparent background
		CRect helpRect(helpX, helpY, helpX + helpWidth, helpY + helpHeight);
		CBrush brush(RGB(0, 0, 0));
		CPen pen(PS_SOLID, 2, RGB(255, 255, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);
		pDC->FillRect(helpRect, &brush);
		pDC->Rectangle(helpRect);
		pDC->SelectObject(pOldPen);

		// Draw help content
		CString helpText;
		helpText += _T("Keyboard Shortcuts");
		helpText += _T("\n==================");
		helpText += _T("\n\nSimulation Control:");
		helpText += _T("\n  F5       Start");
		helpText += _T("\n  F6       Pause");
		helpText += _T("\n  F7       Resume");
		helpText += _T("\n  F8       Stop");
		helpText += _T("\n  F9       Reset");
		helpText += _T("\n  F10      Step");
		helpText += _T("\n  Space    Pause/Resume");
		helpText += _T("\n  +/-      Speed up/down");
		helpText += _T("\n\nScenarios:");
		helpText += _T("\n  1        Solar System");
		helpText += _T("\n  2        Binary Star");
		helpText += _T("\n  3        Three Body");
		helpText += _T("\n  4        Galaxy");
		helpText += _T("\n  N        Next scenario");
		helpText += _T("\n\nCamera:");
		helpText += _T("\n  Wheel    Zoom in/out");
		helpText += _T("\n  +/-      Zoom in/out");
		helpText += _T("\n  L-Drag   Pan");
		helpText += _T("\n  R-Drag   Rotate");
		helpText += _T("\n  M-Drag   Roll");
		helpText += _T("\n  R        Reset rotation");
		helpText += _T("\n  ESC      Reset camera");
		helpText += _T("\n\nDisplay:");
		helpText += _T("\n  I        Toggle integrator");
		helpText += _T("\n  T        Toggle trails");
		helpText += _T("\n  F1/?     This help");
		helpText += _T("\n\nCollision Control:");
		helpText += _T("\n  C        Toggle collision detection");
		helpText += _T("\n  P        Pause on collision");
		helpText += _T("\n  V        Clear collision flashes");
		helpText += _T("\n\nSelection:");
		helpText += _T("\n  L-Click  Select body");
		helpText += _T("\n  Ctrl+Click Deselect");
		helpText += _T("\n\nRecording:");
		helpText += _T("\n  E        Toggle recording");
		helpText += _T("\n\nState:");
		helpText += _T("\n  Menu     Save/Load State");

		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->TextOut(helpX + 10, helpY + 10, helpText);
	}
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

// Calculate orbital period for a selected body using Kepler's third law
// For N-body systems, finds the most massive body and treats it as the primary
// Set status bar message via main frame
void CEpistemotronView::SetStatusBarMessage(LPCTSTR message)
{
	CFrameWnd* pFrame = GetParentFrame();
	if (pFrame != nullptr)
	{
		CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(pFrame);
		if (pMainFrame != nullptr)
		{
			pMainFrame->SetStatusBarMessage(message);
		}
	}
}

// Calculate orbital period for a selected body using Kepler's third law
CString CEpistemotronView::CalculateOrbitalPeriod(const Mass& body, const Universe& universe, int bodyIndex)
{
	constexpr double G = 6.67e-11;  // Gravitational constant (N*m²/kg²)
	constexpr double KM_TO_M = 1000.0;

	CString result;

	int massCount = universe.GetMassCount();
	if (massCount < 2)
	{
		return _T("N/A (need 2+ bodies)");
	}

	// Find the most massive body that's not the selected one
	int primaryIndex = -1;
	double maxMass = 0.0;

	for (int i = 0; i < massCount; i++)
	{
		if (i == bodyIndex) continue;
		const Mass& other = universe.GetAt(i);
		if (other.m_MasseKG > maxMass)
		{
			maxMass = other.m_MasseKG;
			primaryIndex = i;
		}
	}

	if (primaryIndex < 0)
	{
		return _T("N/A (no primary found)");
	}

	const Mass& primary = universe.GetAt(primaryIndex);

	// Calculate distance between bodies (convert km to m)
	double dx = (primary.m_X - body.m_X) * KM_TO_M;
	double dy = (primary.m_Y - body.m_Y) * KM_TO_M;
	double dz = (primary.m_Z - body.m_Z) * KM_TO_M;
	double distance = sqrt(dx * dx + dy * dy + dz * dz);

	// Calculate relative velocity (m/s)
	double dvx = body.m_VitesseX - primary.m_VitesseX;
	double dvy = body.m_VitesseY - primary.m_VitesseY;
	double dvz = body.m_VitesseZ - primary.m_VitesseZ;
	double relativeVel = sqrt(dvx * dvx + dvy * dvy + dvz * dvz);

	// Total mass of the system
	double totalMass = body.m_MasseKG + primary.m_MasseKG;

	// Semi-major axis from vis-viva equation: v² = GM(2/r - 1/a)
	// Rearranged: a = 1 / (2/r - v²/GM)
	double mu = G * totalMass;  // Standard gravitational parameter
	double vSquared = relativeVel * relativeVel;
	double r = distance;

	if (r <= 0)
	{
		return _T("N/A (zero distance)");
	}

	double invASemiMajor = 2.0 / r - vSquared / mu;
	if (std::fabs(invASemiMajor) < 1e-20)
	{
		return _T("Parabolic (escape trajectory)");
	}

	double semiMajorAxis = 1.0 / invASemiMajor;

	if (semiMajorAxis < 0)
	{
		return _T("Hyperbolic (escape trajectory)");
	}

	// Orbital period from Kepler's third law: T = 2π * sqrt(a³ / GM)
	double periodSeconds = 2.0 * 3.14159265358979 * sqrt(semiMajorAxis * semiMajorAxis * semiMajorAxis / mu);

	// Format the result in appropriate units
	if (periodSeconds < 60.0)
	{
		result.Format(_T("%.1f seconds"), periodSeconds);
	}
	else if (periodSeconds < 3600.0)
	{
		result.Format(_T("%.1f minutes"), periodSeconds / 60.0);
	}
	else if (periodSeconds < 86400.0)
	{
		result.Format(_T("%.2f hours"), periodSeconds / 3600.0);
	}
	else if (periodSeconds < 31536000.0)
	{
		result.Format(_T("%.2f days"), periodSeconds / 86400.0);
	}
	else
	{
		result.Format(_T("%.2f years"), periodSeconds / 31536000.0);
	}

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

	// Initialize reference energy and momentum for drift calculation
	m_initialTotalEnergy = pDoc->m_pCurrentUniverse->GetTotalEnergy();
	m_initialLinearMomentum = pDoc->m_pCurrentUniverse->GetTotalLinearMomentumMagnitude();
	m_initialAngularMomentum = pDoc->m_pCurrentUniverse->GetTotalAngularMomentumMagnitude();

	// Reset collision statistics
	m_totalCollisions = 0;
	m_collisionsThisFrame = 0;
	m_lastCollisionTime = 0;
	m_largestCollisionMass = 0.0;
	m_collisionFlashes.clear();

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

		// Set collision toggle from view
		pNextUniverse->m_bEnableCollisions = m_bEnableCollisions;

		// Consume collision events and add visual flashes
		auto collisionEvents = pNextUniverse->ConsumeLastCollisions();
		if (!collisionEvents.empty())
		{
			m_collisionsThisFrame = static_cast<int>(collisionEvents.size());
			m_lastCollisionTime = GetTickCount64();

			// Track largest collision mass
			if (pNextUniverse->GetLargestCollisionMass() > m_largestCollisionMass)
			{
				m_largestCollisionMass = pNextUniverse->GetLargestCollisionMass();
			}

			// Pause on collision if enabled
			if (m_bPauseOnCollision)
			{
				PauseSimulation();
				TRACE(_T("Paused due to collision\n"));
				break;  // Exit loop to stop simulation
			}

			for (const auto& event : collisionEvents)
			{
				AddCollisionFlash(event.x, event.y, event.z, event.color);
			}
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
	INT_PTR result = dlg.DoModal();
	if (result == IDOK)
	{
		CSimConfigDlg::SimConfig config = dlg.GetConfig();
		m_stepSizeSec = config.stepSizeSec;
		m_stepsPerFrame = config.stepsPerFrame;
		// TODO: Apply other configuration settings (numBodies, random positions, etc.)
		Invalidate();
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

	// Initialize reference energy and momentum for drift calculation
	m_initialTotalEnergy = pDoc->m_pCurrentUniverse->GetTotalEnergy();
	m_initialLinearMomentum = pDoc->m_pCurrentUniverse->GetTotalLinearMomentumMagnitude();
	m_initialAngularMomentum = pDoc->m_pCurrentUniverse->GetTotalAngularMomentumMagnitude();
}

void CEpistemotronView::LoadScenarioBinaryStar()
{
	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
		return;

	pDoc->m_pCurrentUniverse->LoadScenario(ScenarioType::BinaryStar);
	m_stepSizeSec = STEP_SIZE_BINARY_STAR;
	m_currentScenario = ScenarioType::BinaryStar;

	// Initialize reference energy and momentum for drift calculation
	m_initialTotalEnergy = pDoc->m_pCurrentUniverse->GetTotalEnergy();
	m_initialLinearMomentum = pDoc->m_pCurrentUniverse->GetTotalLinearMomentumMagnitude();
	m_initialAngularMomentum = pDoc->m_pCurrentUniverse->GetTotalAngularMomentumMagnitude();
}

void CEpistemotronView::LoadScenarioThreeBody()
{
	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
		return;

	pDoc->m_pCurrentUniverse->LoadScenario(ScenarioType::ThreeBody);
	m_stepSizeSec = STEP_SIZE_THREE_BODY;
	m_currentScenario = ScenarioType::ThreeBody;

	// Initialize reference energy and momentum for drift calculation
	m_initialTotalEnergy = pDoc->m_pCurrentUniverse->GetTotalEnergy();
	m_initialLinearMomentum = pDoc->m_pCurrentUniverse->GetTotalLinearMomentumMagnitude();
	m_initialAngularMomentum = pDoc->m_pCurrentUniverse->GetTotalAngularMomentumMagnitude();
}

void CEpistemotronView::LoadScenarioGalaxy()
{
	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
		return;

	pDoc->m_pCurrentUniverse->LoadScenario(ScenarioType::Galaxy);
	m_stepSizeSec = STEP_SIZE_GALAXY;
	m_currentScenario = ScenarioType::Galaxy;

	// Initialize reference energy and momentum for drift calculation
	m_initialTotalEnergy = pDoc->m_pCurrentUniverse->GetTotalEnergy();
	m_initialLinearMomentum = pDoc->m_pCurrentUniverse->GetTotalLinearMomentumMagnitude();
	m_initialAngularMomentum = pDoc->m_pCurrentUniverse->GetTotalAngularMomentumMagnitude();
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

	case VK_F1:
		// Toggle help overlay
		m_bShowHelp = !m_bShowHelp;
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

	case 'S':
	case 's':
		// Simulation reset (different from rotation reset)
		ResetSimulation();
		Invalidate();
		break;

	case 'E':
	case 'e':
		// Toggle recording
		if (m_bRecording)
		{
			StopRecording();
		}
		else
		{
			CString path;
			path = _T(".\\recordings");
			StartRecording(path, _T("frame"));
		}
		Invalidate();
		break;

	default:
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
		break;
	}
}

void CEpistemotronView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL handled = FALSE;

	switch (nChar)
	{
	case VK_SPACE:
		// Space: pause/resume simulation
		if (m_simulationState == SimulationState::Running)
		{
			PauseSimulation();
		}
		else if (m_simulationState == SimulationState::Paused)
		{
			ResumeSimulation();
		}
		Invalidate();
		handled = TRUE;
		break;

	case '1':
		LoadScenarioSolarSystem();
		Invalidate();
		handled = TRUE;
		break;

	case '2':
		LoadScenarioBinaryStar();
		Invalidate();
		handled = TRUE;
		break;

	case '3':
		LoadScenarioThreeBody();
		Invalidate();
		handled = TRUE;
		break;

	case '4':
		LoadScenarioGalaxy();
		Invalidate();
		handled = TRUE;
		break;

	case '?':
		// Toggle help overlay
		m_bShowHelp = !m_bShowHelp;
		Invalidate();
		handled = TRUE;
		break;

	case 'C':
	case 'c':
		{
		// Toggle collision detection
		m_bEnableCollisions = !m_bEnableCollisions;
		CString statusMsg = m_bEnableCollisions ? _T("Collision detection: ON") : _T("Collision detection: OFF");
		SetStatusBarMessage(statusMsg);
		TRACE(_T("%s\n"), statusMsg);
		Invalidate();
		handled = TRUE;
		}
		break;

	case 'P':
	case 'p':
		{
		// Toggle pause on collision
		m_bPauseOnCollision = !m_bPauseOnCollision;
		CString pauseStatus = m_bPauseOnCollision ? _T("Pause on collision: ON") : _T("Pause on collision: OFF");
		SetStatusBarMessage(pauseStatus);
		TRACE(_T("%s\n"), pauseStatus);
		Invalidate();
		handled = TRUE;
		}
		break;

	case 'V':
	case 'v':
		{
		// Clear collision visual effects
		m_collisionFlashes.clear();
		SetStatusBarMessage(_T("Collision flashes cleared"));
		Invalidate();
		handled = TRUE;
		}
		break;

	default:
		CView::OnChar(nChar, nRepCnt, nFlags);
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
	// Check for Ctrl key to deselect
	if ((nFlags & MK_CONTROL) != 0)
	{
		m_selectedBodyIndex = -1;
		m_selectedBodyInfo.Empty();
		RefreshView();
		CView::OnLButtonDown(nFlags, point);
		return;
	}

	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
	{
		CView::OnLButtonDown(nFlags, point);
		return;
	}

	Universe* pUniverse = pDoc->m_pCurrentUniverse;
	CRect rcClient;
	GetClientRect(&rcClient);

	int centerX = rcClient.Width() / 2;
	int centerY = rcClient.Height() / 2;

	// Pre-compute trigonometric values for camera rotation
	const double cosYaw = cos(m_rotationYaw);
	const double sinYaw = sin(m_rotationYaw);
	const double cosPitch = cos(m_rotationPitch);
	const double sinPitch = sin(m_rotationPitch);
	const double cosRoll = cos(m_rotationRoll);
	const double sinRoll = sin(m_rotationRoll);

	// Try to find a clicked body
	int clickedIndex = -1;
	int bestRadius = 0;

	int massCount = pUniverse->GetMassCount();
	for (int i = 0; i < massCount; i++)
	{
		const Mass& m = pUniverse->GetAt(i);

		// Apply camera rotation to position
		double rotatedX = m.m_X;
		double rotatedY = m.m_Y;
		double rotatedZ = m.m_Z;

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
		rotatedY = rotatedX * sinRoll + rotatedY * cosRoll;
		rotatedX = tempX;
		rotatedY = tempY;

		// Skip objects behind camera
		if (rotatedZ >= m_cameraDistance - CAMERA_Z_BUFFER)
			continue;

		// Perspective projection
		double scale = m_fieldOfView / (m_cameraDistance - rotatedZ);

		// Apply pan offset
		double panX = m_panOffsetX * scale;
		double panY = m_panOffsetY * scale;

		int screenX = centerX + (int)(rotatedX * scale) + (int)panX;
		int screenY = centerY - (int)(rotatedY * scale) - (int)panY;

		// Calculate body radius
		int radius = static_cast<int>(pow(m.m_MasseKG, 1.0 / 3.0) * m_massScaleFactor);
		radius = max(MIN_BODY_RADIUS, min(radius, MAX_BODY_RADIUS));

		// Check if click is within body
		int dx = point.x - screenX;
		int dy = point.y - screenY;
		double dist = sqrt(dx * dx + dy * dy);

		if (dist <= radius && radius > bestRadius)
		{
			clickedIndex = i;
			bestRadius = radius;
		}
	}

	// Update selection
	if (clickedIndex >= 0 && clickedIndex != m_selectedBodyIndex)
	{
		m_selectedBodyIndex = clickedIndex;
		const Mass& selected = pUniverse->GetAt(clickedIndex);
		double kineticEnergy = selected.GetKineticEnergy();
		CString orbitalPeriod = CalculateOrbitalPeriod(selected, *pUniverse, clickedIndex);

		m_selectedBodyInfo.Format(
			_T("=== Selected Body ===\n")
			_T("Mass: %s kg\n")
			_T("Position: (%.2f, %.2f, %.2f) km\n")
			_T("Velocity: (%.2f, %.2f, %.2f) m/s\n")
			_T("Kinetic Energy: %s J\n")
			_T("Orbital Period: %s"),
			FormatScientific(selected.m_MasseKG),
			selected.m_X, selected.m_Y, selected.m_Z,
			selected.m_VitesseX, selected.m_VitesseY, selected.m_VitesseZ,
			FormatScientific(kineticEnergy),
			orbitalPeriod
		);
	}
	else if (clickedIndex == -1 && m_selectedBodyIndex >= 0)
	{
		// Clicked in empty space, deselect
		m_selectedBodyIndex = -1;
		m_selectedBodyInfo.Empty();
	}

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


// ============================================================================
// Recording implementation
// ============================================================================

void CEpistemotronView::StartRecording(LPCTSTR path, LPCTSTR prefix)
{
	// Create directory if it doesn't exist
	if (GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES)
	{
		if (!CreateDirectory(path, nullptr))
		{
			TRACE(_T("Failed to create recording directory: %s\n"), path);
			AfxMessageBox(_T("Failed to create recording directory.\nPlease check the path and try again."), MB_ICONERROR);
			return;
		}
	}

	// Initialize recording state
	m_recordPath = path;
	m_recordPrefix = prefix;
	m_recordFrameCount = 0;
	m_bRecording = TRUE;

	TRACE(_T("Recording started: path=%s, prefix=%s\n"), path, prefix);
}

void CEpistemotronView::StopRecording()
{
	m_bRecording = FALSE;
	m_recordPath.Empty();
	m_recordPrefix.Empty();

	TRACE(_T("Recording stopped after %d frames\n"), m_recordFrameCount);
}

CString CEpistemotronView::GetRecordStatus() const
{
	if (!m_bRecording)
	{
		return _T("Recording: OFF");
	}
	CString status;
	status.Format(_T("Recording: ON (%d frames)"), m_recordFrameCount);
	return status;
}

void CEpistemotronView::SaveCurrentFrame()
{
	if (!m_bRecording || m_memBitmap.m_hObject == nullptr)
	{
		return;
	}

	// Generate filename with sequential numbering (prefix_00001.bmp)
	CString filename;
	filename.Format(_T("%s\\%s_%05d.bmp"), m_recordPath, m_recordPrefix, m_recordFrameCount + 1);

	// Get bitmap dimensions
	BITMAP bmpInfo;
	if (!m_memBitmap.GetBitmap(&bmpInfo))
	{
		TRACE(_T("Failed to get bitmap info for frame %d\n"), m_recordFrameCount + 1);
		return;
	}

	int width = bmpInfo.bmWidth;
	int height = bmpInfo.bmHeight;

	// Create a compatible DC to get the DIB
	CDC* pSrcDC = CDC::FromHandle(m_memDC.m_hDC);
	if (pSrcDC == nullptr)
	{
		TRACE(_T("Failed to get DC for frame %d\n"), m_recordFrameCount + 1);
		return;
	}

	// Prepare BITMAPINFO structure to get DIB data
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;  // Negative for top-down DIB
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;

	// Allocate buffer for pixel data
	DWORD dwBytesRequired = ((width * 3 + 3) & ~3) * height;  // Row-aligned
	BYTE* pBits = new BYTE[dwBytesRequired];
	if (pBits == nullptr)
	{
		TRACE(_T("Failed to allocate buffer for frame %d\n"), m_recordFrameCount + 1);
		return;
	}

	// Get the DIB bits from the bitmap
	HDC hdc = m_memDC.GetSafeHdc();
	int lines = ::GetDIBits(hdc, m_memBitmap, 0, height, pBits, &bmi, DIB_RGB_COLORS);
	if (lines <= 0)
	{
		TRACE(_T("Failed to get DIB bits for frame %d\n"), m_recordFrameCount + 1);
		delete[] pBits;
		return;
	}

	// Calculate actual DIB size
	DWORD dibSize = sizeof(BITMAPINFOHEADER) + dwBytesRequired;

	// Write BMP file
	CFile file;
	if (file.Open(filename, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate))
	{
		// BMP file header
		BITMAPFILEHEADER bfh;
		bfh.bfType = 0x4D42;  // 'BM'
		bfh.bfSize = dibSize + sizeof(BITMAPFILEHEADER);
		bfh.bfReserved1 = 0;
		bfh.bfReserved2 = 0;
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		file.Write(&bfh, sizeof(BITMAPFILEHEADER));

		// Write DIB header
		file.Write(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));

		// Write pixel data
		file.Write(pBits, dwBytesRequired);

		file.Close();
		TRACE(_T("Saved frame %d to %s\n"), m_recordFrameCount + 1, filename);
	}
	else
	{
		TRACE(_T("Failed to open file for writing frame %d: %s\n"), m_recordFrameCount + 1, filename);
	}

	delete[] pBits;

	// Increment frame counter
	m_recordFrameCount++;
}

void CEpistemotronView::OnRecordingStart()
{
	// Default recording path: current directory with "Frames" subfolder
	CString defaultPath;
	GetModuleFileName(nullptr, defaultPath.GetBuffer(MAX_PATH), MAX_PATH);
	defaultPath.ReleaseBuffer();
	defaultPath = defaultPath.Left(defaultPath.ReverseFind('\\'));
	defaultPath += _T("\\Frames");

	// Default prefix
	CString defaultPrefix = _T("frame");

	// Start recording with defaults
	StartRecording(defaultPath, defaultPrefix);

	// Show confirmation
	CString msg;
	msg.Format(_T("Recording started.\nFrames will be saved to:\n%s\n\nPress Stop Recording when done."), defaultPath);
	AfxMessageBox(msg, MB_ICONINFORMATION);
}

void CEpistemotronView::OnRecordingStop()
{
	if (!m_bRecording)
	{
		AfxMessageBox(_T("Recording is not active."), MB_ICONINFORMATION);
		return;
	}

	StopRecording();

	CString msg;
	msg.Format(_T("Recording stopped.\nTotal frames saved: %d"), m_recordFrameCount);
	AfxMessageBox(msg, MB_ICONINFORMATION);
}

// ============================================================================
// Save/Load State Handlers
// ============================================================================

void CEpistemotronView::OnStateSave()
{
	CEpistemotronDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pCurrentUniverse)
	{
		SetStatusBarMessage(_T("No simulation active to save"));
		AfxMessageBox(_T("No simulation active to save."), MB_ICONWARNING);
		return;
	}

	SetStatusBarMessage(_T("Select file to save simulation state..."));

	// Create file save dialog
	CFileDialog saveDlg(FALSE, _T("esp"), _T("simulation.esp"),
		OFN_OVERWRITEPROMPT,
		_T("Epistemotron Save File (*.esp)|*.esp|All Files (*.*)|*.*||"),
		this);

	if (saveDlg.DoModal() == IDOK)
	{
		CString filepath = saveDlg.GetPathName();

		if (pDoc->m_pCurrentUniverse->SaveState(filepath))
		{
			CString msg;
			msg.Format(_T("Saved: %s"), filepath);
			SetStatusBarMessage(msg);
			AfxMessageBox(msg, MB_ICONINFORMATION);
		}
		else
		{
			SetStatusBarMessage(_T("Failed to save simulation state"));
			AfxMessageBox(_T("Failed to save simulation state."), MB_ICONERROR);
		}
	}
}

void CEpistemotronView::OnStateLoad()
{
	SetStatusBarMessage(_T("Select file to load simulation state..."));

	// Create file open dialog
	CFileDialog openDlg(TRUE, _T("esp"), nullptr,
		0,
		_T("Epistemotron Save File (*.esp)|*.esp|All Files (*.*)|*.*||"),
		this);

	if (openDlg.DoModal() == IDOK)
	{
		CString filepath = openDlg.GetPathName();

		CEpistemotronDoc* pDoc = GetDocument();
		if (!pDoc)
		{
			SetStatusBarMessage(_T("No document available"));
			AfxMessageBox(_T("No document available."), MB_ICONERROR);
			return;
		}

		SetStatusBarMessage(_T("Loading simulation state..."));

		// Create new universe and load state
		Universe* pNewUniverse = new Universe(1);
		if (pNewUniverse->LoadState(filepath))
		{
			// Replace current universe
			if (pDoc->m_pCurrentUniverse)
			{
				delete pDoc->m_pCurrentUniverse;
			}
			pDoc->m_pCurrentUniverse = pNewUniverse;

			// Reset collision statistics for the loaded state
			m_totalCollisions = pNewUniverse->GetTotalCollisions();
			m_largestCollisionMass = pNewUniverse->GetLargestCollisionMass();
			m_lastCollisionTime = 0;

			// Initialize reference energy and momentum
			m_initialTotalEnergy = pNewUniverse->GetTotalEnergy();
			m_initialLinearMomentum = pNewUniverse->GetTotalLinearMomentumMagnitude();
			m_initialAngularMomentum = pNewUniverse->GetTotalAngularMomentumMagnitude();

			CString msg;
			msg.Format(_T("Loaded: %s (Bodies: %d, Iteration: %d)"),
				filepath, pNewUniverse->GetMassCount(), pNewUniverse->m_iIteration);
			SetStatusBarMessage(msg);
			AfxMessageBox(msg, MB_ICONINFORMATION);

			// Refresh view
			Invalidate();
		}
		else
		{
			delete pNewUniverse;
			SetStatusBarMessage(_T("Failed to load simulation state"));
			AfxMessageBox(_T("Failed to load simulation state.\nFile may be corrupted or invalid format."), MB_ICONERROR);
		}
	}
}

// ============================================================================
// Export Handlers
// ============================================================================

// Export current frame as BMP image
void CEpistemotronView::OnExportFrame()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	int width = rcClient.Width();
	int height = rcClient.Height();

	if (width <= 0 || height <= 0)
	{
		SetStatusBarMessage(_T("Cannot export: invalid client area"));
		AfxMessageBox(_T("Cannot export: invalid client area."), MB_ICONERROR);
		return;
	}

	SetStatusBarMessage(_T("Select file to save frame..."));

	// Create file save dialog
	CFileDialog saveDlg(FALSE, _T("bmp"), _T("frame.bmp"),
		OFN_OVERWRITEPROMPT,
		_T("Bitmap Image (*.bmp)|*.bmp|PNG Image (*.png)|*.png|All Files (*.*)|*.*||"),
		this);

	if (saveDlg.DoModal() == IDOK)
	{
		CString filepath = saveDlg.GetPathName();
		CString ext = saveDlg.GetFileExt();

		// Create offscreen bitmap for capture
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(&m_memDC, width, height);
		CBitmap* oldBitmap = memDC.SelectObject(&bitmap);

		// Copy current view to offscreen DC
		memDC.BitBlt(0, 0, width, height, &m_memDC, 0, 0, SRCCOPY);

		// Save as BMP
		if (ext.CompareNoCase(_T("bmp")) == 0)
		{
			// Get bitmap data
			BITMAP bmpInfo;
			bitmap.GetBitmap(&bmpInfo);

			BITMAPINFOHEADER bih = { 0 };
			bih.biSize = sizeof(BITMAPINFOHEADER);
			bih.biWidth = width;
			bih.biHeight = -height;  // Top-down DIB
			bih.biPlanes = 1;
			bih.biBitCount = 24;
			bih.biCompression = BI_RGB;
			bih.biSizeImage = ((width * 3 + 3) & ~3) * height;

			// Get DIBits
			CFile file;
			if (file.Open(filepath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
			{
				BITMAPFILEHEADER bfh = { 0 };
				bfh.bfType = 0x4D42;  // 'BM'
				bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
				bfh.bfSize = bfh.bfOffBits + bih.biSizeImage;

				file.Write(&bfh, sizeof(bfh));
				file.Write(&bih, sizeof(bih));

				// Get bitmap bits (bottom-up)
				BYTE* pBits = new BYTE[bih.biSizeImage];
				BITMAPINFO bmi = { 0 };
				bmi.bmiHeader = bih;
				bmi.bmiHeader.biHeight = height;  // Bottom-up for GetDIBits

				if (::GetDIBits(memDC.GetSafeHdc(), bitmap, 0, height, pBits, &bmi, DIB_RGB_COLORS))
				{
					file.Write(pBits, bih.biSizeImage);
					SetStatusBarMessage(_T("Frame saved as BMP"));
				}
				delete[] pBits;
				file.Close();
			}
		}
		else if (ext.CompareNoCase(_T("png")) == 0)
		{
			// For PNG, save as BMP first then convert (simple approach)
			CString tempPath = filepath;
			tempPath.Replace(_T(".png"), _T(".bmp"));

			// Save as BMP first
			CString bmpExt = _T("bmp");
			saveDlg.m_ofn.filter = _T("Bitmap Image (*.bmp)|*.bmp||");

			// Use CImage for PNG export (simpler)
			CImage image;
			if (image.Load(filepath.Left(filepath.GetLength() - 4) + _T(".bmp")))
			{
				image.Save(filepath);
				SetStatusBarMessage(_T("Frame saved as PNG"));
			}
			else
			{
				// Fallback: just save as BMP with .png extension warning
				AfxMessageBox(_T("PNG export not available. Saved as BMP instead."), MB_ICONWARNING);
			}
		}

		// Cleanup
		memDC.SelectObject(oldBitmap);
		memDC.DeleteDC();
		bitmap.DeleteObject();

		CString msg;
		msg.Format(_T("Frame exported to: %s"), filepath);
		TRACE(_T("%s\n"), msg);
	}
}

// Export simulation as image sequence
void CEpistemotronView::OnExportSequence()
{
	// Simple implementation: export current frame only with sequence naming
	// Full implementation would require recording mode integration

	SetStatusBarMessage(_T("Select directory for image sequence..."));

	CFileDialog saveDlg(FALSE, _T("bmp"), _T("frame_00001.bmp"),
		OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
		_T("Bitmap Image (*.bmp)|*.bmp|All Files (*.*)|*.*||"),
		this);

	if (saveDlg.DoModal() == IDOK)
	{
		CString filepath = saveDlg.GetPathName();

		// Extract directory
		CString dirPath = filepath.Left(filepath.ReverseFind(_T('\\')) + 1);

		// Create directory if needed
		CreateDirectory(dirPath, nullptr);

		// Export current frame
		CRect rcClient;
		GetClientRect(&rcClient);
		int width = rcClient.Width();
		int height = rcClient.Height();

		if (width > 0 && height > 0)
		{
			// Get current iteration for filename
			CEpistemotronDoc* pDoc = GetDocument();
			int iteration = pDoc && pDoc->m_pCurrentUniverse ? pDoc->m_pCurrentUniverse->m_iIteration : 0;

			CString framePath;
			framePath.Format(_T("%sframe_%05d.bmp"), dirPath, iteration);

			// Save bitmap (reuse OnExportFrame logic simplified)
			CFile file;
			if (file.Open(framePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
			{
				BITMAPINFOHEADER bih = { 0 };
				bih.biSize = sizeof(BITMAPINFOHEADER);
				bih.biWidth = width;
				bih.biHeight = -height;
				bih.biPlanes = 1;
				bih.biBitCount = 24;
				bih.biCompression = BI_RGB;
				bih.biSizeImage = ((width * 3 + 3) & ~3) * height;

				BITMAPFILEHEADER bfh = { 0 };
				bfh.bfType = 0x4D42;
				bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
				bfh.bfSize = bfh.bfOffBits + bih.biSizeImage;

				file.Write(&bfh, sizeof(bfh));
				file.Write(&bih, sizeof(bih));

				BYTE* pBits = new BYTE[bih.biSizeImage];
				BITMAPINFO bmi = { 0 };
				bmi.bmiHeader = bih;
				bmi.bmiHeader.biHeight = height;

				if (::GetDIBits(m_memDC.GetSafeHdc(), m_memBitmap, 0, height, pBits, &bmi, DIB_RGB_COLORS))
				{
					file.Write(pBits, bih.biSizeImage);
					SetStatusBarMessage(_T("Frame saved to sequence"));
				}
				delete[] pBits;
				file.Close();
			}

			CString msg;
			msg.Format(_T("Frame %d exported to: %s"), iteration, framePath);
			AfxMessageBox(msg, MB_ICONINFORMATION);
		}
	}
}
