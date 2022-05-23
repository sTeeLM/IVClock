// PageParam.cpp: 实现文件
//

#include "pch.h"
#include "IVClock.h"
#include "PageParam.h"
#include "afxdialogex.h"


// CPageParam 对话框

IMPLEMENT_DYNAMIC(CPageParam, CDialog)

CPageParam::CPageParam(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PROPPAGE_PARAM, pParent)
{

}

CPageParam::~CPageParam()
{
}

void CPageParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageParam, CDialog)
END_MESSAGE_MAP()


// CPageParam 消息处理程序
