// Picture.cpp : implementation file
//

#include "pch.h"
#include "Epistemotron.h"
#include "Picture.h"
#include "afxdialogex.h"


// Picture dialog

IMPLEMENT_DYNAMIC(Picture, CDialogEx)

Picture::Picture(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PICTURE, pParent)
{

}

Picture::~Picture()
{
}

void Picture::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Picture, CDialogEx)
END_MESSAGE_MAP()


// Picture message handlers
