#pragma once


// CPageSerial 对话框

class CPageSerial : public CDialog
{
	DECLARE_DYNAMIC(CPageSerial)

public:
	CPageSerial(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPageSerial();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_SERIAL};
#endif

protected:
	BOOL m_bInProgress;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	void UpdateUI();
	DECLARE_MESSAGE_MAP()
public:
	INT m_nPort;
	INT m_nBaudRate;
	INT m_nDataBits;
	INT m_nParity;
	INT m_nStopBits;
	BOOL m_bDTRDSR;
	BOOL m_bRTSCTS;
	BOOL m_bXONXOFF;

public:
	afx_msg void OnBnClickedBtnTestConn();
	afx_msg LRESULT cbPing(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnSave();
};
