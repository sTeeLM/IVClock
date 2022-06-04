#pragma once


// CPageDateTime 对话框

class CPageDateTime : public CDialog
{
	DECLARE_DYNAMIC(CPageDateTime)

public:
	CPageDateTime(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPageDateTime();
	virtual BOOL OnInitDialog();
	void Save();
	void UpdateUI();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_DATETIME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	INT SyncIntervalToIndex(UINT uSyncIntervalSec);
	UINT IndexToSyncInterval(INT nIndex);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnDatetimeSyncNow();
	afx_msg void OnDestroy();
	afx_msg LRESULT cbGetTimeDate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT cbSetTimeDate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedChkDatetimeAutoSync();
	afx_msg void OnBnClickedBtnDatetimeRefresh();
	afx_msg void OnCbnSelchangeComboDatetimeSyncInterval();
	static UINT fnLocalDateTimeThread(LPVOID pParam);
	BOOL StartLocalDateTimeThread(CIVError& Error);
	void StopLocalDateTimeThread();
	HANDLE m_hStopDateTimeThread;
	BOOL m_bStopDateTimeThread;
	CWinThread* m_pLocalDateTimeThread;
	BOOL m_bInProgress;
public:
	BOOL m_bTMAutoSync;
	INT m_nTMAutoSyncInterval;
	COleDateTime m_oleDate;
	COleDateTime m_oleTime;
	COleDateTime m_oleLastSync;
	COleDateTime m_oleLastRefresh;
};
