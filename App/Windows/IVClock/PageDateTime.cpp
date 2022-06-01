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
	, m_oleLastSync(COleDateTime::GetCurrentTime())
{

}

CPageDateTime::~CPageDateTime()
{

}

/*
UINT CPageDateTime::fnDateTimeThread(LPVOID pParam)
{
	CPageDateTime* pThis = (CPageDateTime*)pParam;
	DWORD dwWaitRes;
	CIVError Error;
	COleDateTime oleNow;
	do {
		dwWaitRes = WaitForSingleObject(pThis->m_hStopDateTimeThread, 10000);
		if (dwWaitRes == WAIT_FAILED) {
			Error.SetError(CIVError::IVE_INTERNAL);
			break;
		}

		if (pThis->m_bStopDateTimeThread) {
			break;
		}

		oleNow = COleDateTime::GetCurrentTime();

		if (!pThis->RefreshDateTime(Error)) {
			TRACE(_T("RefreshDateTime Error %s\n"), Error.GetErrorStr());
		}

	} while (!pThis->m_bStopDateTimeThread);

	TRACE(_T("fnDateTimeThread quit!\n"));
	return 0;

}

BOOL CPageDateTime::StartDateTimeThread(CIVError &Error)
{
	DWORD dwCreateFlag = CREATE_SUSPENDED;

	if (m_hStopDateTimeThread) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}

	m_bStopDateTimeThread = FALSE;

	if (!(m_hStopDateTimeThread = CreateEvent(NULL, TRUE, FALSE, _T("DateTimeInc")))) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}

	m_pDateTimeThread = AfxBeginThread(fnDateTimeThread, this, 0, 0, dwCreateFlag, NULL);
	if (m_pDateTimeThread) {
		m_pDateTimeThread->m_bAutoDelete = FALSE;
		m_pDateTimeThread->ResumeThread();
	}
	else {
		Error.SetError(CIVError::IVE_INTERNAL);
	}
	return m_pDateTimeThread != NULL;
}

void CPageDateTime::StopDateTimeThread()
{
	if (m_pDateTimeThread) {
		m_bStopDateTimeThread = TRUE;
		SetEvent(m_hStopDateTimeThread);
		WaitForSingleObject(m_pDateTimeThread->m_hThread, INFINITE);
		delete m_pDateTimeThread;
		m_pDateTimeThread = NULL;
		m_bStopDateTimeThread = FALSE;
	}

	if (m_hStopDateTimeThread) {
		ResetEvent(m_hStopDateTimeThread);
		CloseHandle(m_hStopDateTimeThread);
		m_hStopDateTimeThread = NULL;
	}
}

BOOL CPageDateTime::RefreshDateTime(CIVError& Error)
{
	DWORD dwWaitRes;
	BOOL bRet = FALSE;

	dwWaitRes = WaitForSingleObject(m_hOleDateMutex, INFINITE);
	if (dwWaitRes == WAIT_FAILED) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return bRet;
	}

	if (theApp.m_RemoteConfig.LoadRemoteConfigDateTime(Error, m_hStopDateTimeThread)) {
		m_oleDate.SetDateTime(
			theApp.m_RemoteConfig.GetDateTime().year + 2000,
			theApp.m_RemoteConfig.GetDateTime().mon,
			theApp.m_RemoteConfig.GetDateTime().date,
			theApp.m_RemoteConfig.GetDateTime().hour,
			theApp.m_RemoteConfig.GetDateTime().min,
			theApp.m_RemoteConfig.GetDateTime().sec
		);
		m_oleTime = m_oleDate;
		CDateTimeCtrl* pDate = (CDateTimeCtrl*)GetDlgItem(IDC_DTP_DATE);
		CDateTimeCtrl* pTime = (CDateTimeCtrl*)GetDlgItem(IDC_DTP_TIME);
		pDate->SetTime(m_oleDate);
		pTime->SetTime(m_oleTime);
		bRet = TRUE;
	}

	ReleaseMutex(m_hOleDateMutex);

	return bRet;
}

BOOL CPageDateTime::SyncDateTime(CIVError& Error)
{
	DWORD dwWaitRes;
	BOOL bRet = FALSE;

	dwWaitRes = WaitForSingleObject(m_hOleDateMutex, INFINITE);

	if (dwWaitRes == WAIT_FAILED) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return bRet;
	}

	COleDateTimeSpan OneSec(0, 0, 0, 1); // 同步需要大约1S
	m_oleDate = m_oleTime = COleDateTime::GetCurrentTime() + OneSec;

	theApp.m_RemoteConfig.GetDateTime().year = m_oleDate.GetYear() - 2000;
	theApp.m_RemoteConfig.GetDateTime().mon = m_oleDate.GetMonth();
	theApp.m_RemoteConfig.GetDateTime().date = m_oleDate.GetDay();
	theApp.m_RemoteConfig.GetDateTime().hour = m_oleDate.GetHour();
	theApp.m_RemoteConfig.GetDateTime().min = m_oleDate.GetMinute();
	theApp.m_RemoteConfig.GetDateTime().sec = m_oleDate.GetSecond();

	if (theApp.m_RemoteConfig.SetRemoteConfigDateTime(Error)) {
		// 也顺手刷新本地显示
		CDateTimeCtrl* pDate = (CDateTimeCtrl*)GetDlgItem(IDC_DTP_DATE);
		CDateTimeCtrl* pTime = (CDateTimeCtrl*)GetDlgItem(IDC_DTP_TIME);
		pDate->SetTime(m_oleDate);
		pTime->SetTime(m_oleTime);
		bRet = TRUE;
	}

	ReleaseMutex(m_hOleDateMutex);

	return bRet;
}
*/
BOOL CPageDateTime::OnInitDialog()
{
	CIVError Error;
	CConfigManager::CONFIG_VALUE_T val;
	remote_control_body_time_t datetime;

	theApp.m_RemoteConfig.GetDateTime(Error, datetime);

	theApp.m_Config.GetConfig(_T("time_sync"), _T("enable"), val);

	m_bTMAutoSync = val.u8;

	theApp.m_Config.GetConfig(_T("time_sync"), _T("interval_sec"), val);

	m_nTMAutoSyncInterval = val.u8;

	m_oleDate.SetDateTime(
		datetime.year + 2000,
		datetime.mon,
		datetime.date,
		datetime.hour,
		datetime.min,
		datetime.sec
	);
	m_oleTime = m_oleDate;

	if (!CDialog::OnInitDialog()) {
		return FALSE;
	}

	GetDlgItem(IDC_COMBO_DATETIME_SYNC_INTERVAL)->
		EnableWindow(((CButton*)GetDlgItem(IDC_CHK_DATETIME_AUTO_SYNC))->GetCheck() == BST_CHECKED);

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
	ON_WM_DESTROY()
	ON_MESSAGE(WM_CB_GET_TIME, cbGetTimeDate)
	ON_MESSAGE(WM_CB_GET_TIME, cbSetTimeDate)
	ON_BN_CLICKED(IDC_BTN_DATETIME_SYNC_NOW, &CPageDateTime::OnBnClickedBtnDatetimeSyncNow)
	ON_BN_CLICKED(IDC_CHK_DATETIME_AUTO_SYNC, &CPageDateTime::OnBnClickedChkDatetimeAutoSync)
	ON_BN_CLICKED(IDC_BTN_DATETIME_REFRESH, &CPageDateTime::OnBnClickedBtnDatetimeRefresh)
END_MESSAGE_MAP()


// CPageDateTime 消息处理程序

LRESULT CPageDateTime::cbGetTimeDate(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


LRESULT CPageDateTime::cbSetTimeDate(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


void CPageDateTime::OnBnClickedBtnDatetimeRefresh()
{
	CIVError Error;
	theApp.m_RemoteConfig.AddTask(Error, CTask::IV_TASK_GET_TIME, GetSafeHwnd(), WM_CB_GET_TIME);

}

void CPageDateTime::OnBnClickedBtnDatetimeSyncNow()
{
	CIVError Error;
	theApp.m_RemoteConfig.AddTask(Error, CTask::IV_TASK_GET_TIME, GetSafeHwnd(), WM_CB_SET_TIME);

}

void CPageDateTime::OnDestroy()
{
	CDialog::OnDestroy();
}


void CPageDateTime::Save()
{
	CConfigManager::CONFIG_VALUE_T val;

	val.u8 = m_bTMAutoSync;
	theApp.m_Config.SetConfig(_T("time_sync"), _T("enable"), val);

	val.u8 = m_nTMAutoSyncInterval;
	theApp.m_Config.SetConfig(_T("time_sync"), _T("interval_sec"), val);

}

void CPageDateTime::OnBnClickedChkDatetimeAutoSync()
{
	GetDlgItem(IDC_COMBO_DATETIME_SYNC_INTERVAL)->
		EnableWindow(((CButton*)GetDlgItem(IDC_CHK_DATETIME_AUTO_SYNC))->GetCheck() == BST_CHECKED);
}



