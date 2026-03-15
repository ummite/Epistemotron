// SimConfigDlg.cpp : implementation file
//

#include "pch.h"
#include "Epistemotron.h"
#include "SimConfigDlg.h"
#include "afxdialogex.h"

// Validation constants
static constexpr int MIN_NUM_BODIES = 1;
static constexpr int MAX_NUM_BODIES = 10000;
static constexpr int MIN_STEP_SIZE = 1;
static constexpr int MAX_STEP_SIZE = 31536000;  // 1 year in seconds
static constexpr int MIN_STEPS_PER_FRAME = 1;
static constexpr int MAX_STEPS_PER_FRAME = 10000;
static constexpr int MAX_DESCRIPTION_LEN = 256;

IMPLEMENT_DYNAMIC(CSimConfigDlg, CDialogEx)

CSimConfigDlg::CSimConfigDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIM_CONFIG_DLG, pParent)
	, m_numBodies(4)
	, m_stepSizeSec(3600)
	, m_stepsPerFrame(1)
	, m_useRandomPositions(FALSE)
	, m_randomRadiusKm(146000000.0)
	, m_description(_T("Solar System Simulation"))
{
}

void CSimConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NUM_BODIES, m_numBodies);
	DDV_MinMaxInt(pDX, m_numBodies, MIN_NUM_BODIES, MAX_NUM_BODIES);
	DDX_Text(pDX, IDC_EDIT_STEP_SIZE, m_stepSizeSec);
	DDV_MinMaxInt(pDX, m_stepSizeSec, MIN_STEP_SIZE, MAX_STEP_SIZE);
	DDX_Text(pDX, IDC_EDIT_STEPS_PER_FRAME, m_stepsPerFrame);
	DDV_MinMaxInt(pDX, m_stepsPerFrame, MIN_STEPS_PER_FRAME, MAX_STEPS_PER_FRAME);
	DDX_Check(pDX, IDC_CHECK_RANDOM, m_useRandomPositions);
	DDX_Text(pDX, IDC_EDIT_RANDOM_RADIUS, m_randomRadiusKm);
	DDV_MinMaxDouble(pDX, m_randomRadiusKm, 0.001, 1e18);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_description);
	DDV_MaxChars(pDX, m_description, MAX_DESCRIPTION_LEN);
}

BEGIN_MESSAGE_MAP(CSimConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_RANDOMIZE, &CSimConfigDlg::OnBnClickedBtnRandomize)
END_MESSAGE_MAP()

// Input validation - prevent invalid simulation parameters
void CSimConfigDlg::OnOK()
{
	if (!UpdateData(TRUE))
		return;  // Data validation failed, MFC shows error

	// Additional validation beyond DDV if needed
	CDialogEx::OnOK();
}

CSimConfigDlg::SimConfig CSimConfigDlg::GetConfig() const
{
	SimConfig config;
	config.numBodies = m_numBodies;
	config.stepSizeSec = m_stepSizeSec;
	config.stepsPerFrame = m_stepsPerFrame;
	config.useRandom = m_useRandomPositions;
	config.randomRadius = m_randomRadiusKm;
	config.description = m_description;
	return config;
}

void CSimConfigDlg::OnBnClickedBtnRandomize()
{
	UpdateData(TRUE);
	// Generate a random description based on configuration
	CString desc;
	if (m_useRandomPositions)
	{
		desc.Format(_T("Random %d-Body System"), m_numBodies);
	}
	else
	{
		switch (m_numBodies)
		{
		case 2: desc = _T("Binary Star System"); break;
		case 3: desc = _T("Three-Body Problem"); break;
		case 4: desc = _T("Solar System (Sun, Earth, Moon, Mars)"); break;
		default: desc.Format(_T("%d-Body Simulation"), m_numBodies);
		}
	}
	m_description = desc;
	UpdateData(FALSE);
}
