#pragma once


// CPageBatTemp 对话框

class CPageBatTemp : public CDialog
{
	DECLARE_DYNAMIC(CPageBatTemp)

public:
	CPageBatTemp(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPageBatTemp();
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
	afx_msg void OnBnClickedBtnBattempRefresh();
	afx_msg LRESULT cbGetBatTemp(WPARAM wParam, LPARAM lParam);

protected:
	BOOL m_bInProgress;
	CString m_strTempCen;
	CString m_strTempFah;
	CString m_strTempBatVoltage;
	CProgressCtrl m_ctlTempBatQuantity;
};
