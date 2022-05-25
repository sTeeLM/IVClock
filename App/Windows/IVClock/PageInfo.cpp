// PageInfo.cpp: 实现文件
//

#include "pch.h"
#include "IVClock.h"
#include "PageInfo.h"
#include "afxdialogex.h"
#include "SerialPort.h"

// CPageInfo 对话框

IMPLEMENT_DYNAMIC(CPageInfo, CDialog)

CPageInfo::CPageInfo(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PROPPAGE_INFO, pParent)
	, m_nPort(0)
	, m_nBaudRate(0)
	, m_nDataBits(0)
	, m_nParity(0)
	, m_nStopBits(0)
	, m_bDTRDSR(FALSE)
	, m_bRTSCTS(FALSE)
	, m_bXONXOFF(FALSE)
{

}

CPageInfo::~CPageInfo()
{
}

void CPageInfo::DoDataExchange(CDataExchange* pDX)
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

BOOL CPageInfo::OnInitDialog()
{
	// load port combo list
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO_PORT);
	CConfigManager::CONFIG_VALUE_T val;
	CString strLabel = _T("");

	for (INT i = 0; i < theApp.m_Serial.GetPortCount(); i++) {
		pBox->AddString(theApp.m_Serial.GetPortFriendlyName(i));
	}
	theApp.m_Config.GetConfig(_T("connect"), _T("port"), val);
	m_nPort = val.n8 < theApp.m_Serial.GetPortCount() ? val.u8 : 0;

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_BAUDRATE);
	for (INT i = 0; i < theApp.m_Serial.GetBaudRateCount(); i++) {
		strLabel.Format(_T("%d"), theApp.m_Serial.GetBaudRate(i));
		pBox->AddString(strLabel);
	}
	theApp.m_Config.GetConfig(_T("connect"), _T("baudrate"), val);
	m_nBaudRate = val.n8 < theApp.m_Serial.GetBaudRateCount() ? val.u8 : 0;

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_DATA_BITS);
	for (INT i = 0; i < theApp.m_Serial.GetDataBitsCount(); i++) {
		strLabel.Format(_T("%d"), theApp.m_Serial.GetDataBits(i));
		pBox->AddString(strLabel);
	}
	theApp.m_Config.GetConfig(_T("connect"), _T("databits"), val);
	m_nDataBits = val.n8 < theApp.m_Serial.GetDataBitsCount() ? val.u8 : 0;

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_PARITY);
	for (INT i = 0; i < theApp.m_Serial.GetParityCount(); i++) {
		pBox->AddString(theApp.m_Serial.GetParity(i));
	}
	theApp.m_Config.GetConfig(_T("connect"), _T("parity"), val);
	m_nParity = val.n8 < theApp.m_Serial.GetParityCount() ? val.u8 : 0;

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_STOP_BITS);
	for (INT i = 0; i < theApp.m_Serial.GetStopBitsCount(); i++) {
		pBox->AddString(theApp.m_Serial.GetStopBits(i));
	}
	theApp.m_Config.GetConfig(_T("connect"), _T("stopbits"), val);
	m_nStopBits = val.n8 < theApp.m_Serial.GetStopBitsCount() ? val.u8 : 0;

	theApp.m_Config.GetConfig(_T("connect"), _T("dtr_dsr"), val);
	m_bRTSCTS = val.u8;

	theApp.m_Config.GetConfig(_T("connect"), _T("dts_cts"), val);
	m_bDTRDSR = val.u8;

	theApp.m_Config.GetConfig(_T("connect"), _T("xon_xoff"), val);
	m_bXONXOFF = val.u8;

	return CDialog::OnInitDialog();
}

BEGIN_MESSAGE_MAP(CPageInfo, CDialog)
	ON_BN_CLICKED(IDC_BTN_TEST_CONN, &CPageInfo::OnBnClickedBtnTestConn)
END_MESSAGE_MAP()


// CPageInfo 消息处理程序


void CPageInfo::OnBnClickedBtnTestConn()
{
	UpdateData(TRUE);
	CSerialPortConnection * pConn = theApp.m_Serial.OpenSerial(
		m_nPort,
		m_nBaudRate,
		m_nDataBits,
		m_nParity,
		m_nStopBits,
		m_bRTSCTS,
		m_bDTRDSR,
		m_bXONXOFF
	);

}
