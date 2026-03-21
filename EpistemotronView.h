
// EpistemotronView.h : interface of the CEpistemotronView class
//

#pragma once

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
	void ConfigureSimulation();  // Open configuration dialog
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
	UINT m_timerId;
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

	// Timer handler
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSimulationTimer();

	// Camera control handlers
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

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
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in EpistemotronView.cpp
inline CEpistemotronDoc* CEpistemotronView::GetDocument() const
   { return reinterpret_cast<CEpistemotronDoc*>(m_pDocument); }
#endif

