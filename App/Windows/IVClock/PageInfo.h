#pragma once


// CPageInfo 对话框

class CPageInfo : public CDialog
{
	DECLARE_DYNAMIC(CPageInfo)

public:
	CPageInfo(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPageInfo();
	virtual BOOL OnInitDialog();
	void UpdateUI();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_BATTEMP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL LoadRemoteConfig(CIVError& Error);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnInfoRefresh();
	afx_msg LRESULT cbGetInfo(WPARAM wParam, LPARAM lParam);

protected:
	BOOL m_bInProgress;
	CString m_strTempCen;
	CString m_strTempFah;
	CString m_strTempBatVoltage;
	CString m_strFirmwareVersion;
	CString m_strFirmwareBuildID;
	CProgressCtrl m_ctlTempBatQuantity;
};
