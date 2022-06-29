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
	, m_bInProgress(FALSE)
	, m_nAlarmHour(0)
	, m_nAlarmMin(0)
	, m_nAlarmSnd(0)
{

}

CPageAlarm::~CPageAlarm()
{
}

BOOL CPageAlarm::OnInitDialog()
{
	CIVError Error;
	if (!LoadRemoteConfig(Error)) {
		return FALSE;
	}

	if (!CDialog::OnInitDialog()) {
		return FALSE;
	}

	UpdateUI();

	return TRUE;
}

void CPageAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_ALARM_INDEX, m_nAlarmIndex);
	DDX_CBIndex(pDX, IDC_COMBO_ALARM_HOUR, m_nAlarmHour);
	DDX_CBIndex(pDX, IDC_COMBO_ALARM_MIN, m_nAlarmMin);
	DDX_CBIndex(pDX, IDC_COMBO_ALARM_SND, m_nAlarmSnd);
}

BOOL CPageAlarm::SaveRemoteConfig(CIVError& Error)
{
	remote_control_body_alarm_t alarm;

	UpdateData(TRUE);

	ZeroMemory(&alarm, sizeof(alarm));

	alarm.hour = m_nAlarmHour;
	alarm.min  = m_nAlarmMin;
	alarm.snd  = m_nAlarmSnd;
	alarm.alarm_index = m_nAlarmIndex;

	for (UINT i = IDC_CHK_ALARM_DAY7; i >= IDC_CHK_ALARM_DAY1; i--) {
		alarm.day_mask <<= 1;
		alarm.day_mask |= (((CButton*)GetDlgItem(i))->GetCheck() == BST_CHECKED) ? 1 : 0;
	}

	return theApp.m_RemoteConfig.SetAlarm(Error, alarm);
}

BOOL CPageAlarm::LoadRemoteConfig(CIVError& Error)
{
	// load IDC_COMBO_ALARM_INDEX
	CString strLabel;
	remote_control_body_alarm_t alarm;

	if (!theApp.m_RemoteConfig.IsAlarmValid()) {
		return TRUE;
	}

	CComboBox* pAlarmIndexBox = (CComboBox*)GetDlgItem(IDC_COMBO_ALARM_INDEX);
	for (INT i = pAlarmIndexBox->GetCount() - 1; i >= 0; i--)
	{
		pAlarmIndexBox->DeleteString(i);
	}
	for (INT i = 0; i < theApp.m_RemoteConfig.GetAlarmCnt(); i++) {
		strLabel.Format(_T("Alarm %02d"), i);
		pAlarmIndexBox->AddString(strLabel);
	}

	// load IDC_COMBO_ALARM_HOUR
	CComboBox* pAlarmHourBox = (CComboBox*)GetDlgItem(IDC_COMBO_ALARM_HOUR);
	for (INT i = pAlarmHourBox->GetCount() - 1; i >= 0; i--)
	{
		pAlarmHourBox->DeleteString(i);
	}
	for (INT i = 0; i < 24; i++) {
		strLabel.Format(_T("%02d"), i);
		pAlarmHourBox->AddString(strLabel);
	}

	// load IDC_COMBO_ALARM_MIN
	CComboBox* pAlarmMinBox = (CComboBox*)GetDlgItem(IDC_COMBO_ALARM_MIN);
	for (INT i = pAlarmMinBox->GetCount() - 1; i >= 0; i--)
	{
		pAlarmMinBox->DeleteString(i);
	}
	for (INT i = 0; i < 60; i++) {
		strLabel.Format(_T("%02d"), i);
		pAlarmMinBox->AddString(strLabel);
	}

	// load IDC_COMBO_ALARM_SND
	CComboBox* pAlarmSndBox = (CComboBox*)GetDlgItem(IDC_COMBO_ALARM_SND);
	for (INT i = pAlarmSndBox->GetCount() - 1; i >= 0; i--)
	{
		pAlarmSndBox->DeleteString(i);
	}
	for (INT i = 0; i < theApp.m_RemoteConfig.GetAlarmSndCnt(); i++) {
		strLabel.Format(_T("snd %02d"), i);
		pAlarmSndBox->AddString(strLabel);
	}

	if (theApp.m_RemoteConfig.GetAlarmCnt() > 0) {
		if (!theApp.m_RemoteConfig.GetAlarm(Error, alarm, m_nAlarmIndex)) {
			return FALSE;
		}
		SetAlarmDisplay(alarm);
		m_nAlarmHour = alarm.hour;
		m_nAlarmMin  = alarm.min;
		m_nAlarmSnd  = alarm.snd;
	}

	return TRUE;
}

void CPageAlarm::UpdateUI()
{
	for (UINT i = IDC_CHK_ALARM_DAY1; i <= IDC_CHK_ALARM_DAY7; i++) {
		GetDlgItem(i)->EnableWindow(!m_bInProgress && theApp.m_RemoteConfig.IsAlarmValid());
	}
	GetDlgItem(IDC_COMBO_ALARM_INDEX)->EnableWindow(!m_bInProgress && theApp.m_RemoteConfig.IsAlarmValid());
	GetDlgItem(IDC_COMBO_ALARM_HOUR)->EnableWindow(!m_bInProgress && theApp.m_RemoteConfig.IsAlarmValid());
	GetDlgItem(IDC_COMBO_ALARM_MIN)->EnableWindow(!m_bInProgress && theApp.m_RemoteConfig.IsAlarmValid());
	GetDlgItem(IDC_COMBO_ALARM_SND)->EnableWindow(!m_bInProgress && theApp.m_RemoteConfig.IsAlarmValid());
	GetDlgItem(IDC_BTN_SET)->EnableWindow(!m_bInProgress && theApp.m_RemoteConfig.IsAlarmValid());
	GetDlgItem(IDC_BTN_ALARM_SEL_ALL)->EnableWindow(!m_bInProgress && theApp.m_RemoteConfig.IsAlarmValid());
	GetDlgItem(IDC_BTN_ALARM_SEL_NONE)->EnableWindow(!m_bInProgress && theApp.m_RemoteConfig.IsAlarmValid());
}


BEGIN_MESSAGE_MAP(CPageAlarm, CDialog)
	ON_MESSAGE(WM_CB_SET_ALARM, cbSetAlarm)
	ON_MESSAGE(WM_CB_GET_ALARM, cbGetAlarm)
	ON_BN_CLICKED(IDC_BTN_ALARM_SEL_ALL, &CPageAlarm::OnBnClickedBtnAlarmSelAll)
	ON_BN_CLICKED(IDC_BTN_ALARM_SEL_NONE, &CPageAlarm::OnBnClickedBtnAlarmSelNone)
	ON_BN_CLICKED(IDC_BTN_SET, &CPageAlarm::OnBnClickedBtnSet)
	ON_CBN_SELCHANGE(IDC_COMBO_ALARM_INDEX, &CPageAlarm::OnCbnSelchangeComboAlarmIndex)
END_MESSAGE_MAP()


// CPageAlarm 消息处理程序

LRESULT CPageAlarm::cbGetAlarm(WPARAM wParam, LPARAM lParam)
{
	CTask* pTask = (CTask*)wParam;
	CIVError Error;
	if (pTask->m_bRes) {
		if (!LoadRemoteConfig(Error)){
			TRACE(_T("GetAlarm failed: %s\n"), Error.GetErrorStr());
		}
	}
	else {
		TRACE(_T("GetAlarm failed: %s\n"), pTask->m_Error.GetErrorStr());
	}
	m_bInProgress = FALSE;
	UpdateUI();
	return 0;
}

LRESULT CPageAlarm::cbSetAlarm(WPARAM wParam, LPARAM lParam)
{
	CTask* pTask = (CTask*)wParam;

	if (!pTask->m_bRes) {
		if(pTask->m_pParam)
			AfxMessageBox(pTask->m_Error.GetErrorStr());
		else {
			TRACE(_T("Set Alarm Error: %s\n"), pTask->m_Error.GetErrorStr());
		}
	}

	m_bInProgress = FALSE;
	UpdateUI();

	return 0;
}


void CPageAlarm::OnBnClickedBtnAlarmSelAll()
{
	for (UINT i = IDC_CHK_ALARM_DAY1; i <= IDC_CHK_ALARM_DAY7; i++) {
		((CButton*)GetDlgItem(i))->SetCheck(BST_CHECKED);
	}
}


void CPageAlarm::OnBnClickedBtnAlarmSelNone()
{
	for (UINT i = IDC_CHK_ALARM_DAY1; i <= IDC_CHK_ALARM_DAY7; i++) {
		((CButton*)GetDlgItem(i))->SetCheck(BST_UNCHECKED);
	}
}


void CPageAlarm::OnBnClickedBtnSet()
{
	CIVError Error;

	if (!SaveRemoteConfig(Error)) {
		AfxMessageBox(Error.GetErrorStr());
		return;
	}

	m_bInProgress = TRUE;
	UpdateUI();

	if (!theApp.m_RemoteConfig.AddTask(Error, CTask::IV_TASK_SET_ALARM, GetSafeHwnd(), WM_CB_SET_ALARM,(LPVOID)1)) {
		AfxMessageBox(Error.GetErrorStr());
		m_bInProgress = FALSE;
		UpdateUI();
	}


}

void CPageAlarm::SetAlarmDisplay(const remote_control_body_alarm_t &alarm)
{
	for (UINT i = IDC_CHK_ALARM_DAY1; i <= IDC_CHK_ALARM_DAY7; i++) {
		((CButton*)GetDlgItem(i))->SetCheck(((alarm.day_mask >> (i - IDC_CHK_ALARM_DAY1)) & 0x1) ? BST_CHECKED : BST_UNCHECKED);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_ALARM_INDEX))->SetCurSel(alarm.alarm_index);
	((CComboBox*)GetDlgItem(IDC_COMBO_ALARM_HOUR))->SetCurSel(alarm.hour);
	((CComboBox*)GetDlgItem(IDC_COMBO_ALARM_MIN))->SetCurSel(alarm.min);
	((CComboBox*)GetDlgItem(IDC_COMBO_ALARM_SND))->SetCurSel(alarm.snd);
}

void CPageAlarm::OnCbnSelchangeComboAlarmIndex()
{
	INT nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_ALARM_INDEX))->GetCurSel();
	CIVError Error;
	remote_control_body_alarm_t alarm;

	alarm.alarm_index = nIndex;
	if (!theApp.m_RemoteConfig.SetAlarm(Error, alarm)) {
		AfxMessageBox(Error.GetErrorStr());
		return;
	}

	if (!theApp.m_RemoteConfig.AddTask(Error, CTask::IV_TASK_GET_ALARM, GetSafeHwnd(), WM_CB_GET_ALARM, (LPVOID)1)) {
		AfxMessageBox(Error.GetErrorStr());
		return;
	}

	m_bInProgress = TRUE;
	UpdateUI();
}
