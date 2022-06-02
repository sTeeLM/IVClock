// PageParam.cpp: 实现文件
//

#include "pch.h"
#include "IVClock.h"
#include "PageParam.h"
#include "afxdialogex.h"


// CPageParam 对话框

IMPLEMENT_DYNAMIC(CPageParam, CDialog)

CPageParam::CPageParam(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PROPPAGE_PARAM, pParent)
	, m_bTM12(FALSE)
	, m_bTmpCent(FALSE)
	, m_nTmrSND(0)
	, m_nPlyVol(0)
	, m_nPSTimeo(0)
	, m_bPS(FALSE)
	, m_bBS(FALSE)
	, m_nBSFrom(0)
	, m_nBSTo(0)
	, m_bBP(FALSE)
	, m_bMonLT(FALSE)
	, m_bAcc(FALSE)
	, m_nStepPowerTimeo(15)
	, m_bInProgress(FALSE)
{

}

CPageParam::~CPageParam()
{
}

void CPageParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_TM12, m_bTM12);
	DDX_Radio(pDX, IDC_RADIO_TMP_CENT, m_bTmpCent);
	DDX_CBIndex(pDX, IDC_COMBO_TMR_SND, m_nTmrSND);
	DDX_Slider(pDX, IDC_SLIDER_PLY_VOL, m_nPlyVol);
	DDX_CBIndex(pDX, IDC_COMBO_PS_TIMEO, m_nPSTimeo);
	DDX_Check(pDX, IDC_CHK_PS_EN, m_bPS);
	DDX_Check(pDX, IDC_CHK_BS_EN, m_bBS);
	DDX_CBIndex(pDX, IDC_COMBO_BS_FROM, m_nBSFrom);
	DDX_CBIndex(pDX, IDC_COMBO_BS_TO, m_nBSTo);
	DDX_Check(pDX, IDC_CHK_BP_EN, m_bBP);
	DDX_Check(pDX, IDC_CHK_MON_LT_EN, m_bMonLT);
	DDX_Check(pDX, IDC_CHK_ACC_EN, m_bAcc);
}


void CPageParam::UpdateUI()
{
	GetDlgItem(IDC_COMBO_PS_TIMEO)->EnableWindow(!m_bInProgress &&
		((CButton*)GetDlgItem(IDC_CHK_PS_EN))->GetCheck() == BST_CHECKED);
	GetDlgItem(IDC_COMBO_BS_FROM)->EnableWindow(!m_bInProgress &&
		((CButton*)GetDlgItem(IDC_CHK_BS_EN))->GetCheck() == BST_CHECKED);
	GetDlgItem(IDC_COMBO_BS_TO)->EnableWindow(!m_bInProgress &&
		((CButton*)GetDlgItem(IDC_CHK_BS_EN))->GetCheck() == BST_CHECKED);

	GetDlgItem(IDC_RADIO_TM12)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_RADIO_TM24)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_RADIO_TMP_CENT)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_RADIO_TMP_FAHR)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_SLIDER_PLY_VOL)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_CHK_PS_EN)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_CHK_BS_EN)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_COMBO_TMR_SND)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_CHK_BP_EN)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_CHK_MON_LT_EN)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_CHK_ACC_EN)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_BTN_SET)->EnableWindow(!m_bInProgress);
}

BOOL CPageParam::LoadRemoteConfig(CIVError& Error)
{
	remote_control_body_param_t param;

	if (!theApp.m_RemoteConfig.GetParam(Error, param))
		return FALSE;

	m_bTM12 = !param.time_12;
	m_bTmpCent = !param.temp_cen;

	// load IDC_COMBO_TMR_SND
	CComboBox* pTmrSndBox = (CComboBox*)GetDlgItem(IDC_COMBO_TMR_SND);
	for (INT i = pTmrSndBox->GetCount() - 1; i >= 0; i--)
	{
		pTmrSndBox->DeleteString(i);
	}
	CString strLabel;
	for (INT i = 0; i < param.tmr_snd_cnt; i++) {
		strLabel.Format(_T("sound %02d"), i);
		pTmrSndBox->AddString(strLabel);
	}

	m_nTmrSND = param.tmr_snd;

	// load IDC_SLIDER_PLY_VOL
	CSliderCtrl* pSlide = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PLY_VOL);
	pSlide->SetRangeMin(param.min_ply_vol, TRUE);
	pSlide->SetRangeMax(param.max_ply_vol, TRUE);

	m_nPlyVol = param.ply_vol;

	// load IDC_COMBO_PS_TIMEO
	CComboBox* pTmrPSTimeo = (CComboBox*)GetDlgItem(IDC_COMBO_PS_TIMEO);
	for (INT i = pTmrPSTimeo->GetCount() - 1; i >= 0; i--)
	{
		pTmrPSTimeo->DeleteString(i);
	}

	for (INT i = param.min_power_timeo;
		i <= param.max_power_timeo; i += param.step_power_timeo) {
		strLabel.Format(_T("%02d"), i);
		pTmrPSTimeo->AddString(strLabel);
	}

	m_nStepPowerTimeo = param.step_power_timeo;

	m_bPS = param.power_timeo != 0;
	if (param.step_power_timeo) {
		m_nPSTimeo = (param.power_timeo
			/ param.step_power_timeo) - 1;
	}

	m_bBS = param.alm1_en;
	m_nBSFrom = param.alm1_begin;
	m_nBSTo = param.alm1_end;
	m_bBP = param.bp_en;
	m_bMonLT = param.mon_lt_en;
	m_bAcc = param.acc_en;

	return TRUE;
}

BOOL CPageParam::SaveRemoteConfig(CIVError &Error)
{
	remote_control_body_param_t param;

	ZeroMemory(&param, sizeof(param));

	param.time_12 = !m_bTM12;
	param.temp_cen = !m_bTmpCent;

	param.tmr_snd = m_nTmrSND;
	param.ply_vol = m_nPlyVol;

	if (m_bPS) {
		param.power_timeo = (m_nPSTimeo + 1) * m_nStepPowerTimeo;
	}
	else {
		param.power_timeo = 0;
	}
	param.alm1_en = m_bBS;
	param.alm1_begin = m_nBSFrom;
	param.alm1_end = m_nBSTo;
	param.bp_en = m_bBP;
	param.mon_lt_en = m_bMonLT;
	param.acc_en = m_bAcc;

	return theApp.m_RemoteConfig.SetParam(Error, param);
}

BOOL CPageParam::OnInitDialog()
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

BEGIN_MESSAGE_MAP(CPageParam, CDialog)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_CB_SET_PARAM, cbSetParam)
	ON_BN_CLICKED(IDC_CHK_PS_EN, &CPageParam::OnBnClickedChkPsEn)
	ON_BN_CLICKED(IDC_CHK_BS_EN, &CPageParam::OnBnClickedChkBsEn)
	ON_BN_CLICKED(IDC_BTN_SET, &CPageParam::OnBnClickedBtnSet)
END_MESSAGE_MAP()


// CPageParam 消息处理程序

LRESULT CPageParam::cbSetParam(WPARAM wParam, LPARAM lParam)
{
	CTask* pTask = (CTask*)wParam;

	m_bInProgress = FALSE;
	UpdateUI();

	if (pTask) {
		if (!pTask->m_bRes) {
			AfxMessageBox(pTask->m_Error.GetErrorStr());
		}
	}
	return 0;
}

void CPageParam::OnBnClickedChkPsEn()
{
	UpdateUI();
}


void CPageParam::OnBnClickedChkBsEn()
{
	UpdateUI();
}

void CPageParam::OnBnClickedBtnSet()
{
	CIVError Error;

	UpdateData(TRUE);

	if (!SaveRemoteConfig(Error)) {
		AfxMessageBox(Error.GetErrorStr());
		return;
	}

	m_bInProgress = TRUE;

	UpdateUI();

	if (!theApp.m_RemoteConfig.AddTask(Error, CTask::IV_TASK_SET_PARAM, GetSafeHwnd(), WM_CB_SET_PARAM)) {
		AfxMessageBox(Error.GetErrorStr());
		m_bInProgress = FALSE;
		UpdateUI();
	}
}
