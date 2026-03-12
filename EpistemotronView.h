
// EpistemotronView.h : interface of the CEpistemotronView class
//

#pragma once

class CEpistemotronDoc;

// Simulation states
enum class SimulationState { Stopped, Running, Paused };

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

	// Timer handler
	afx_msg void OnSimulationTimer();

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
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in EpistemotronView.cpp
inline CEpistemotronDoc* CEpistemotronView::GetDocument() const
   { return reinterpret_cast<CEpistemotronDoc*>(m_pDocument); }
#endif

