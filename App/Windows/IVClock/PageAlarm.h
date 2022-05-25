#pragma once


// CPageAlarm 对话框

class CPageAlarm : public CDialog
{
	DECLARE_DYNAMIC(CPageAlarm)

public:
	CPageAlarm(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPageAlarm();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_ALARM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	INT m_nAlarmIndex;
	afx_msg void OnBnClickedBtnAlarmSelAll();
	afx_msg void OnBnClickedBtnAlarmSelNone();
	COleDateTime m_oleAlarmTime;
};
