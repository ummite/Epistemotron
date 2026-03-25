
// EpistemotronView.h : interface of the CEpistemotronView class
//

#pragma once

#include "Science/Universe.h"

class CEpistemotronDoc;

// Simulation states enum is now defined in framework.h (via pch.h)
// Also remove ScenarioType and IntegratorType from here as they're in framework.h
class CEpistemotronView : public CView
{
protected:
	CEpistemotronView() noexcept;
	DECLARE_DYNCREATE(CEpistemotronView)

// Attributes
public:
	CEpistemotronDoc* GetDocument() const;

// Simulation control
public:
	void StartSimulation();
	void PauseSimulation();
	void ResumeSimulation();
	void StopSimulation();
	void ResetSimulation();
	void StepSimulation();
	void SpeedUp();
	void SlowDown();
	SimulationState GetSimulationState() const { return m_simulationState; }

// Scenario selection
public:
	void LoadScenarioSolarSystem();
	void LoadScenarioBinaryStar();
	void LoadScenarioThreeBody();
	void LoadScenarioGalaxy();
	void CycleScenario();  // Cycle through scenarios (keyboard shortcut)
	ScenarioType GetCurrentScenario() const { return m_currentScenario; }
	CString GetScenarioName() const;

// Operations
public:
	void RefreshView();  // Trigger a view redraw

// Recording methods
public:
	void StartRecording(LPCTSTR path, LPCTSTR prefix);
	void StopRecording();
	BOOL IsRecording() const { return m_bRecording; }
	CString GetRecordStatus() const;  // Returns formatted recording status string
protected:
	void SaveCurrentFrame();  // Save current frame to disk (called from OnDraw)

// Overrides
public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CEpistemotronView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// Simulation state
	SimulationState m_simulationState;
	UINT_PTR m_timerId;
	int m_stepsPerFrame;
	int m_stepSizeSec;  // Time step in seconds
	IntegratorType m_integratorType;  // Integrator selection (now defined in pch.h via framework.h)

	// Current scenario type
	ScenarioType m_currentScenario;

	// Camera controls
	double m_cameraDistance;  // Distance from camera to origin (km)
	double m_fieldOfView;     // Field of view in km
	double m_massScaleFactor; // Scale factor for mass radius rendering

	// Camera pan offset (in km)
	double m_panOffsetX;
	double m_panOffsetY;

	// Camera rotation (in radians)
	double m_rotationPitch;  // Rotation around X-axis (up/down)
	double m_rotationYaw;    // Rotation around Y-axis (left/right)
	double m_rotationRoll;   // Rotation around Z-axis (tilt)

	// Mouse tracking for pan and rotation
	BOOL m_bDragging;
	BOOL m_bRotating;
	BOOL m_bRolling;  // TRUE if middle button (roll), FALSE if right button (pitch/yaw)
	CPoint m_lastMousePos;

	// Trail visualization
	BOOL m_bShowTrails;  // Toggle for showing/hiding orbit trails

	// Collision control
	BOOL m_bEnableCollisions;  // Toggle for collision detection
	BOOL m_bPauseOnCollision;  // Pause simulation when collision occurs

	// Help overlay
	BOOL m_bShowHelp;    // Toggle for showing keyboard shortcuts help

	// Energy statistics reference (for drift calculation)
	double m_initialTotalEnergy;      // Total energy at simulation start (joules)
	double m_initialLinearMomentum;   // Linear momentum magnitude at start (kg*m/s)
	double m_initialAngularMomentum;  // Angular momentum magnitude at start (kg*m²/s)

	// Collision statistics
	int m_totalCollisions;         // Total collisions since simulation start
	int m_collisionsThisFrame;     // Collisions that occurred in the last frame
	UINT64 m_lastCollisionTime;    // Time of last collision (GetTickCount64)
	double m_largestCollisionMass; // Mass of largest collision (kg)

	// Visual collision feedback - flash effects
	struct CollisionFlash
	{
		double x;           // X position in km
		double y;           // Y position in km
		double z;           // Z position in km
		COLORREF color;     // Flash color
		int remainingFrames; // How many frames left to display

		CollisionFlash(double px, double py, double pz, COLORREF pcolor, int frames)
			: x(px), y(py), z(pz), color(pcolor), remainingFrames(frames) {}
	};
	std::vector<CollisionFlash> m_collisionFlashes;

	// Collision flash management
	void AddCollisionFlash(double x, double y, double z, COLORREF color);
	void UpdateCollisionFlashes();
	void RenderCollisionFlashes(CDC* pDC, int centerX, int centerY, double cameraDistance, double fov, double panX, double panY);

	// Celestial body selection
	int m_selectedBodyIndex;      // Index of selected body (-1 if none selected)
	CString m_selectedBodyInfo;   // Formatted info string for selected body

	// Double buffering - pre-allocated to avoid per-frame allocation
	CDC m_memDC;
	CBitmap m_memBitmap;
	CBitmap* m_pOldBitmap;
	int m_memBitmapWidth;
	int m_memBitmapHeight;
	BOOL EnsureDoubleBuffer(CDC* pDC, int width, int height);

	// Recording members - image sequence export
	BOOL m_bRecording;           // TRUE if recording is active
	CString m_recordPath;        // Output directory path
	int m_recordFrameCount;      // Frame counter for sequential naming
	CString m_recordPrefix;      // Prefix for output filenames

	// Timer handler
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// FPS counter members
	UINT64 m_lastFrameTime;      // Timestamp of last frame (ms)
	int m_frameCount;            // Frame count for FPS calculation
	double m_currentFps;         // Current frames per second

	// Camera control handlers
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	// Mouse handlers for panning
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// Mouse handlers for rotation (right button - pitch/yaw)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	// Mouse handlers for roll (middle button)
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	// Utility
	CString GetIntegratorName() const;

	// Utility functions
	CString FormatScientific(double value);
	CString CalculateOrbitalPeriod(const Mass& body, const Universe& universe, int bodyIndex);

	// Status bar helper
	void SetStatusBarMessage(LPCTSTR message);

	// Trail rendering
	void RenderTrailForMasses(CDC* pDC, const Universe& universe,
		int centerX, int centerY, double cameraDistance, double fov,
		double panX, double panY);

	// 3D rendering methods
	void RenderUniverse3D(CDC* pDC, const Universe& universe, const CRect& rcClient);
	void DrawUIOverlay(CDC* pDC, const CRect& rcClient);

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSimulationStart();
	afx_msg void OnSimulationPause();
	afx_msg void OnSimulationResume();
	afx_msg void OnSimulationStop();
	afx_msg void OnSimulationReset();
	afx_msg void OnSimulationSpeedUp();
	afx_msg void OnSimulationSlowDown();
	afx_msg void OnSimulationStep();
	afx_msg void OnSimulationConfig();

	// Scenario selection handlers
	afx_msg void OnScenarioSolarSystem();
	afx_msg void OnScenarioBinaryStar();
	afx_msg void OnScenarioThreeBody();
	afx_msg void OnScenarioGalaxy();
	afx_msg void OnScenarioNext();

	// Recording command handlers
	afx_msg void OnRecordingStart();
	afx_msg void OnRecordingStop();

	// Save/Load state command handlers
	afx_msg void OnStateSave();
	afx_msg void OnStateLoad();

	// Export command handlers
	afx_msg void OnExportFrame();
	afx_msg void OnExportSequence();

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in EpistemotronView.cpp
inline CEpistemotronDoc* CEpistemotronView::GetDocument() const
   { return reinterpret_cast<CEpistemotronDoc*>(m_pDocument); }
#endif

