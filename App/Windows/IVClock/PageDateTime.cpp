// PageDateTime.cpp: 实现文件
//

#include "pch.h"
#include "IVClock.h"
#include "PageDateTime.h"
#include "afxdialogex.h"


// CPageDateTime 对话框

IMPLEMENT_DYNAMIC(CPageDateTime, CDialog)

CPageDateTime::CPageDateTime(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PROPPAGE_DATETIME, pParent)
{

}

CPageDateTime::~CPageDateTime()
{
}

void CPageDateTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageDateTime, CDialog)
END_MESSAGE_MAP()


// CPageDateTime 消息处理程序
