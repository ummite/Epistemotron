
#include "pch.h"
#include "MainFrm.h"
#include "ClassView.h"
#include "Resource.h"
#include "Epistemotron.h"

class CClassViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CClassView;

	DECLARE_SERIAL(CClassViewMenuButton)

public:
	CClassViewMenuButton(HMENU hMenu = nullptr) noexcept : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CClassViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClassView::CClassView() noexcept
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CClassView::~CClassView()
{
}

BEGIN_MESSAGE_MAP(CClassView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView message handlers

int CClassView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}

	// Load images:
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSubMenu = menuSort.GetSubMenu(0);
	if (pSubMenu == nullptr)
	{
		TRACE(_T("Failed to load sort menu\n"));
		return -1;
	}

	// Detach the menu handle so CMenu destructor won't destroy it when it goes out of scope
	HMENU hMenu = pSubMenu->Detach();
	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CClassViewMenuButton(hMenu));

	CClassViewMenuButton* pButton = DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;

		// GetCmdMgr returns CCommandManager, not CCmdTarget
		// CCommandManager doesn't provide GetCmdImage, so skip image setting
		// pButton->SetImage is not applicable for command manager

		pButton->SetMessageWnd(this);
	}

	// Fill in some static tree view data (dummy code, nothing magic here)
	FillClassView();

	return 0;
}

void CClassView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CClassView::FillClassView()
{
	HTREEITEM hRoot = m_wndClassView.InsertItem(_T("Epistemotron classes"), 0, 0);
	m_wndClassView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	// ============================================================================
	// Application Classes
	// ============================================================================
	HTREEITEM hAppSection = m_wndClassView.InsertItem(_T("Application"), 0, 0, hRoot);

	HTREEITEM hClass = m_wndClassView.InsertItem(_T("CEpistemotronApp"), 1, 1, hAppSection);
	m_wndClassView.InsertItem(_T("CEpistemotronApp()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("~CEpistemotronApp()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("InitInstance()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("ExitInstance()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnFileNew()"), 3, 3, hClass);

	hClass = m_wndClassView.InsertItem(_T("CEpistemotronDoc"), 1, 1, hAppSection);
	m_wndClassView.InsertItem(_T("CEpistemotronDoc()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("~CEpistemotronDoc()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnNewDocument()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("Serialize()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnDrawThumbnail()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("m_pCurrentUniverse"), 6, 6, hClass);

	hClass = m_wndClassView.InsertItem(_T("CEpistemotronView"), 1, 1, hAppSection);
	m_wndClassView.InsertItem(_T("CEpistemotronView()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("~CEpistemotronView()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnDraw()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("StartSimulation()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("PauseSimulation()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("ResumeSimulation()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("StopSimulation()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("ResetSimulation()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("StepSimulation()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("RenderUniverse3D()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("DrawUIOverlay()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("LoadScenarioSolarSystem()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("LoadScenarioBinaryStar()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("LoadScenarioThreeBody()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("LoadScenarioGalaxy()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnExportFrame()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnExportSequence()"), 3, 3, hClass);

	// ============================================================================
	// Frame Classes
	// ============================================================================
	HTREEITEM hFrameSection = m_wndClassView.InsertItem(_T("Frame Windows"), 0, 0, hRoot);

	hClass = m_wndClassView.InsertItem(_T("CMainFrame"), 1, 1, hFrameSection);
	m_wndClassView.InsertItem(_T("CMainFrame()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("~CMainFrame()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnCreate()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("m_wndMenuBar"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_wndToolBar"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_wndStatusBar"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_wndFileView"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_wndClassView"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_wndOutput"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_wndProperties"), 6, 6, hClass);

	hClass = m_wndClassView.InsertItem(_T("CChildFrame"), 1, 1, hFrameSection);
	m_wndClassView.InsertItem(_T("CChildFrame()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("~CChildFrame()"), 3, 3, hClass);

	// ============================================================================
	// Dockable Pane Classes
	// ============================================================================
	HTREEITEM hPaneSection = m_wndClassView.InsertItem(_T("Dockable Panes"), 0, 0, hRoot);

	hClass = m_wndClassView.InsertItem(_T("CFileView"), 1, 1, hPaneSection);
	m_wndClassView.InsertItem(_T("CFileView()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("FillFileView()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("m_wndFileView"), 6, 6, hClass);

	hClass = m_wndClassView.InsertItem(_T("CClassView"), 1, 1, hPaneSection);
	m_wndClassView.InsertItem(_T("CClassView()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("FillClassView()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("m_wndClassView"), 6, 6, hClass);

	hClass = m_wndClassView.InsertItem(_T("COutputWnd"), 1, 1, hPaneSection);
	m_wndClassView.InsertItem(_T("COutputWnd()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("AddMessage()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("ClearAll()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnEditCopy()"), 3, 3, hClass);

	hClass = m_wndClassView.InsertItem(_T("CPropertiesWnd"), 1, 1, hPaneSection);
	m_wndClassView.InsertItem(_T("CPropertiesWnd()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnInitialUpdate()"), 3, 3, hClass);

	// ============================================================================
	// Dialog Classes
	// ============================================================================
	HTREEITEM hDialogSection = m_wndClassView.InsertItem(_T("Dialogs"), 0, 0, hRoot);

	hClass = m_wndClassView.InsertItem(_T("CSimConfigDlg"), 1, 1, hDialogSection);
	m_wndClassView.InsertItem(_T("CSimConfigDlg()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("DoDataExchange()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("GetConfig()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnBnClickedBtnRandomize()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("m_numBodies"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_stepSizeSec"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_stepsPerFrame"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_useRandomPositions"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_randomRadiusKm"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_description"), 6, 6, hClass);

	// ============================================================================
	// Science Module Classes
	// ============================================================================
	HTREEITEM hScienceSection = m_wndClassView.InsertItem(_T("Science Module"), 0, 0, hRoot);

	hClass = m_wndClassView.InsertItem(_T("Mass"), 1, 1, hScienceSection);
	m_wndClassView.InsertItem(_T("Mass()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("Randomize()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("Distance()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("EffectuerPasChangementPosition()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("EffectuerPasChangementVitesse()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("GetPhysicalRadiusKM()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("IsCollidingWith()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("GetKineticEnergy()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("GetLinearMomentum()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("GetAngularMomentum()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("RecordTrailPosition()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("ClearTrail()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("m_X, m_Y, m_Z"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_VitesseX, m_VitesseY, m_VitesseZ"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_MasseKG"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_trail"), 6, 6, hClass);

	hClass = m_wndClassView.InsertItem(_T("Universe"), 1, 1, hScienceSection);
	m_wndClassView.InsertItem(_T("Universe()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("Randomize()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("GenerateSimulationStep()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("SimulateFrom()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("SimulateFromVelocityVerlet()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("ProcessCollisionsSimple()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("ExportPPM()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("LoadSolarSystem()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("LoadBinaryStar()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("LoadThreeBody()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("LoadGalaxy()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("GetTotalEnergy()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("GetTotalLinearMomentum()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("GetTotalAngularMomentum()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("GetTotalCollisions()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("m_arrMasses"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_iIteration"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_totalCollisions"), 6, 6, hClass);

	hClass = m_wndClassView.InsertItem(_T("Simulator"), 1, 1, hScienceSection);
	m_wndClassView.InsertItem(_T("Simulator()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("Test()"), 3, 3, hClass);

	hClass = m_wndClassView.InsertItem(_T("TrailPoint"), 1, 1, hScienceSection);
	m_wndClassView.InsertItem(_T("x"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("y"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("z"), 6, 6, hClass);

	// ============================================================================
	// Globals
	// ============================================================================
	HTREEITEM hGlobals = m_wndClassView.InsertItem(_T("Globals"), 2, 2, hRoot);
	m_wndClassView.InsertItem(_T("theApp"), 5, 5, hGlobals);

	// Expand all sections
	m_wndClassView.Expand(hRoot, TVE_EXPAND);
	m_wndClassView.Expand(hAppSection, TVE_EXPAND);
	m_wndClassView.Expand(hFrameSection, TVE_EXPAND);
	m_wndClassView.Expand(hPaneSection, TVE_EXPAND);
	m_wndClassView.Expand(hDialogSection, TVE_EXPAND);
	m_wndClassView.Expand(hScienceSection, TVE_EXPAND);
	m_wndClassView.Expand(hGlobals, TVE_EXPAND);
}

void CClassView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != nullptr)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	if (!menu.LoadMenu(IDR_POPUP_SORT))
	{
		return;
	}

	CMenu* pSumMenu = menu.GetSubMenu(0);
	if (pSumMenu == nullptr)
	{
		return;
	}

	CWnd* pMainWnd = AfxGetMainWnd();
	if (pMainWnd != nullptr && pMainWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
		{
			delete pPopupMenu;  // Prevent memory leak on failed creation
			return;
		}

		((CMDIFrameWndEx*)pMainWnd)->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
		// Note: OnShowPopupMenu takes ownership of pPopupMenu and will delete it when done
	}
}

void CClassView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndClassView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CClassView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CClassView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CClassViewMenuButton* pButton = DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		// GetCmdMgr can return nullptr, check before dereferencing
		// CCommandManager doesn't provide GetCmdImage, skip image setting
		// CCmdTarget* pCmdMgr = GetCmdMgr();
		// if (pCmdMgr != nullptr)
		// {
		//	pButton->SetImage(pCmdMgr->GetCmdImage(id));
		// }

		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void CClassView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CClassView::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("Add member function..."));
}

void CClassView::OnClassAddMemberVariable()
{
	AfxMessageBox(_T("Add member variable..."));
}

void CClassView::OnClassDefinition()
{
	AfxMessageBox(_T("Go to class definition..."));
}

void CClassView::OnClassProperties()
{
	AfxMessageBox(_T("Class properties..."));
}

void CClassView::OnNewFolder()
{
	AfxMessageBox(_T("New Folder..."));
}

void CClassView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CClassView::OnSetFocus(CWnd* /*pOldWnd*/)
{
	m_wndClassView.SetFocus();
}

void CClassView::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndClassView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* Locked */);
}
