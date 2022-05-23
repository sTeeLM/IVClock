
// IVClockDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "IVClock.h"
#include "IVClockDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIVClockDlg 对话框



CIVClockDlg::CIVClockDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IVCLOCK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIVClockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_ctlTab);
}

BEGIN_MESSAGE_MAP(CIVClockDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CIVClockDlg::OnTcnSelchangeTabMain)
	ON_BN_CLICKED(IDOK, &CIVClockDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CIVClockDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CIVClockDlg 消息处理程序

BOOL CIVClockDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_ctlTab.InsertItem(0, _T("INFO"));
	m_ctlTab.InsertItem(1, _T("PARAM"));
	m_ctlTab.InsertItem(2, _T("DATETIME"));
	m_ctlTab.InsertItem(3, _T("ALARM"));

	m_pageInfo.Create(IDD_PROPPAGE_INFO, &m_ctlTab);
	m_pageParam.Create(IDD_PROPPAGE_PARAM, &m_ctlTab);
	m_pageDateTime.Create(IDD_PROPPAGE_DATETIME, &m_ctlTab);
	m_pageAlarm.Create(IDD_PROPPAGE_ALARM, &m_ctlTab);

	CRect rect;
	m_ctlTab.GetClientRect(&rect);
	rect.top += 20;
	rect.bottom -= 4;
	rect.left += 4;
	rect.right -= 4;
	m_pageInfo.MoveWindow(&rect);
	m_pageParam.MoveWindow(&rect);
	m_pageDateTime.MoveWindow(&rect);
	m_pageAlarm.MoveWindow(&rect);

	m_pageInfo.ShowWindow(SW_SHOW);
	m_ctlTab.SetCurSel(0);
	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CIVClockDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CIVClockDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CIVClockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CIVClockDlg::OnTcnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult)
{
	INT nCurSel = m_ctlTab.GetCurSel();
	switch (nCurSel) {
	case 0:
		m_pageInfo.ShowWindow(SW_SHOW);
		m_pageParam.ShowWindow(SW_HIDE);
		m_pageDateTime.ShowWindow(SW_HIDE);
		m_pageAlarm.ShowWindow(SW_HIDE);
		break;
	case 1:
		m_pageInfo.ShowWindow(SW_HIDE);
		m_pageParam.ShowWindow(SW_SHOW);
		m_pageDateTime.ShowWindow(SW_HIDE);
		m_pageAlarm.ShowWindow(SW_HIDE);
		break;
	case 2:
		m_pageInfo.ShowWindow(SW_HIDE);
		m_pageParam.ShowWindow(SW_HIDE);
		m_pageDateTime.ShowWindow(SW_SHOW);
		m_pageAlarm.ShowWindow(SW_HIDE);
		break;
	case 3:
		m_pageInfo.ShowWindow(SW_HIDE);
		m_pageParam.ShowWindow(SW_HIDE);
		m_pageDateTime.ShowWindow(SW_HIDE);
		m_pageAlarm.ShowWindow(SW_SHOW);
		break;
	}
	*pResult = 0;
}


void CIVClockDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	// CDialogEx::OnOK();
}


void CIVClockDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	// CDialogEx::OnCancel();
}
