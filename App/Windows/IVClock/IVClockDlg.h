
// IVClockDlg.h: 头文件
//

#pragma once

#include "PageParam.h"
#include "PageDateTime.h"
#include "PageInfo.h"
#include "PageAlarm.h"
// CIVClockDlg 对话框
class CIVClockDlg : public CDialogEx
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
	CTabCtrl m_ctlTab;
	CPageParam m_pageParam;
	CPageSerial  m_pageSerial;
	CPageAlarm m_pageAlarm;
	CPageDateTime m_pageDateTime;
	afx_msg void OnTcnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
