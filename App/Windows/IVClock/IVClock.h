
// IVClock.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号

#include "ConfigManager.h"
#include "RemoteConfig.h"

// CIVClockApp:
// 有关此类的实现，请参阅 IVClock.cpp
//

class CIVClockApp : public CWinApp
{
public:
	CIVClockApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
public:
	CRemoteConfig m_RemoteConfig;
	CConfigManager m_Config;
protected:
	CString GetWorkingPath();
};

extern CIVClockApp theApp;

#define WM_SHOWTASK (WM_USER + 1)
#define WM_CB_PING  (WM_USER + 2)
#define WM_CB_GET_PARAM (WM_USER + 3)
#define WM_CB_SET_PARAM (WM_USER + 4)
#define WM_CB_GET_TIME (WM_USER + 3)
#define WM_CB_SET_TIME (WM_USER + 4)