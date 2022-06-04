// PageInfo.cpp: 实现文件
//

#include "pch.h"
#include "IVClock.h"
#include "PageSerial.h"
#include "afxdialogex.h"
#include "SerialPort.h"

// CPageSerial 对话框

IMPLEMENT_DYNAMIC(CPageSerial, CDialog)

CPageSerial::CPageSerial(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PROPPAGE_INFO, pParent)
	, m_nPort(0)
	, m_nBaudRate(0)
	, m_nDataBits(0)
	, m_nParity(0)
	, m_nStopBits(0)
	, m_bDTRDSR(FALSE)
	, m_bRTSCTS(FALSE)
	, m_bXONXOFF(FALSE)
	, m_bInProgress(FALSE)
{

}

CPageSerial::~CPageSerial()
{
}

void CPageSerial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_PORT, m_nPort);
	DDX_CBIndex(pDX, IDC_COMBO_BAUDRATE, m_nBaudRate);
	DDX_CBIndex(pDX, IDC_COMBO_DATA_BITS, m_nDataBits);
	DDX_CBIndex(pDX, IDC_COMBO_PARITY, m_nParity);
	DDX_CBIndex(pDX, IDC_COMBO_STOP_BITS, m_nStopBits);
	DDX_Check(pDX, IDC_CHK_DTR_DSR, m_bDTRDSR);
	DDX_Check(pDX, IDC_CHK_RTS_CTS, m_bRTSCTS);
	DDX_Check(pDX, IDC_CHK_XON_XOFF, m_bXONXOFF);
}

BOOL CPageSerial::OnInitDialog()
{
	// load port combo list
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO_PORT);
	CConfigManager::CONFIG_VALUE_T val;

	for (INT i = 0; i < CSerialPort::GetPortCount(); i++) {
		pBox->AddString(CSerialPort::GetPortFriendlyName(i));
	}
	theApp.m_Config.GetConfig(_T("connect"), _T("port"), val);
	m_nPort = CSerialPort::PortNumberToIndex(val.u8);

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_BAUDRATE);
	for (INT i = 0; i < CSerialPort::GetBaudRateCount(); i++) {
		pBox->AddString(CSerialPort::GetBaudRateDesc(i));
	}
	theApp.m_Config.GetConfig(_T("connect"), _T("baudrate"), val);
	m_nBaudRate = val.n8 < CSerialPort::GetBaudRateCount() ? val.u8 : 0;

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_DATA_BITS);
	for (INT i = 0; i < CSerialPort::GetDataBitsCount(); i++) {
		pBox->AddString(CSerialPort::GetDataBitsDesc(i));
	}
	theApp.m_Config.GetConfig(_T("connect"), _T("databits"), val);
	m_nDataBits = val.n8 < CSerialPort::GetDataBitsCount() ? val.u8 : 0;

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_PARITY);
	for (INT i = 0; i < CSerialPort::GetParityCount(); i++) {
		pBox->AddString(CSerialPort::GetParityDesc(i));
	}
	theApp.m_Config.GetConfig(_T("connect"), _T("parity"), val);
	m_nParity = val.n8 < CSerialPort::GetParityCount() ? val.u8 : 0;

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_STOP_BITS);
	for (INT i = 0; i < CSerialPort::GetStopBitsCount(); i++) {
		pBox->AddString(CSerialPort::GetStopBitsDesc(i));
	}
	theApp.m_Config.GetConfig(_T("connect"), _T("stopbits"), val);
	m_nStopBits = val.n8 < CSerialPort::GetStopBitsCount() ? val.u8 : 0;

	theApp.m_Config.GetConfig(_T("connect"), _T("dtr_dsr"), val);
	m_bRTSCTS = val.u8;

	theApp.m_Config.GetConfig(_T("connect"), _T("dts_cts"), val);
	m_bDTRDSR = val.u8;

	theApp.m_Config.GetConfig(_T("connect"), _T("xon_xoff"), val);
	m_bXONXOFF = val.u8;

	return CDialog::OnInitDialog();
}

BEGIN_MESSAGE_MAP(CPageSerial, CDialog)
	ON_MESSAGE(WM_CB_PING, cbPing)
	ON_BN_CLICKED(IDC_BTN_TEST_CONN, &CPageSerial::OnBnClickedBtnTestConn)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CPageSerial::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CPageSerial 消息处理程序

LRESULT CPageSerial::cbPing(WPARAM wParam, LPARAM lParam)
{
	CTask* pTask = (CTask*)wParam;
	CString strLabel;
	if (pTask->m_bRes) {
		strLabel.LoadString(IDS_TEST_PASSED);
		GetDlgItem(IDC_EDIT_TEST_RES)->SetWindowText(strLabel);
	}
	else {
		strLabel.LoadString(IDS_TEST_FAILED);
		GetDlgItem(IDC_EDIT_TEST_RES)->SetWindowText(strLabel);
	}
	m_bInProgress = FALSE;
	UpdateUI();
	return 0;
}
/*
*	DDX_CBIndex(pDX, IDC_COMBO_PORT, m_nPort);
	DDX_CBIndex(pDX, IDC_COMBO_BAUDRATE, m_nBaudRate);
	DDX_CBIndex(pDX, IDC_COMBO_DATA_BITS, m_nDataBits);
	DDX_CBIndex(pDX, IDC_COMBO_PARITY, m_nParity);
	DDX_CBIndex(pDX, IDC_COMBO_STOP_BITS, m_nStopBits);
	DDX_Check(pDX, IDC_CHK_DTR_DSR, m_bDTRDSR);
	DDX_Check(pDX, IDC_CHK_RTS_CTS, m_bRTSCTS);
	DDX_Check(pDX, IDC_CHK_XON_XOFF, m_bXONXOFF); 
*/
void CPageSerial::UpdateUI()
{
	GetDlgItem(IDC_COMBO_PORT)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_COMBO_BAUDRATE)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_COMBO_DATA_BITS)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_COMBO_PARITY)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_COMBO_STOP_BITS)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_CHK_DTR_DSR)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_CHK_RTS_CTS)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_CHK_XON_XOFF)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_BTN_TEST_CONN)->EnableWindow(!m_bInProgress);
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(!m_bInProgress);
}

void CPageSerial::OnBnClickedBtnTestConn()
{
	CIVError Error;

	CString strLabel;

	UpdateData(TRUE);

	strLabel.LoadString(IDS_TEST_TESTING);
	GetDlgItem(IDC_EDIT_TEST_RES)->SetWindowText(strLabel);
	
	m_bInProgress = TRUE;
	UpdateUI();

	if (!theApp.m_RemoteConfig.AddTask(Error, CTask::IV_TASK_PING, this->GetSafeHwnd(), WM_CB_PING)) {
		AfxMessageBox(Error.GetErrorStr());
		m_bInProgress = FALSE;
		UpdateUI();
	}
	
}


void CPageSerial::OnBnClickedBtnSave()
{
	CConfigManager::CONFIG_VALUE_T val;
	UpdateData(TRUE);

	val.u8 = CSerialPort::GetPortNbr(m_nPort);
	theApp.m_Config.SetConfig(_T("connect"), _T("port"), val);

	val.u8 = m_nBaudRate;
	theApp.m_Config.SetConfig(_T("connect"), _T("baudrate"), val);

	val.u8 = m_nDataBits;
	theApp.m_Config.SetConfig(_T("connect"), _T("databits"), val);

	val.u8 = m_nParity;
	theApp.m_Config.SetConfig(_T("connect"), _T("parity"), val);

	val.u8 = m_nStopBits;
	theApp.m_Config.SetConfig(_T("connect"), _T("stopbits"), val);

	val.u8 = m_bRTSCTS;
	theApp.m_Config.SetConfig(_T("connect"), _T("dtr_dsr"), val);

	val.u8 = m_bDTRDSR;
	theApp.m_Config.SetConfig(_T("connect"), _T("dts_cts"), val);

	val.u8 = m_bXONXOFF;
	theApp.m_Config.SetConfig(_T("connect"), _T("xon_xoff"), val);

	AfxMessageBox(_T("config saved!"));
}
