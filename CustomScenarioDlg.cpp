// CustomScenarioDlg.cpp : implementation file
//

#include "pch.h"
#include "Epistemotron.h"
#include "CustomScenarioDlg.h"
#include "Science/Universe.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CCustomScenarioDlg, CDialogEx)

CCustomScenarioDlg::CCustomScenarioDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD, pParent)
	, m_scenarioName(_T("My Custom Scenario"))
	, m_bodyName(_T("Body"))
	, m_bodyMass(5.98e24)  // Earth mass
	, m_bodyX(0.0)
	, m_bodyY(0.0)
	, m_bodyZ(0.0)
	, m_bodyVX(0.0)
	, m_bodyVY(0.0)
	, m_bodyVZ(0.0)
{
}

BOOL CCustomScenarioDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add column headers for the list (simulated with static text in dialog)
	// The listbox will show formatted strings with all body info

	return TRUE;  // return TRUE unless you set the focus to a control
}

CCustomScenarioDlg::~CCustomScenarioDlg()
{
}

void CCustomScenarioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SCENARIO_NAME, m_scenarioName);
	DDX_Control(pDX, IDC_LIST_BODY_LIST, m_bodyList);
	DDX_Text(pDX, IDC_EDIT_BODY_NAME, m_bodyName);
	DDX_Text(pDX, IDC_EDIT_BODY_MASS, m_bodyMass);
	DDX_Text(pDX, IDC_EDIT_BODY_X, m_bodyX);
	DDX_Text(pDX, IDC_EDIT_BODY_Y, m_bodyY);
	DDX_Text(pDX, IDC_EDIT_BODY_Z, m_bodyZ);
	DDX_Text(pDX, IDC_EDIT_BODY_VX, m_bodyVX);
	DDX_Text(pDX, IDC_EDIT_BODY_VY, m_bodyVY);
	DDX_Text(pDX, IDC_EDIT_BODY_VZ, m_bodyVZ);
}

BEGIN_MESSAGE_MAP(CCustomScenarioDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ADD_BODY, &CCustomScenarioDlg::OnBnClickedBtnAddBody)
	ON_BN_CLICKED(IDC_BTN_REMOVE_BODY, &CCustomScenarioDlg::OnBnClickedBtnRemoveBody)
	ON_BN_CLICKED(IDC_BTN_CLEAR_ALL, &CCustomScenarioDlg::OnBnClickedBtnClearAll)
	ON_BN_CLICKED(IDC_BTN_LOAD_CURRENT, &CCustomScenarioDlg::OnBnClickedBtnLoadFromCurrent)
	ON_BN_CLICKED(IDC_BTN_EDIT_BODY, &CCustomScenarioDlg::OnBnClickedBtnEditBody)
	ON_BN_CLICKED(IDC_BTN_SAVE_SCENARIO, &CCustomScenarioDlg::OnBnClickedBtnSaveScenario)
	ON_BN_CLICKED(IDC_BTN_LOAD_SCENARIO, &CCustomScenarioDlg::OnBnClickedBtnLoadScenario)
	ON_BN_CLICKED(IDOK, &CCustomScenarioDlg::OnBnClickedBtnApply)
END_MESSAGE_MAP()

void CCustomScenarioDlg::OnBnClickedBtnAddBody()
{
	UpdateData(TRUE);

	// Store body data
	BodyData body;
	body.name = m_bodyName;
	body.mass = m_bodyMass;
	body.x = m_bodyX;
	body.y = m_bodyY;
	body.z = m_bodyZ;
	body.vx = m_bodyVX;
	body.vy = m_bodyVY;
	body.vz = m_bodyVZ;
	m_bodies.push_back(body);

	// Format body info as a single string for listbox (Name, Mass, X, Y, Z, VX, VY, VZ)
	CString massStr;
	massStr.Format(_T("%.3e"), m_bodyMass);
	CString bodyInfo;
	bodyInfo.Format(_T("%-12s  %-16s  %8.1f  %8.1f  %8.1f  %10.1f  %10.1f  %10.1f"),
		m_bodyName, massStr, m_bodyX, m_bodyY, m_bodyZ, m_bodyVX, m_bodyVY, m_bodyVZ);

	m_bodyList.AddString(bodyInfo);

	// Clear the input fields for next entry
	m_bodyName.Empty();
	m_bodyMass = 5.98e24;
	m_bodyX = 0.0;
	m_bodyY = 0.0;
	m_bodyZ = 0.0;
	m_bodyVX = 0.0;
	m_bodyVY = 0.0;
	m_bodyVZ = 0.0;
	UpdateData(FALSE);
}

void CCustomScenarioDlg::OnBnClickedBtnRemoveBody()
{
	int selectedIndex = m_bodyList.GetCurSel();
	if (selectedIndex >= 0 && selectedIndex < (int)m_bodies.size())
	{
		m_bodies.erase(m_bodies.begin() + selectedIndex);
		m_bodyList.DeleteString(selectedIndex);
	}
}

void CCustomScenarioDlg::OnBnClickedBtnClearAll()
{
	if (AfxMessageBox(_T("Are you sure you want to clear all bodies?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		m_bodies.clear();
		m_bodyList.ResetContent();
	}
}

void CCustomScenarioDlg::OnBnClickedBtnLoadFromCurrent()
{
	// This will be implemented to load from current universe
	AfxMessageBox(_T("Load from current universe - coming soon!"), MB_ICONINFORMATION);
}

void CCustomScenarioDlg::OnBnClickedBtnEditBody()
{
	int selectedIndex = m_bodyList.GetCurSel();
	if (selectedIndex >= 0 && selectedIndex < (int)m_bodies.size())
	{
		// Load selected body properties into edit fields
		const BodyData& body = m_bodies[selectedIndex];
		m_bodyName = body.name;
		m_bodyMass = body.mass;
		m_bodyX = body.x;
		m_bodyY = body.y;
		m_bodyZ = body.z;
		m_bodyVX = body.vx;
		m_bodyVY = body.vy;
		m_bodyVZ = body.vz;
		UpdateData(FALSE);
	}
	else
	{
		AfxMessageBox(_T("Please select a body to edit."), MB_ICONWARNING);
	}
}

void CCustomScenarioDlg::OnBnClickedBtnApply()
{
	OnOK();
}

void CCustomScenarioDlg::OnBnClickedBtnSaveScenario()
{
	CFileDialog saveDlg(FALSE, _T("esc"), nullptr,
		OFN_OVERWRITEPROMPT,
		_T("Epistemotron Scenario Files (*.esc)|*.esc|All Files (*.*)|*.*|"),
		this);

	if (saveDlg.DoModal() == IDOK)
	{
		if (SaveScenario(saveDlg.GetPathName()))
		{
			CString msg;
			msg.Format(_T("Scenario saved to: %s"), saveDlg.GetPathName());
			AfxMessageBox(msg, MB_ICONINFORMATION);
		}
	}
}

void CCustomScenarioDlg::OnBnClickedBtnLoadScenario()
{
	CFileDialog openDlg(TRUE, _T("esc"), nullptr,
		0,
		_T("Epistemotron Scenario Files (*.esc)|*.esc|All Files (*.*)|*.*|"),
		this);

	if (openDlg.DoModal() == IDOK)
	{
		if (LoadScenario(openDlg.GetPathName()))
		{
			CString msg;
			msg.Format(_T("Scenario loaded: %s"), m_scenarioName);
			AfxMessageBox(msg, MB_ICONINFORMATION);
		}
	}
}

std::vector<Mass> CCustomScenarioDlg::GetScenarioBodies() const
{
	std::vector<Mass> bodies;
	bodies.reserve(m_bodies.size());

	for (const auto& bodyData : m_bodies)
	{
		Mass body;
		body.m_Name = bodyData.name;
		body.m_MasseKG = bodyData.mass;
		body.m_X = bodyData.x;
		body.m_Y = bodyData.y;
		body.m_Z = bodyData.z;
		body.m_VitesseX = bodyData.vx;
		body.m_VitesseY = bodyData.vy;
		body.m_VitesseZ = bodyData.vz;
		bodies.push_back(body);
	}

	return bodies;
}

void CCustomScenarioDlg::LoadFromCurrentUniverse(Universe* pUniverse)
{
	if (!pUniverse) return;

	m_bodies.clear();
	m_bodyList.ResetContent();

	int massCount = pUniverse->GetMassCount();
	for (int i = 0; i < massCount; i++)
	{
		const Mass& m = pUniverse->GetAt(i);

		// Store body data
		BodyData body;
		CString bodyName = m.m_Name;
		if (bodyName.IsEmpty())
		{
			bodyName.Format(_T("Body %d"), i + 1);
		}
		body.name = bodyName;
		body.mass = m.m_MasseKG;
		body.x = m.m_X;
		body.y = m.m_Y;
		body.z = m.m_Z;
		body.vx = m.m_VitesseX;
		body.vy = m.m_VitesseY;
		body.vz = m.m_VitesseZ;
		m_bodies.push_back(body);

		// Add to listbox (Name, Mass, X, Y, Z, VX, VY, VZ)
		CString massStr;
		massStr.Format(_T("%.3e"), m.m_MasseKG);
		CString bodyInfo;
		bodyInfo.Format(_T("%-12s  %-16s  %8.1f  %8.1f  %8.1f  %10.1f  %10.1f  %10.1f"),
			bodyName, massStr, m.m_X, m.m_Y, m.m_Z, m.m_VitesseX, m.m_VitesseY, m.m_VitesseZ);
		m_bodyList.AddString(bodyInfo);
	}
}

// ============================================================================
// Save/Load Scenario
// ============================================================================

BOOL CCustomScenarioDlg::SaveScenario(LPCTSTR filename)
{
	CStdioFile file;
	if (!file.Open(filename, CFile::modeCreate | CFile::modeWrite))
	{
		AfxMessageBox(_T("Failed to create file."), MB_ICONERROR);
		return FALSE;
	}

	// Write header
	file.WriteString(_T("# Epistemotron Scenario File\n"));
	file.WriteString(_T("# Format: ScenarioName, BodyCount, then BodyName Mass X Y Z VX VY VZ per line\n"));
	file.WriteString(m_scenarioName + _T("\n"));
	CString countStr;
	countStr.Format(_T("%d\n"), static_cast<int>(m_bodies.size()));
	file.WriteString(countStr);

	// Write each body
	for (const auto& body : m_bodies)
	{
		CString line;
		line.Format(_T("%s %.15e %.15e %.15e %.15e %.15e %.15e %.15e\n"),
			body.name, body.mass, body.x, body.y, body.z, body.vx, body.vy, body.vz);
		file.WriteString(line);
	}

	file.Close();
	return TRUE;
}

BOOL CCustomScenarioDlg::LoadScenario(LPCTSTR filename)
{
	CStdioFile file;
	if (!file.Open(filename, CFile::modeRead))
	{
		AfxMessageBox(_T("Failed to open file."), MB_ICONERROR);
		return FALSE;
	}

	CString line;

	// Skip comment lines and read header
	while (file.ReadString(line) && line.Left(1) == _T("#"))
	{
		// Skip comments
	}

	if (line.IsEmpty())
	{
		AfxMessageBox(_T("Invalid scenario file."), MB_ICONWARNING);
		file.Close();
		return FALSE;
	}

	// Read scenario name
	m_scenarioName = line;
	m_scenarioName.TrimRight();
	UpdateData(FALSE);

	// Read body count
	int bodyCount = 0;
	if (!file.ReadString(line))
	{
		AfxMessageBox(_T("Invalid scenario file: missing body count."), MB_ICONWARNING);
		file.Close();
		return FALSE;
	}
	bodyCount = _ttoi(line);

	// Clear existing bodies
	m_bodies.clear();
	m_bodyList.ResetContent();

	// Read each body
	for (int i = 0; i < bodyCount; i++)
	{
		if (!file.ReadString(line))
		{
			AfxMessageBox(_T("Invalid scenario file: unexpected end of file."), MB_ICONWARNING);
			file.Close();
			return FALSE;
		}

		BodyData body;
		double mass, x, y, z, vx, vy, vz;
		// Parse the line - use LPCTSTR for CString
		char nameBuffer[16] = { 0 };
		CStringA lineA(line);  // Convert to ANSI for sscanf_s
		int parsed = sscanf_s(lineA, "%15s %lf %lf %lf %lf %lf %lf %lf",
			nameBuffer, 16, &mass, &x, &y, &z, &vx, &vy, &vz);
		body.name = nameBuffer;

		if (parsed < 2)
		{
			AfxMessageBox(_T("Invalid scenario file: malformed body data."), MB_ICONWARNING);
			file.Close();
			return FALSE;
		}

		body.mass = mass;
		body.x = x;
		body.y = y;
		body.z = z;
		body.vx = vx;
		body.vy = vy;
		body.vz = vz;
		m_bodies.push_back(body);

		// Add to listbox
		CString massStr;
		massStr.Format(_T("%.3e"), body.mass);
		CString bodyInfo;
		bodyInfo.Format(_T("%-12s  %-16s  %8.1f  %8.1f  %8.1f  %10.1f  %10.1f  %10.1f"),
			body.name, massStr, body.x, body.y, body.z, body.vx, body.vy, body.vz);
		m_bodyList.AddString(bodyInfo);
	}

	file.Close();
	return TRUE;
}
