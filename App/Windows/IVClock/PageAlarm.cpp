// PageAlarm.cpp: 实现文件
//

#include "pch.h"
#include "IVClock.h"
#include "PageAlarm.h"
#include "afxdialogex.h"


// CPageAlarm 对话框

IMPLEMENT_DYNAMIC(CPageAlarm, CDialog)

CPageAlarm::CPageAlarm(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PROPPAGE_ALARM, pParent)
{

}

CPageAlarm::~CPageAlarm()
{
}

void CPageAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageAlarm, CDialog)
END_MESSAGE_MAP()


// CPageAlarm 消息处理程序
