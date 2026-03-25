// BreakpointDlg.cpp : implementation file
//

#include "pch.h"
#include "Epistemotron.h"
#include "BreakpointDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CBreakpointDlg, CDialogEx)

CBreakpointDlg::CBreakpointDlg(int currentIteration, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD, pParent)
	, m_currentIteration(currentIteration)
	, m_targetIteration(currentIteration + 100)  // Default: 100 iterations ahead
{
}

CBreakpointDlg::~CBreakpointDlg()
{
}

void CBreakpointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_BREAKPOINT_CURRENT, m_currentIteration);
	DDX_Text(pDX, IDC_EDIT_BREAKPOINT_TARGET, m_targetIteration);
}

BEGIN_MESSAGE_MAP(CBreakpointDlg, CDialogEx)
END_MESSAGE_MAP()
