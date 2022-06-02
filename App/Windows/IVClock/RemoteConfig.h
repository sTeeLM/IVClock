#pragma once
#include <afx.h>
#include "ConfigManager.h"
#include "SerialPort.h"
#include "IVError.h"
#include "TaskQueue.h"
#include "remote_protocol.h"

class CRemoteConfig :public CObject
{
public:
	CRemoteConfig() :
		m_nPort(0),
		m_nBaudRate(0),
		m_nDataBits(0),
		m_nParity(0),
		m_nStopBits(0),
		m_bRTSCTS(FALSE),
		m_bDTRDSR(FALSE),
		m_bXONXOFF(FALSE),
		m_AlarmArray(NULL),
		m_nAlarmCnt(0),
		m_pRemoteConfigMon(NULL),
		m_bQuitRemoteConfigMon(FALSE),
		m_hQuitRemoteConfigMon(NULL),
		m_hSerialMutex(NULL),
		m_hDataMutex(NULL),
		m_hTaskEvent(NULL),
		m_bInTray(FALSE),
		m_hWndDateTime(NULL),
		m_oleLastSync(COleDateTime::GetCurrentTime())
	{
		ZeroMemory(&m_Param, sizeof(m_Param));
		ZeroMemory(&m_DateTime, sizeof(m_DateTime));
	}
	virtual ~CRemoteConfig() {
		if (m_AlarmArray)
			free(m_AlarmArray);
		m_AlarmArray = NULL;
	}
public:

	BOOL LoadSerialConfig(CIVError& Error);

	BOOL Ping(CIVError & Error, HANDLE hWaitEvent = NULL);

	BOOL StartRemoteConfigMon(CIVError& Error);

	void StopRemoteConfigMon();

	BOOL Initialize(CIVError& Error);
	void DeInitialize();

	BOOL LoadRemoteConfig(CIVError& Error, HANDLE hWaitEvent = NULL);

	BOOL GetParam(CIVError& Error, remote_control_body_param_t& param);

	BOOL GetDateTime(CIVError& Error, remote_control_body_time_t& datetime);

	BOOL SetParam(CIVError& Error, const remote_control_body_param_t& param);

	BOOL SetDateTime(CIVError& Error, const remote_control_body_time_t& datetime);

	BOOL SetDateTime(CIVError& Error, const COleDateTime & oleDateTime);

	BOOL AddTask(CIVError& Error, CTask::IV_TASK_TYPE_T eTaskType, HWND hCallbackHwnd, UINT nMessage, LPVOID pParam = NULL);

	void IntoTray() {
		m_bInTray = TRUE;
	}

	void LeaveTray() {
		m_bInTray = FALSE;
	}

	void SetDateTimeHwnd(HWND hWnd) { m_hWndDateTime = hWnd; }
protected:

	// 
	BOOL m_bInTray;
	HWND m_hWndDateTime;
	// serial config
	INT m_nPort;
	INT m_nBaudRate;
	INT m_nDataBits;
	INT m_nParity;
	INT m_nStopBits;
	INT m_bRTSCTS;
	INT m_bDTRDSR;
	INT m_bXONXOFF;

	// param
	HANDLE m_hDataMutex;
	remote_control_body_param_t m_Param;
	remote_control_body_time_t  m_DateTime;
	remote_control_body_alarm_t* m_AlarmArray;
	INT m_nAlarmCnt;

	// thread
	CWinThread* m_pRemoteConfigMon;
	BOOL        m_bQuitRemoteConfigMon;
	HANDLE      m_hQuitRemoteConfigMon;
	HANDLE      m_hTaskEvent;

	// task queue
	HANDLE      m_hSerialMutex;
	CTaskQueue  m_TaskQueue;
	COleDateTime m_oleLastSync;
protected:
	static UINT fnRemoteConfigMon(LPVOID pParam);
	BOOL RemoteConfigMon(CIVError& Error);
	BOOL ProcessSerialMsg(CSerialPortConnection* pConn, remote_control_msg_t& msg, CIVError& Error);
	BOOL LoadSetConfig(CIVError& Error, HANDLE hWaitEvent, remote_control_cmd_type_t eCmd, INT nIndex = 0);

	BOOL SetRemoteConfigParam(CIVError& Error, HANDLE hWaitEvent = NULL);
	BOOL SetRemoteConfigAlarm(INT nAlarmIndex, CIVError& Error, HANDLE hWaitEvent = NULL);
	BOOL SetRemoteConfigDateTime(CIVError& Error, HANDLE hWaitEvent = NULL);

	BOOL LoadRemoteConfigParam(CIVError& Error, HANDLE hWaitEvent = NULL);
	BOOL LoadRemoteConfigAlarm(CIVError& Error, HANDLE hWaitEvent = NULL);
	BOOL LoadRemoteConfigDateTime(CIVError& Error, HANDLE hWaitEvent = NULL);
};

