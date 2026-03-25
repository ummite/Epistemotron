#pragma once

#include "Resource.h"  // For IDD_CUSTOM_SCENARIO_DLG
#include "Science/Mass.h"

// Forward declaration
class Universe;

// Custom scenario editor dialog
class CCustomScenarioDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCustomScenarioDlg)

public:
	CCustomScenarioDlg(CWnd* pParent = nullptr);
	virtual ~CCustomScenarioDlg();

	enum { IDD = IDD_CUSTOM_SCENARIO_DLG };

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CUSTOM_SCENARIO_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	// Scenario name
	CString m_scenarioName;

	// Body list (using CListBox for simplicity)
	CListBox m_bodyList;

	// Body properties for editing
	CString m_bodyName;
	double m_bodyMass;
	double m_bodyX;
	double m_bodyY;
	double m_bodyZ;
	double m_bodyVX;
	double m_bodyVY;
	double m_bodyVZ;

	// Storage for body data (parallel to listbox)
	struct BodyData
	{
		CString name;
		double mass;
		double x, y, z;
		double vx, vy, vz;
	};
	std::vector<BodyData> m_bodies;

	// Methods
	afx_msg void OnBnClickedBtnAddBody();
	afx_msg void OnBnClickedBtnRemoveBody();
	afx_msg void OnBnClickedBtnClearAll();
	afx_msg void OnBnClickedBtnLoadFromCurrent();
	afx_msg void OnBnClickedBtnEditBody();
	afx_msg void OnBnClickedBtnSaveScenario();
	afx_msg void OnBnClickedBtnLoadScenario();
	afx_msg void OnBnClickedBtnApply();

	// Get the scenario as a vector of Mass objects
	std::vector<Mass> GetScenarioBodies() const;

	// Load current universe into the dialog
	void LoadFromCurrentUniverse(Universe* pUniverse);

	// Save/Load scenario to/from file (JSON format)
	BOOL SaveScenario(LPCTSTR filename);
	BOOL LoadScenario(LPCTSTR filename);
};
