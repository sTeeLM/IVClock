#pragma once


// CPageDateTime 对话框

class CPageDateTime : public CDialog
{
	DECLARE_DYNAMIC(CPageDateTime)

public:
	CPageDateTime(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPageDateTime();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_DATETIME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bTMAutoSync;
	INT m_nTMAutoSyncInterval;
	COleDateTime m_oleDate;
	COleDateTime m_oleTime;
	afx_msg void OnBnClickedBtnDatetimeSyncNow();
};
