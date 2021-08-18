
// IVClockDlg.h: 头文件
//

#pragma once

#include "CheckComboBox.h"

// CIVClockDlg 对话框
class CIVClockDlg : public CDialog
{
// 构造
public:
	CIVClockDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IVCLOCK_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSetAlarm();
	afx_msg void OnBnClickedButtonSyncTimeDate();

	CString m_strRemoteTimeDate;
	CString m_strLocalTimeDate;
	BOOL m_bAutoSyncTimeDate;
	BOOL m_bTime12;
	BOOL m_bTempCen;
	BOOL m_bLightMon;
	BOOL m_bPowerSave;
	CString m_strPowerSave;
	CSpinButtonCtrl m_ctlSpinPowerSave;
	CString m_strTimerSnd;
	CSpinButtonCtrl m_ctlTimerSnd;
	CString m_strPlayVol;
	CSpinButtonCtrl m_ctlPlayVol;
	BOOL m_bBeeper;
	BOOL m_bBaoShi;
	CCheckComboBox m_ctlComboAlm0;
	CCheckComboBox m_ctlComboAlm1;
	CCheckComboBox m_ctlComboAlm2;
	CCheckComboBox m_ctlComboAlm3;
	CCheckComboBox m_ctlComboAlm4;
	CCheckComboBox m_ctlComboAlm5;
	CCheckComboBox m_ctlComboAlm6;
	CCheckComboBox m_ctlComboAlm7;
	CCheckComboBox m_ctlComboAlm8;
	CCheckComboBox m_ctlComboAlm9;

};
