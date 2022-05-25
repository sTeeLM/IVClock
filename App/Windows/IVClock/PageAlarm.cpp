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
	, m_nAlarmIndex(0)
	, m_oleAlarmTime(COleDateTime::GetCurrentTime())
{

}

CPageAlarm::~CPageAlarm()
{
}

void CPageAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_ALARM_INDEX, m_nAlarmIndex);
	DDX_DateTimeCtrl(pDX, IDC_DTP_ALARM_TIME, m_oleAlarmTime);
}


BEGIN_MESSAGE_MAP(CPageAlarm, CDialog)
	ON_BN_CLICKED(IDC_BTN_ALARM_SEL_ALL, &CPageAlarm::OnBnClickedBtnAlarmSelAll)
	ON_BN_CLICKED(IDC_BTN_ALARM_SEL_NONE, &CPageAlarm::OnBnClickedBtnAlarmSelNone)
END_MESSAGE_MAP()


// CPageAlarm 消息处理程序


void CPageAlarm::OnBnClickedBtnAlarmSelAll()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CPageAlarm::OnBnClickedBtnAlarmSelNone()
{
	// TODO: 在此添加控件通知处理程序代码
}
