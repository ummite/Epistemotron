
// EpistemotronView.cpp : implementation of the CEpistemotronView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Epistemotron.h"
#endif

#include "EpistemotronDoc.h"
#include "EpistemotronView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEpistemotronView

IMPLEMENT_DYNCREATE(CEpistemotronView, CView)

BEGIN_MESSAGE_MAP(CEpistemotronView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CEpistemotronView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CEpistemotronView construction/destruction

CEpistemotronView::CEpistemotronView() noexcept
{
	// TODO: add construction code here

}

CEpistemotronView::~CEpistemotronView()
{
}

BOOL CEpistemotronView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CEpistemotronView drawing

void CEpistemotronView::OnDraw(CDC* /*pDC*/)
{
	CEpistemotronDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CEpistemotronView printing


void CEpistemotronView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CEpistemotronView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CEpistemotronView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CEpistemotronView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CEpistemotronView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CEpistemotronView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CEpistemotronView diagnostics

#ifdef _DEBUG
void CEpistemotronView::AssertValid() const
{
	CView::AssertValid();
}

void CEpistemotronView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEpistemotronDoc* CEpistemotronView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEpistemotronDoc)));
	return (CEpistemotronDoc*)m_pDocument;
}
#endif //_DEBUG


// CEpistemotronView message handlers
