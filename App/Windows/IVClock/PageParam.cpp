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
	GetDlgItem(IDC_COMBO_PS_TIMEO)->EnableWindow(((CButton*)GetDlgItem(IDC_CHK_PS_EN))->GetCheck() == BST_CHECKED);
	GetDlgItem(IDC_COMBO_BS_FROM)->EnableWindow(((CButton*)GetDlgItem(IDC_CHK_BS_EN))->GetCheck() == BST_CHECKED);
	GetDlgItem(IDC_COMBO_BS_TO)->EnableWindow(((CButton*)GetDlgItem(IDC_CHK_BS_EN))->GetCheck() == BST_CHECKED);
}

void CPageParam::LoadRemoteConfig()
{

	m_bTM12 = !theApp.m_RemoteConfig.GetParam().time_12;
	m_bTmpCent = !theApp.m_RemoteConfig.GetParam().temp_cen;

	// load IDC_COMBO_TMR_SND
	CComboBox* pTmrSndBox = (CComboBox*)GetDlgItem(IDC_COMBO_TMR_SND);
	for (INT i = pTmrSndBox->GetCount() - 1; i >= 0; i--)
	{
		pTmrSndBox->DeleteString(i);
	}
	CString strLabel;
	for (INT i = 0; i < theApp.m_RemoteConfig.GetParam().tmr_snd_cnt; i++) {
		strLabel.Format(_T("sound %02d"), i);
		pTmrSndBox->AddString(strLabel);
	}

	m_nTmrSND = theApp.m_RemoteConfig.GetParam().tmr_snd;

	// load IDC_SLIDER_PLY_VOL
	CSliderCtrl* pSlide = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PLY_VOL);
	pSlide->SetRangeMin(theApp.m_RemoteConfig.GetParam().min_ply_vol, TRUE);
	pSlide->SetRangeMax(theApp.m_RemoteConfig.GetParam().max_ply_vol, TRUE);

	m_nPlyVol = theApp.m_RemoteConfig.GetParam().ply_vol;

	// load IDC_COMBO_PS_TIMEO
	CComboBox* pTmrPSTimeo = (CComboBox*)GetDlgItem(IDC_COMBO_PS_TIMEO);
	for (INT i = pTmrPSTimeo->GetCount() - 1; i >= 0; i--)
	{
		pTmrPSTimeo->DeleteString(i);
	}
	for (INT i = theApp.m_RemoteConfig.GetParam().min_power_timeo;
		i <= theApp.m_RemoteConfig.GetParam().max_power_timeo; i += theApp.m_RemoteConfig.GetParam().step_power_timeo) {
		strLabel.Format(_T("%02d"), i);
		pTmrPSTimeo->AddString(strLabel);
	}
	m_bPS = theApp.m_RemoteConfig.GetParam().power_timeo != 0;
	if (theApp.m_RemoteConfig.GetParam().step_power_timeo) {
		m_nPSTimeo = (theApp.m_RemoteConfig.GetParam().power_timeo
			/ theApp.m_RemoteConfig.GetParam().step_power_timeo) - 1;
	}

	m_bBS = theApp.m_RemoteConfig.GetParam().alm1_en;
	m_nBSFrom = theApp.m_RemoteConfig.GetParam().alm1_begin;
	m_nBSTo = theApp.m_RemoteConfig.GetParam().alm1_end;
	m_bBP = theApp.m_RemoteConfig.GetParam().bp_en;
	m_bMonLT = theApp.m_RemoteConfig.GetParam().mon_lt_en;
	m_bAcc = theApp.m_RemoteConfig.GetParam().acc_en;
}

void CPageParam::SaveRemoteConfig()
{
	theApp.m_RemoteConfig.GetParam().time_12 = !m_bTM12;
	theApp.m_RemoteConfig.GetParam().temp_cen = !m_bTmpCent;

	theApp.m_RemoteConfig.GetParam().tmr_snd = m_nTmrSND;
	theApp.m_RemoteConfig.GetParam().ply_vol = m_nPlyVol;

	if (m_bPS) {
		theApp.m_RemoteConfig.GetParam().power_timeo = (m_nPSTimeo + 1) * theApp.m_RemoteConfig.GetParam().step_power_timeo;
	}
	else {
		theApp.m_RemoteConfig.GetParam().power_timeo = 0;
	}
	theApp.m_RemoteConfig.GetParam().alm1_en = m_bBS;
	theApp.m_RemoteConfig.GetParam().alm1_begin = m_nBSFrom;
	theApp.m_RemoteConfig.GetParam().alm1_end = m_nBSTo;
	theApp.m_RemoteConfig.GetParam().bp_en = m_bBP;
	theApp.m_RemoteConfig.GetParam().mon_lt_en = m_bMonLT;
	theApp.m_RemoteConfig.GetParam().acc_en = m_bAcc;
}

BOOL CPageParam::OnInitDialog()
{	
	LoadRemoteConfig();

	if (!CDialog::OnInitDialog()) {
		return FALSE;
	}
	
	UpdateUI();

	return TRUE;

}

BEGIN_MESSAGE_MAP(CPageParam, CDialog)
	ON_BN_CLICKED(IDC_CHK_PS_EN, &CPageParam::OnBnClickedChkPsEn)
	ON_BN_CLICKED(IDC_CHK_BS_EN, &CPageParam::OnBnClickedChkBsEn)
END_MESSAGE_MAP()


// CPageParam 消息处理程序


void CPageParam::OnBnClickedChkPsEn()
{
	UpdateUI();
}


void CPageParam::OnBnClickedChkBsEn()
{
	UpdateUI();
}


void CPageParam::Save()
{
	CIVError Error;

	UpdateData(TRUE);

	SaveRemoteConfig();

	if (!theApp.m_RemoteConfig.SetRemoteConfigParam(Error)) {
		AfxMessageBox(Error.GetErrorStr());
	}
}