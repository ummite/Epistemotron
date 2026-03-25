#pragma once

// Simple breakpoint dialog for setting pause iteration
class CBreakpointDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBreakpointDlg)

public:
	CBreakpointDlg(int currentIteration, CWnd* pParent = nullptr);
	virtual ~CBreakpointDlg();

	enum { IDD = IDD_BREAKPOINT_DLG };

// Dialog Data
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

public:
	int m_currentIteration;
	int m_targetIteration;

	// Get the target iteration set by user
	int GetTargetIteration() const { return m_targetIteration; }
};
