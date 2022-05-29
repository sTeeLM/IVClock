﻿#pragma once


// CPageInfo 对话框

class CPageInfo : public CDialog
{
	DECLARE_DYNAMIC(CPageInfo)

public:
	CPageInfo(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPageInfo();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_INFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
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
	void Save();
};
