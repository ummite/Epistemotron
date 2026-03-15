#pragma once

class CSimConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSimConfigDlg)

public:
	CSimConfigDlg(CWnd* pParent = nullptr);

	enum { IDD = IDD_SIM_CONFIG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	// Configuration values
	int m_numBodies;
	int m_stepSizeSec;
	int m_stepsPerFrame;
	BOOL m_useRandomPositions;
	double m_randomRadiusKm;
	CString m_description;

	// Get simulation configuration
	struct SimConfig
	{
		int numBodies;
		int stepSizeSec;
		int stepsPerFrame;
		BOOL useRandom;
		double randomRadius;
		CString description;
	};
	SimConfig GetConfig() const;

protected:
	afx_msg void OnBnClickedBtnRandomize();

	DECLARE_MESSAGE_MAP()
};
