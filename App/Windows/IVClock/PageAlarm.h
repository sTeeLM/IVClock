#pragma once


// CPageAlarm 对话框

class CPageAlarm : public CDialog
{
	DECLARE_DYNAMIC(CPageAlarm)

public:
	CPageAlarm(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPageAlarm();
	virtual BOOL OnInitDialog();
	void UpdateUI();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_ALARM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL SaveRemoteConfig(CIVError& Error);
	BOOL LoadRemoteConfig(CIVError& Error);
	void SetAlarmDisplay(const remote_control_body_alarm_t& alarm);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnAlarmSelAll();
	afx_msg void OnBnClickedBtnAlarmSelNone();
	afx_msg void OnBnClickedBtnSet();
	afx_msg void OnCbnSelchangeComboAlarmIndex();
	afx_msg LRESULT cbSetAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT cbGetAlarm(WPARAM wParam, LPARAM lParam);
protected:
	BOOL m_bInProgress;

public:
	INT m_nAlarmIndex;
	INT m_nAlarmSnd;
	INT m_nAlarmHour;
	INT m_nAlarmMin;

};
