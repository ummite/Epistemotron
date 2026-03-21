
// Epistemotron.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Epistemotron.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "EpistemotronDoc.h"
#include "EpistemotronView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEpistemotronApp

BEGIN_MESSAGE_MAP(CEpistemotronApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CEpistemotronApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CEpistemotronApp construction

CEpistemotronApp::CEpistemotronApp() noexcept
{
	m_bHiColorIcons = TRUE;

	// Support Restart Manager for Windows update resilience
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// Application ID for Windows 10+ taskbar grouping and settings isolation
	SetAppID(_T("Epistemotron.GravitationalSimulator.NBody.1.0"));

	// NOTE: All significant initialization is deferred to InitInstance()
	// to ensure proper error handling and resource cleanup.
}

// The one and only CEpistemotronApp object

CEpistemotronApp theApp;


// CEpistemotronApp initialization

BOOL CEpistemotronApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// AfxInitRichEdit2() is required to use RichEdit control.
	// This application does not currently use RichEdit controls, so this
	// initialization is omitted to reduce startup time and memory usage.
	// If RichEdit support is needed in the future, uncomment the line below:
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// Registry key for application settings storage
	SetRegistryKey(_T("Epistemotron\\GravitationalSimulator\\1.0"));

	// Load standard INI file options, including Most Recently Used (MRU) file list
	// MRU list size constant defined here for maintainability
	constexpr int MAX_MRU_FILES = 4;
	LoadStdProfileSettings(MAX_MRU_FILES);


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_EpistemotronTYPE,
		RUNTIME_CLASS(CEpistemotronDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CEpistemotronView));
	if (!pDocTemplate)
	{
		AfxMessageBox(_T("Epistemotron: Low on resources!"));
		AfxOleTerm(FALSE);
		return FALSE;
	}
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame)
	{
		AfxMessageBox(_T("Epistemotron: Low on resources!"));
		AfxOleTerm(FALSE);
		return FALSE;  // Memory allocation failed
	}
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		AfxOleTerm(FALSE);
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
	{
		AfxOleTerm(FALSE);
		return FALSE;
	}
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CEpistemotronApp::ExitInstance()
{
	// Clean up OLE resources (required if AfxOleInit was called)
	AfxOleTerm(FALSE);

	// NOTE: Document templates are automatically cleaned up by CWinAppEx::ExitInstance()
	// No additional resource cleanup needed for this application.

	return CWinAppEx::ExitInstance();
}

// CEpistemotronApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CEpistemotronApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CEpistemotronApp customization load/save methods

void CEpistemotronApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CEpistemotronApp::LoadCustomState()
{
}

void CEpistemotronApp::SaveCustomState()
{
}

// CEpistemotronApp message handlers



