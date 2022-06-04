// CPageBatTemp.cpp: 实现文件
//

#include "pch.h"
#include "IVClock.h"
#include "PageBatTemp.h"
#include "afxdialogex.h"


// CPageBatTemp 对话框

IMPLEMENT_DYNAMIC(CPageBatTemp, CDialog)

CPageBatTemp::CPageBatTemp(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PROPPAGE_BATTEMP, pParent)
	, m_bInProgress(FALSE)
	, m_strTempCen(_T(""))
	, m_strTempFah(_T(""))
	, m_strTempBatVoltage(_T(""))
{

}

CPageBatTemp::~CPageBatTemp()
{
}

BOOL CPageBatTemp::LoadRemoteConfig(CIVError& Error)
{
	remote_control_body_bat_temp_t battemp;

	if (!theApp.m_RemoteConfig.IsBatTempValid())
		return TRUE;

	if(!theApp.m_RemoteConfig.GetBatTemp(Error, battemp))
		return FALSE;

	m_strTempCen.Format(_T("%0.2f ℃"), battemp.temp_cen);
	m_strTempFah.Format(_T("%0.2f ℉"), battemp.temp_fah);
	m_strTempBatVoltage.Format(_T("%0.2f V"), battemp.bat_voltage);

	if(m_ctlTempBatQuantity.GetSafeHwnd())
		m_ctlTempBatQuantity.SetPos(battemp.bat_quantity);
	
	return TRUE;
}

BOOL CPageBatTemp::OnInitDialog()
{
	CIVError Error;
	remote_control_body_bat_temp_t battemp;

	if (!LoadRemoteConfig(Error)) {
		return FALSE;
	}

	if (!CDialog::OnInitDialog()) {
		return FALSE;
	}

	if (!theApp.m_RemoteConfig.GetBatTemp(Error, battemp))
		return FALSE;

	if (m_ctlTempBatQuantity.GetSafeHwnd()) {
		m_ctlTempBatQuantity.SetRange(0, 100);
		m_ctlTempBatQuantity.SetPos(battemp.bat_quantity);
	}

	UpdateUI();

	return TRUE;
}

void CPageBatTemp::UpdateUI()
{
	GetDlgItem(IDC_BTN_BATTEMP_REFRESH)->EnableWindow(!m_bInProgress && theApp.m_RemoteConfig.IsBatTempValid());
}

void CPageBatTemp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEMP_CEN, m_strTempCen);
	DDX_Text(pDX, IDC_EDIT_TEMP_FAH, m_strTempFah);
	DDX_Text(pDX, IDC_EDIT_BAT_VOTAGE, m_strTempBatVoltage);
	DDX_Control(pDX, IDC_PROGRESS_BAT_QUANTITY, m_ctlTempBatQuantity);
}


BEGIN_MESSAGE_MAP(CPageBatTemp, CDialog)
	ON_MESSAGE(WM_CB_GET_BATTEMP, cbGetBatTemp)
	ON_BN_CLICKED(IDC_BTN_BATTEMP_REFRESH, &CPageBatTemp::OnBnClickedBtnBattempRefresh)
END_MESSAGE_MAP()


// CPageBatTemp 消息处理程序

LRESULT CPageBatTemp::cbGetBatTemp(WPARAM wParam, LPARAM lParam)
{
	CTask* pTask = (CTask*)wParam;
	CIVError Error;

	if (pTask->m_bRes) {
		if (LoadRemoteConfig(Error)) {
			UpdateData(FALSE);
		} else{
			if (pTask->m_pParam)
				AfxMessageBox(pTask->m_Error.GetErrorStr());
			else {
				TRACE(_T("cbGetBatTemp Error:%s\n"), pTask->m_Error.GetErrorStr());
			}
		}
	}
	else {
		if (pTask->m_pParam)
			AfxMessageBox(pTask->m_Error.GetErrorStr());
		else {
			TRACE(_T("cbGetBatTemp Error:%s\n"), pTask->m_Error.GetErrorStr());
		}
	}

	m_bInProgress = FALSE;
	UpdateUI();

	return 0;
}

void CPageBatTemp::OnBnClickedBtnBattempRefresh()
{
	CIVError Error;
	if (!theApp.m_RemoteConfig.AddTask(Error, CTask::IV_TASK_GET_BATTEMP, GetSafeHwnd(), WM_CB_GET_BATTEMP, (LPVOID)1)) {
		AfxMessageBox(Error.GetErrorStr());
	}

	m_bInProgress = TRUE;
	UpdateUI();
}
