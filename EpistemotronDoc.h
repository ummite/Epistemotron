
// EpistemotronDoc.h : interface of the CEpistemotronDoc class
//

#pragma once

class Universe;  // Forward declaration

class CEpistemotronDoc : public CDocument
{
protected:
	CEpistemotronDoc() noexcept;
	DECLARE_DYNCREATE(CEpistemotronDoc)

// Attributes
public:
	Universe* m_pCurrentUniverse;  // Current simulation state

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CEpistemotronDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
