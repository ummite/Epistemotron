
#include "pch.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd() noexcept
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndOutputBuild.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndOutputDebug.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
		!m_wndOutputFind.Create(dwStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// Attach list windows to tab:
	bNameValid = strTabName.LoadString(IDS_BUILD_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputBuild, strTabName, (UINT)0);
	bNameValid = strTabName.LoadString(IDS_DEBUG_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputDebug, strTabName, (UINT)1);
	bNameValid = strTabName.LoadString(IDS_FIND_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputFind, strTabName, (UINT)2);

	// Fill output tabs with some dummy text (nothing magic here)
	FillBuildWindow();
	FillDebugWindow();
	FillFindWindow();

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos (nullptr, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);
	if (pOldFont == nullptr)
	{
		return;
	}

	int cxExtentMax = 0;
	int nCount = wndListBox.GetCount();

	// Cache text in a single CString to avoid repeated allocations
	CString strItem;
	strItem.Empty();

	for (int i = 0; i < nCount; i ++)
	{
		wndListBox.GetText(i, strItem);
		cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillBuildWindow()
{
	m_wndOutputBuild.AddString(_T("Build output is being displayed here."));
	m_wndOutputBuild.AddString(_T("The output is being displayed in rows of a list view"));
	m_wndOutputBuild.AddString(_T("but you can change the way it is displayed as you wish..."));
}

void COutputWnd::FillDebugWindow()
{
	m_wndOutputDebug.AddString(_T("Debug output is being displayed here."));
	m_wndOutputDebug.AddString(_T("The output is being displayed in rows of a list view"));
	m_wndOutputDebug.AddString(_T("but you can change the way it is displayed as you wish..."));
}

void COutputWnd::FillFindWindow()
{
	m_wndOutputFind.AddString(_T("Find output is being displayed here."));
	m_wndOutputFind.AddString(_T("The output is being displayed in rows of a list view"));
	m_wndOutputFind.AddString(_T("but you can change the way it is displayed as you wish..."));
}

void COutputWnd::UpdateFonts()
{
	// Save old fonts to ensure proper cleanup
	CFont* pOldFontBuild = m_wndOutputBuild.GetFont();
	CFont* pOldFontDebug = m_wndOutputDebug.GetFont();
	CFont* pOldFontFind = m_wndOutputFind.GetFont();

	m_wndOutputBuild.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputDebug.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputFind.SetFont(&afxGlobalData.fontRegular);

	// Note: afxGlobalData.fontRegular is a shared system font,
	// so we don't delete pOldFont* - the list boxes manage their own font resources
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList() noexcept
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList message handlers

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	if (!menu.LoadMenu(IDR_OUTPUT_POPUP))
		return;

	CMenu* pSumMenu = menu.GetSubMenu(0);
	if (pSumMenu == nullptr)
		return;

	// Check for null main window to prevent crash
	CWnd* pMainWnd = AfxGetMainWnd();
	if (pMainWnd == nullptr)
		return;

	if (pMainWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
		{
			delete pPopupMenu;  // Prevent memory leak
			return;
		}

		// OnShowPopupMenu takes ownership of pPopupMenu
		((CMDIFrameWndEx*)pMainWnd)->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	// Copy selected items text to clipboard
	int nSelCount = GetSelCount();
	if (nSelCount <= 0)
	{
		TRACE0("COutputList::OnEditCopy: No text selected\n");
		return;
	}

	CString strText;
	int nIndex = GetCurSel();
	if (nIndex != LB_ERR)
	{
		GetText(nIndex, strText);
	}

	if (strText.IsEmpty())
	{
		return;
	}

	// CF_UNICODETEXT always requires wide character (UTF-16) format
	// Allocate (string length + 1) * 2 bytes for null-terminated wide string
	size_t allocSize = (strText.GetLength() + 1) * sizeof(WCHAR);
	HGLOBAL hMem = GlobalAlloc(GMEM_DDESHARE, allocSize);
	if (hMem == nullptr)
	{
		TRACE0("COutputList::OnEditCopy: Failed to allocate memory for clipboard data\n");
		return;
	}

	WCHAR* pch = (WCHAR*)GlobalLock(hMem);
	if (pch == nullptr)
	{
		TRACE0("COutputList::OnEditCopy: Failed to lock memory for clipboard data\n");
		GlobalFree(hMem);
		return;
	}

	// Convert CString to wide string with bounds checking
	size_t strLen = strText.GetLength();
	// wcsncpy_s is safe when we allocated (strLen + 1) * sizeof(WCHAR)
	wcsncpy_s(pch, allocSize / sizeof(WCHAR), strText, strLen);
	pch[strLen] = L'\0';  // Ensure null termination
	GlobalUnlock(hMem);

	if (!OpenClipboard())
	{
		DWORD errorCode = GetLastError();
		TRACE1("COutputList::OnEditCopy: Failed to open clipboard. Error: %lu\n", errorCode);
		GlobalFree(hMem);
		return;
	}

	if (!EmptyClipboard())
	{
		DWORD errorCode = GetLastError();
		TRACE1("COutputList::OnEditCopy: Failed to empty clipboard. Error: %lu\n", errorCode);
		CloseClipboard();
		GlobalFree(hMem);
		return;
	}

	if (!SetClipboardData(CF_UNICODETEXT, hMem))
	{
		DWORD errorCode = GetLastError();
		TRACE1("COutputList::OnEditCopy: Failed to set clipboard data. Error: %lu\n", errorCode);
		CloseClipboard();
		GlobalFree(hMem);
		return;
	}

	CloseClipboard();
}

void COutputList::OnEditClear()
{
	// Clear all items from the list box
	ResetContent();
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != nullptr && pParentBar != nullptr)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}
