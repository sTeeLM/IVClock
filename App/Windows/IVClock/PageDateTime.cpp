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
	, m_hStopDateTimeThread(NULL)
	, m_bStopDateTimeThread(FALSE)
	, m_pLocalDateTimeThread(NULL)
	, m_bInProgress(FALSE)
	, m_oleDate(COleDateTime::GetCurrentTime())
	, m_oleTime(COleDateTime::GetCurrentTime())
	, m_oleLastSync(COleDateTime::GetCurrentTime())
	, m_oleLastRefresh(COleDateTime::GetCurrentTime())
{

}

CPageDateTime::~CPageDateTime()
{

}

UINT CPageDateTime::fnLocalDateTimeThread(LPVOID pParam)
{
	CPageDateTime* pThis = (CPageDateTime*)pParam;
	DWORD dwWaitRes;
	CIVError Error;
	COleDateTime oleNow;

	do {
		dwWaitRes = WaitForSingleObject(pThis->m_hStopDateTimeThread, 500);
		if (dwWaitRes == WAIT_FAILED) {
			Error.SetError(CIVError::IVE_INTERNAL);
			break;
		}

		if (pThis->m_bStopDateTimeThread) {
			break;
		}

		oleNow = COleDateTime::GetCurrentTime();
		CDateTimeCtrl* pDate = (CDateTimeCtrl*)pThis->GetDlgItem(IDC_DTP_LOCAL_DATE);
		CDateTimeCtrl* pTime = (CDateTimeCtrl*)pThis->GetDlgItem(IDC_DTP_LOCAL_TIME);
		pDate->SetTime(oleNow);
		pTime->SetTime(oleNow);

	} while (!pThis->m_bStopDateTimeThread);

	TRACE(_T("fnDateTimeThread quit!\n"));
	return 0;

}

BOOL CPageDateTime::StartLocalDateTimeThread(CIVError &Error)
{
	DWORD dwCreateFlag = CREATE_SUSPENDED;

	if (m_hStopDateTimeThread) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}

	m_bStopDateTimeThread = FALSE;

	if (!(m_hStopDateTimeThread = CreateEvent(NULL, TRUE, FALSE, _T("LocalDateTimeInc")))) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}

	m_pLocalDateTimeThread = AfxBeginThread(fnLocalDateTimeThread, this, 0, 0, dwCreateFlag, NULL);
	if (m_pLocalDateTimeThread) {
		m_pLocalDateTimeThread->m_bAutoDelete = FALSE;
		m_pLocalDateTimeThread->ResumeThread();
	}
	else {
		Error.SetError(CIVError::IVE_INTERNAL);
	}
	return m_pLocalDateTimeThread != NULL;
}

void CPageDateTime::StopLocalDateTimeThread()
{
	if (m_pLocalDateTimeThread) {
		m_bStopDateTimeThread = TRUE;
		SetEvent(m_hStopDateTimeThread);
		WaitForSingleObject(m_pLocalDateTimeThread->m_hThread, INFINITE);
		delete m_pLocalDateTimeThread;
		m_pLocalDateTimeThread = NULL;
		m_bStopDateTimeThread = FALSE;
	}

	if (m_hStopDateTimeThread) {
		ResetEvent(m_hStopDateTimeThread);
		CloseHandle(m_hStopDateTimeThread);
		m_hStopDateTimeThread = NULL;
	}
}

/*
1天;15天;30天;
*/
INT CPageDateTime::SyncIntervalToIndex(UINT uSyncIntervalSec)
{
	switch (uSyncIntervalSec) {
	case 86400:
		return 0;
	case 86400 * 15:
		return 1;
	case 86400 * 30:
		return 2;
	}
	return 0;
}

UINT CPageDateTime::IndexToSyncInterval(INT nIndex)
{
	switch (nIndex) {
	case 0:
		return 86400;
	case 1:
		return 86400 * 15;
	case 2:
		return 86400 * 30;
	}
	return 86400;
}

void CPageDateTime::UpdateUI()
{
	GetDlgItem(IDC_BTN_DATETIME_SYNC_NOW)->EnableWindow(!m_bInProgress && theApp.m_RemoteConfig.IsDateTimeValid());
	GetDlgItem(IDC_BTN_DATETIME_REFRESH)->EnableWindow(!m_bInProgress && theApp.m_RemoteConfig.IsDateTimeValid());
}

BOOL CPageDateTime::OnInitDialog()
{
	CIVError Error;
	CConfigManager::CONFIG_VALUE_T val;
	remote_control_body_time_t datetime;

	theApp.m_Config.GetConfig(_T("time_sync"), _T("enable"), val);

	m_bTMAutoSync = val.u8;

	theApp.m_Config.GetConfig(_T("time_sync"), _T("interval_sec"), val);

	m_nTMAutoSyncInterval = SyncIntervalToIndex(val.u32);

	if (theApp.m_RemoteConfig.IsDateTimeValid()) {
		if (theApp.m_RemoteConfig.GetDateTime(Error, datetime)) {
			m_oleDate.SetDateTime(
				datetime.year + 2000,
				datetime.mon,
				datetime.date,
				datetime.hour,
				datetime.min,
				datetime.sec
			);
			m_oleTime = m_oleDate;
		}
		else {
			AfxMessageBox(Error.GetErrorStr());
			return FALSE;
		}
	}

	if (!StartLocalDateTimeThread(Error)) {
		return FALSE;
	}

	if (!CDialog::OnInitDialog()) {
		return FALSE;
	}

	GetDlgItem(IDC_COMBO_DATETIME_SYNC_INTERVAL)->
		EnableWindow(((CButton*)GetDlgItem(IDC_CHK_DATETIME_AUTO_SYNC))->GetCheck() == BST_CHECKED);

	UpdateUI();

	return TRUE;
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
	ON_MESSAGE(WM_CB_SET_TIME, cbSetTimeDate)
	ON_BN_CLICKED(IDC_BTN_DATETIME_SYNC_NOW, &CPageDateTime::OnBnClickedBtnDatetimeSyncNow)
	ON_BN_CLICKED(IDC_CHK_DATETIME_AUTO_SYNC, &CPageDateTime::OnBnClickedChkDatetimeAutoSync)
	ON_BN_CLICKED(IDC_BTN_DATETIME_REFRESH, &CPageDateTime::OnBnClickedBtnDatetimeRefresh)
	ON_CBN_SELCHANGE(IDC_COMBO_DATETIME_SYNC_INTERVAL, &CPageDateTime::OnCbnSelchangeComboDatetimeSyncInterval)
END_MESSAGE_MAP()


// CPageDateTime 消息处理程序

LRESULT CPageDateTime::cbGetTimeDate(WPARAM wParam, LPARAM lParam)
{
	remote_control_body_time_t datetime;
	CTask* pTask = (CTask*)wParam;
	CIVError Error;


	if (pTask->m_bRes) {
		if (theApp.m_RemoteConfig.GetDateTime(Error, datetime)) {
			m_oleDate.SetDateTime(
				datetime.year + 2000,
				datetime.mon,
				datetime.date,
				datetime.hour,
				datetime.min,
				datetime.sec
			);
			m_oleTime = m_oleDate;
			CDateTimeCtrl* pDate = (CDateTimeCtrl*)GetDlgItem(IDC_DTP_DATE);
			CDateTimeCtrl* pTime = (CDateTimeCtrl*)GetDlgItem(IDC_DTP_TIME);
			pDate->SetTime(m_oleDate);
			pTime->SetTime(m_oleTime);
			GetDlgItem(IDC_EDIT_LAST_REFRESH)->SetWindowText(COleDateTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S")));
		}
		else {
			if (pTask->m_pParam)
				AfxMessageBox(Error.GetErrorStr());
			else {
				TRACE(_T("GetTimeDate Error:%s\n"), Error.GetErrorStr());
			}
		}
	}
	else {
		if(pTask->m_pParam)
			AfxMessageBox(pTask->m_Error.GetErrorStr());
		else {
			TRACE(_T("GetTimeDate Error:%s\n"), pTask->m_Error.GetErrorStr());
		}
	}
	m_bInProgress = FALSE;
	UpdateUI();
	return 0;
}


LRESULT CPageDateTime::cbSetTimeDate(WPARAM wParam, LPARAM lParam)
{
	CTask* pTask = (CTask*)wParam;
	CIVError Error;

	if (pTask->m_bRes) {
		m_oleTime = m_oleDate = m_oleLastSync;
		CDateTimeCtrl* pDate = (CDateTimeCtrl*)GetDlgItem(IDC_DTP_DATE);
		CDateTimeCtrl* pTime = (CDateTimeCtrl*)GetDlgItem(IDC_DTP_TIME);
		pDate->SetTime(m_oleDate);
		pTime->SetTime(m_oleTime);
		GetDlgItem(IDC_EDIT_LAST_SYNC)->SetWindowText(m_oleLastSync.Format(_T("%Y-%m-%d %H:%M:%S")));
	}
	else {
		if (pTask->m_pParam) {
			AfxMessageBox(pTask->m_Error.GetErrorStr());
		}
		else {
			TRACE(_T("cbSetTimeDate Error:%s\n"), Error.GetErrorStr());
		}
	}
	m_bInProgress = FALSE;
	UpdateUI();
	return 0;
}


void CPageDateTime::OnBnClickedBtnDatetimeRefresh()
{
	CIVError Error;
	if (!theApp.m_RemoteConfig.AddTask(Error, CTask::IV_TASK_GET_TIME, GetSafeHwnd(), WM_CB_GET_TIME, (LPVOID)1)) {
		AfxMessageBox(Error.GetErrorStr());
	}

	m_bInProgress = TRUE;
	UpdateUI();

}

void CPageDateTime::OnBnClickedBtnDatetimeSyncNow()
{
	CIVError Error;

	COleDateTimeSpan OneSec(0, 0, 0, 1); // 同步需要大约1S
	m_oleLastSync = COleDateTime::GetCurrentTime() + OneSec;

	if (!theApp.m_RemoteConfig.SetDateTime(Error, m_oleLastSync)) {
		AfxMessageBox(Error.GetErrorStr());
		return;
	}

	if (!theApp.m_RemoteConfig.AddTask(Error, CTask::IV_TASK_SET_TIME, GetSafeHwnd(), WM_CB_SET_TIME, (LPVOID)1)) {
		AfxMessageBox(Error.GetErrorStr());
		return;
	}

	m_bInProgress = TRUE;
	UpdateUI();
}

void CPageDateTime::OnDestroy()
{
	StopLocalDateTimeThread();
	CDialog::OnDestroy();
}


void CPageDateTime::Save()
{
	CConfigManager::CONFIG_VALUE_T val;

	val.u8 = m_bTMAutoSync;
	theApp.m_Config.SetConfig(_T("time_sync"), _T("enable"), val);

	val.u32 = IndexToSyncInterval(m_nTMAutoSyncInterval);
	theApp.m_Config.SetConfig(_T("time_sync"), _T("interval_sec"), val);

}

void CPageDateTime::OnBnClickedChkDatetimeAutoSync()
{
	
	GetDlgItem(IDC_COMBO_DATETIME_SYNC_INTERVAL)->
		EnableWindow(((CButton*)GetDlgItem(IDC_CHK_DATETIME_AUTO_SYNC))->GetCheck() == BST_CHECKED);
	
	UpdateData(TRUE);
	Save();
}





void CPageDateTime::OnCbnSelchangeComboDatetimeSyncInterval()
{
	UpdateData(TRUE);
	Save();
}
