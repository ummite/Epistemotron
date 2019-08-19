#pragma once


// Picture dialog

class Picture : public CDialogEx
{
	DECLARE_DYNAMIC(Picture)

public:
	Picture(CWnd* pParent = nullptr);   // standard constructor
	virtual ~Picture();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PICTURE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
