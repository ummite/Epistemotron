
#include "pch.h"
#include "mainfrm.h"
#include "FileView.h"
#include "Resource.h"
#include "Epistemotron.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView() noexcept
{
}

CFileView::~CFileView()
{
}

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, c_nControlId))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	// Load view images:
	m_FileViewImages.Create(IDB_FILE_VIEW, c_nToolBarHeight, 0, c_magentaMask);
	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// Fill in some static tree view data (dummy code, nothing magic here)
	FillFileView();
	AdjustLayout();

	return 0;
}

void CFileView::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	// Base class handles resizing; we just adjust our internal layout
	CDockablePane::OnSize(0, 0, 0);
	AdjustLayout();
}

void CFileView::FillFileView()
{
	HTREEITEM hRoot = m_wndFileView.InsertItem(_T("Epistemotron files"), 0, 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_STATEIMAGEMASK);

	// ============================================================================
	// Source Files
	// ============================================================================
	HTREEITEM hSrc = m_wndFileView.InsertItem(_T("Source Files"), 0, 0, hRoot);

	// Main application source files
	m_wndFileView.InsertItem(_T("Epistemotron.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("EpistemotronDoc.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("EpistemotronView.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("MainFrm.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("ChildFrm.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("pch.cpp"), 1, 1, hSrc);

	// Dockable pane source files
	m_wndFileView.InsertItem(_T("FileView.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("ClassView.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("OutputWnd.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("PropertiesWnd.cpp"), 1, 1, hSrc);

	// Dialog source files
	m_wndFileView.InsertItem(_T("SimConfigDlg.cpp"), 1, 1, hSrc);

	// Science module source files
	m_wndFileView.InsertItem(_T("Science\\Mass.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("Science\\Universe.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("Science\\Simulator.cpp"), 1, 1, hSrc);

	// ============================================================================
	// Header Files
	// ============================================================================
	HTREEITEM hInc = m_wndFileView.InsertItem(_T("Header Files"), 0, 0, hRoot);

	// Main application header files
	m_wndFileView.InsertItem(_T("Epistemotron.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("EpistemotronDoc.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("EpistemotronView.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("MainFrm.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("ChildFrm.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("framework.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("pch.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("Resource.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("targetver.h"), 2, 2, hInc);

	// Dockable pane header files
	m_wndFileView.InsertItem(_T("FileView.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("ClassView.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("OutputWnd.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("PropertiesWnd.h"), 2, 2, hInc);

	// Dialog header files
	m_wndFileView.InsertItem(_T("SimConfigDlg.h"), 2, 2, hInc);

	// Science module header files
	m_wndFileView.InsertItem(_T("Science\\Mass.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("Science\\Universe.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("Science\\Simulator.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("Science\\Environment.h"), 2, 2, hInc);

	// ============================================================================
	// Resource Files
	// ============================================================================
	HTREEITEM hRes = m_wndFileView.InsertItem(_T("Resource Files"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("Epistemotron.rc"), 3, 3, hRes);
	m_wndFileView.InsertItem(_T("Epistemotron.rc2"), 3, 3, hRes);

	// ============================================================================
	// Documentation Files
	// ============================================================================
	HTREEITEM hDoc = m_wndFileView.InsertItem(_T("Documentation Files"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("CLAUDE.md"), 4, 4, hDoc);
	m_wndFileView.InsertItem(_T("IMPROVEMENTS.md"), 4, 4, hDoc);
	m_wndFileView.InsertItem(_T("TODO.md"), 4, 4, hDoc);

	// ============================================================================
	// Build Files
	// ============================================================================
	HTREEITEM hBuild = m_wndFileView.InsertItem(_T("Build Files"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("CMakeLists.txt"), 5, 5, hBuild);
	m_wndFileView.InsertItem(_T("Epistemotron.sln"), 5, 5, hBuild);
	m_wndFileView.InsertItem(_T("Epistemotron.vcxproj"), 5, 5, hBuild);

	// Expand all nodes
	m_wndFileView.Expand(hRoot, TVE_EXPAND);
	m_wndFileView.Expand(hSrc, TVE_EXPAND);
	m_wndFileView.Expand(hInc, TVE_EXPAND);
	m_wndFileView.Expand(hRes, TVE_EXPAND);
	m_wndFileView.Expand(hDoc, TVE_EXPAND);
	m_wndFileView.Expand(hBuild, TVE_EXPAND);
}

void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = static_cast<CTreeCtrl*>(&m_wndFileView);
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	static const CPoint c_invalidPoint(-1, -1);
	if (point != c_invalidPoint)
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
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(nullptr,
		rectClient.left + c_nTreeMargin,
		rectClient.top + cyTlb + c_nTreeMargin,
		rectClient.Width() - 2 * c_nTreeMargin,
		rectClient.Height() - cyTlb - 2 * c_nTreeMargin,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

void CFileView::OnProperties()
{
	AfxMessageBox(_T("Properties"));
}

void CFileView::OnFileOpen()
{
	// Placeholder: Open the selected file in the editor
	// TODO: Implement file opening logic
}

void CFileView::OnFileOpenWith()
{
	// Placeholder: Open the selected file with external application
	// TODO: Implement "Open With" dialog and file handling
}

void CFileView::OnDummyCompile()
{
	// Placeholder: Trigger compilation of selected file(s)
	// TODO: Implement compilation trigger
}

void CFileView::OnEditCut()
{
	// Placeholder: Cut selected file(s) to clipboard
	// TODO: Implement cut operation for file tree items
}

void CFileView::OnEditCopy()
{
	// Placeholder: Copy selected file(s) to clipboard
	// TODO: Implement copy operation for file tree items
}

void CFileView::OnEditClear()
{
	// Placeholder: Delete selected file(s)
	// TODO: Implement delete/clear operation for file tree items
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(c_nTreeMargin, c_nTreeMargin);
	dc.Draw3dRect(rectTree, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// Focus goes to the tree control, not this pane
	CDockablePane::OnSetFocus(nullptr);
	m_wndFileView.SetFocus();
}

void CFileView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* Locked */);

	// Clear the old image list from the tree view before deleting
	m_wndFileView.SetImageList(nullptr, TVSIL_NORMAL);
	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		// Don't return early - leave image list as nullptr to prevent dangling pointer
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	if (!m_FileViewImages.Create(c_nToolBarHeight, bmpObj.bmHeight, nFlags, 0, 0))
	{
		TRACE(_T("Failed to create image list\n"));
		return;
	}

	m_FileViewImages.Add(&bmp, c_magentaMask);

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}


