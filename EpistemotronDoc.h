
// EpistemotronDoc.h : interface of the CEpistemotronDoc class
//

#pragma once

class Universe;  // Forward declaration

class CEpistemotronDoc : public CDocument
{
protected:
	CEpistemotronDoc() noexcept;
	DECLARE_DYNCREATE(CEpistemotronDoc)

// Copy semantics (Rule of Five)
public:
	CEpistemotronDoc(const CEpistemotronDoc& src);
	CEpistemotronDoc& operator=(const CEpistemotronDoc& src);
	CEpistemotronDoc(CEpistemotronDoc&& src) noexcept;
	CEpistemotronDoc& operator=(CEpistemotronDoc&& src) noexcept;

// Attributes
public:
	Universe* GetUniverse() const { return m_pCurrentUniverse; }
	void SetUniverse(Universe* pUniverse) { m_pCurrentUniverse = pUniverse; }
	Universe* m_pCurrentUniverse;  // Current simulation state - public for direct access

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
