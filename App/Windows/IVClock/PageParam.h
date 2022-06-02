#pragma once


// CPageParam 对话框

class CPageParam : public CDialog
{
	DECLARE_DYNAMIC(CPageParam)

public:
	CPageParam(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPageParam();
	virtual BOOL OnInitDialog();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_PARAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void UpdateUI();
	BOOL LoadRemoteConfig(CIVError& Error);
	BOOL SaveRemoteConfig(CIVError& Error);
	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_bInProgress;
	BOOL m_bTM12;
	BOOL m_bTmpCent;
	INT m_nTmrSND;
	INT m_nPlyVol;
	INT m_nPSTimeo;
	BOOL m_bPS;
	BOOL m_bBS;
	INT m_nBSFrom;
	INT m_nBSTo;
	BOOL m_bBP;
	BOOL m_bMonLT;
	BOOL m_bAcc;
	INT m_nStepPowerTimeo;

public:
	afx_msg void OnBnClickedChkPsEn();
	afx_msg void OnBnClickedChkBsEn();
	afx_msg LRESULT cbSetParam(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnSet();
};
