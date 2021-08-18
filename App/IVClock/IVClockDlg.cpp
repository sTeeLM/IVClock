
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

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CIVClockDlg 对话框



CIVClockDlg::CIVClockDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_IVCLOCK_DIALOG, pParent)
	, m_strRemoteTimeDate(_T(""))
	, m_strLocalTimeDate(_T(""))
	, m_bAutoSyncTimeDate(FALSE)
	, m_bTime12(FALSE)
	, m_bTempCen(FALSE)
	, m_bLightMon(FALSE)
	, m_bPowerSave(FALSE)
	, m_strPowerSave(_T(""))
	, m_strTimerSnd(_T(""))
	, m_strPlayVol(_T(""))
	, m_bBeeper(FALSE)
	, m_bBaoShi(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIVClockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_REMOTE_TIME_DATE, m_strRemoteTimeDate);
	DDX_Text(pDX, IDC_STATIC_LOCAL_TIME_DATE, m_strLocalTimeDate);
	DDX_Check(pDX, IDC_CHECK_AUTO_SYNC_TIME_DATE, m_bAutoSyncTimeDate);
	DDX_Radio(pDX, IDC_RADIO_TIME12, m_bTime12);
	DDX_Radio(pDX, IDC_RADIO_TEMP_CEN, m_bTempCen);
	DDX_Check(pDX, IDC_CHECK_LM, m_bLightMon);
	DDX_Check(pDX, IDC_CHECK_PS, m_bPowerSave);
	DDX_Text(pDX, IDC_EDIT_PS, m_strPowerSave);
	DDX_Control(pDX, IDC_SPIN_PS, m_ctlSpinPowerSave);
	DDX_Text(pDX, IDC_EDIT_TMR_SND, m_strTimerSnd);
	DDX_Control(pDX, IDC_SPIN_TMR_SND, m_ctlTimerSnd);
	DDX_Text(pDX, IDC_EDIT_PLY_VOL, m_strPlayVol);
	DDX_Control(pDX, IDC_SPIN_PLY_VOL, m_ctlPlayVol);
	DDX_Check(pDX, IDC_CHECK_BEEPER, m_bBeeper);
	DDX_Check(pDX, IDC_CHECK_BAOSHI, m_bBaoShi);
	DDX_Control(pDX, IDC_COMBO_ALM0, m_ctlComboAlm0);
	DDX_Control(pDX, IDC_COMBO_ALM1, m_ctlComboAlm1);
	DDX_Control(pDX, IDC_COMBO_ALM2, m_ctlComboAlm2);
	DDX_Control(pDX, IDC_COMBO_ALM3, m_ctlComboAlm3);
	DDX_Control(pDX, IDC_COMBO_ALM4, m_ctlComboAlm4);
	DDX_Control(pDX, IDC_COMBO_ALM5, m_ctlComboAlm5);
	DDX_Control(pDX, IDC_COMBO_ALM6, m_ctlComboAlm6);
	DDX_Control(pDX, IDC_COMBO_ALM7, m_ctlComboAlm7);
	DDX_Control(pDX, IDC_COMBO_ALM8, m_ctlComboAlm8);
	DDX_Control(pDX, IDC_COMBO_ALM9, m_ctlComboAlm9);
}

BEGIN_MESSAGE_MAP(CIVClockDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CIVClockDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CIVClockDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SET_ALARM, &CIVClockDlg::OnBnClickedButtonSetAlarm)
	ON_BN_CLICKED(IDC_BUTTON_SYNC_TIME_DATE, &CIVClockDlg::OnBnClickedButtonSyncTimeDate)
END_MESSAGE_MAP()


// CIVClockDlg 消息处理程序

BOOL CIVClockDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码
//  	m_ctlComboAlm0.SubclassDlgItem(IDC_COMBO_ALM0, this);
	 
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
		CDialog::OnSysCommand(nID, lParam);
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CIVClockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CIVClockDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnOK();
}


void CIVClockDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}


void CIVClockDlg::OnBnClickedButtonSetAlarm()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CIVClockDlg::OnBnClickedButtonSyncTimeDate()
{
	// TODO: 在此添加控件通知处理程序代码
}
