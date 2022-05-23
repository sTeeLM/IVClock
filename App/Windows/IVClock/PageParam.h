#pragma once


// CPageParam 对话框

class CPageParam : public CDialog
{
	DECLARE_DYNAMIC(CPageParam)

public:
	CPageParam(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPageParam();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_PARAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
