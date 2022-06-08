// CPageInfo.cpp: 实现文件
//

#include "pch.h"
#include "IVClock.h"
#include "PageInfo.h"
#include "afxdialogex.h"


// CPageInfo 对话框

IMPLEMENT_DYNAMIC(CPageInfo, CDialog)

CPageInfo::CPageInfo(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PROPPAGE_BATTEMP, pParent)
	, m_bInProgress(FALSE)
	, m_strTempCen(_T(""))
	, m_strTempFah(_T(""))
	, m_strTempBatVoltage(_T(""))
	, m_strFirmwareVersion(_T(""))
	, m_strFirmwareBuildID(_T(""))
{

}

CPageInfo::~CPageInfo()
{
}

BOOL CPageInfo::LoadRemoteConfig(CIVError& Error)
{
	remote_control_body_info_t info;

	if (!theApp.m_RemoteConfig.IsInfoValid())
		return TRUE;

	if(!theApp.m_RemoteConfig.GetInfo(Error, info))
		return FALSE;

	m_strTempCen.Format(_T("%0.2f ℃"), info.temp_cen);
	m_strTempFah.Format(_T("%0.2f ℉"), info.temp_fah);
	m_strTempBatVoltage.Format(_T("%0.2f V"), info.bat_voltage);
	m_strFirmwareVersion.Format(_T("%02d.%02d"), info.firmware_version_major, info.firmware_version_minor);

	CString strTemp;
	m_strFirmwareBuildID.Empty();
	for (INT i = 0; i < sizeof(info.build_id); i++) {
		strTemp.Format(_T("%02x"), info.build_id[i]);
		m_strFirmwareBuildID += strTemp;
	}

	if(m_ctlTempBatQuantity.GetSafeHwnd())
		m_ctlTempBatQuantity.SetPos(info.bat_quantity);
	
	return TRUE;
}

BOOL CPageInfo::OnInitDialog()
{
	CIVError Error;
	remote_control_body_info_t battemp;

	if (!LoadRemoteConfig(Error)) {
		return FALSE;
	}

	if (!CDialog::OnInitDialog()) {
		return FALSE;
	}

	if (!theApp.m_RemoteConfig.GetInfo(Error, battemp))
		return FALSE;

	if (m_ctlTempBatQuantity.GetSafeHwnd()) {
		m_ctlTempBatQuantity.SetRange(0, 100);
		m_ctlTempBatQuantity.SetPos(battemp.bat_quantity);
	}

	UpdateUI();

	return TRUE;
}

void CPageInfo::UpdateUI()
{
	GetDlgItem(IDC_BTN_BATTEMP_REFRESH)->EnableWindow(!m_bInProgress && theApp.m_RemoteConfig.IsInfoValid());
}

void CPageInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEMP_CEN, m_strTempCen);
	DDX_Text(pDX, IDC_EDIT_TEMP_FAH, m_strTempFah);
	DDX_Text(pDX, IDC_EDIT_BAT_VOTAGE, m_strTempBatVoltage);
	DDX_Text(pDX, IDC_EDIT_FIRM_VERSION, m_strFirmwareVersion);
	DDX_Text(pDX, IDC_EDIT_FIRM_BUILD_ID, m_strFirmwareBuildID);
	DDX_Control(pDX, IDC_PROGRESS_BAT_QUANTITY, m_ctlTempBatQuantity);
}


BEGIN_MESSAGE_MAP(CPageInfo, CDialog)
	ON_MESSAGE(WM_CB_GET_INFO, cbGetInfo)
	ON_BN_CLICKED(IDC_BTN_BATTEMP_REFRESH, &CPageInfo::OnBnClickedBtnInfoRefresh)
END_MESSAGE_MAP()


// CPageInfo 消息处理程序

LRESULT CPageInfo::cbGetInfo(WPARAM wParam, LPARAM lParam)
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
				TRACE(_T("cbGetInfo Error:%s\n"), pTask->m_Error.GetErrorStr());
			}
		}
	}
	else {
		if (pTask->m_pParam)
			AfxMessageBox(pTask->m_Error.GetErrorStr());
		else {
			TRACE(_T("cbGetInfo Error:%s\n"), pTask->m_Error.GetErrorStr());
		}
	}

	m_bInProgress = FALSE;
	UpdateUI();

	return 0;
}

void CPageInfo::OnBnClickedBtnInfoRefresh()
{
	CIVError Error;
	if (!theApp.m_RemoteConfig.AddTask(Error, CTask::IV_TASK_GET_INFO, GetSafeHwnd(), WM_CB_GET_INFO, (LPVOID)1)) {
		AfxMessageBox(Error.GetErrorStr());
	}

	m_bInProgress = TRUE;
	UpdateUI();
}
