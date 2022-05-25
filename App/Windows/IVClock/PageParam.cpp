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
}

BEGIN_MESSAGE_MAP(CPageParam, CDialog)
	ON_BN_CLICKED(IDC_CHK_PS_EN, &CPageParam::OnBnClickedChkPsEn)
END_MESSAGE_MAP()


// CPageParam 消息处理程序


void CPageParam::OnBnClickedChkPsEn()
{
	TRACE(_T("OnBnClickedChkPsEn\n"));
	UpdateUI();
}
