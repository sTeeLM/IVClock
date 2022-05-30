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
	, m_bTMAutoSync(FALSE)
	, m_nTMAutoSyncInterval(0)
	, m_oleDate(COleDateTime::GetCurrentTime())
	, m_oleTime(COleDateTime::GetCurrentTime())
{

}

CPageDateTime::~CPageDateTime()
{

}

BOOL CPageDateTime::OnInitDialog()
{
	if (!CDialog::OnInitDialog()) {
		return FALSE;
	}
	return TRUE;
}

void CPageDateTime::OnOK()
{
	CDialog::OnOK();
}
void CPageDateTime::OnCancel()
{
	CDialog::OnCancel();
}

void CPageDateTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_DATETIME_AUTO_SYNC, m_bTMAutoSync);
	DDX_CBIndex(pDX, IDC_COMBO_DATETIME_SYNC_INTERVAL, m_nTMAutoSyncInterval);
	DDX_DateTimeCtrl(pDX, IDC_DTP_DATE, m_oleDate);
	DDX_DateTimeCtrl(pDX, IDC_DTP_TIME, m_oleTime);
}


BEGIN_MESSAGE_MAP(CPageDateTime, CDialog)
	ON_BN_CLICKED(IDC_BTN_DATETIME_SYNC_NOW, &CPageDateTime::OnBnClickedBtnDatetimeSyncNow)
END_MESSAGE_MAP()


// CPageDateTime 消息处理程序


void CPageDateTime::OnBnClickedBtnDatetimeSyncNow()
{
	// TODO: 在此添加控件通知处理程序代码
}
