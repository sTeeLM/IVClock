// PageInfo.cpp: 实现文件
//

#include "pch.h"
#include "IVClock.h"
#include "PageInfo.h"
#include "afxdialogex.h"


// CPageInfo 对话框

IMPLEMENT_DYNAMIC(CPageInfo, CDialog)

CPageInfo::CPageInfo(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PROPPAGE_INFO, pParent)
{

}

CPageInfo::~CPageInfo()
{
}

void CPageInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageInfo, CDialog)
END_MESSAGE_MAP()


// CPageInfo 消息处理程序
