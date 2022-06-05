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
		m_RemoteAlarmArray(NULL),
		m_nRemoteAlarmCnt(0),
		m_nRemoteAlarmSndCnt(0),
		m_pRemoteConfigMon(NULL),
		m_bQuitRemoteConfigMon(FALSE),
		m_hQuitRemoteConfigMon(NULL),
		m_hSerialMutex(NULL),
		m_hDataMutex(NULL),
		m_hTaskEvent(NULL),
		m_bInTray(FALSE),
		m_hWndParam(NULL),
		m_hWndDateTime(NULL),
		m_hWndAlarm(NULL),
		m_hWndInfo(NULL),
		m_oleLastSync(COleDateTime::GetCurrentTime()),
		m_oleLastSuccessCom(COleDateTime::GetCurrentTime()),
		m_bRemoteParamValid(FALSE),
		m_bRemoteDateTimeValid(FALSE),
		m_bRemoteAlarmValid(FALSE),
		m_bRemoteInfoValid(FALSE)
	{
		ZeroMemory(&m_RemoteParam, sizeof(m_RemoteParam));
		ZeroMemory(&m_RemoteDateTime, sizeof(m_RemoteDateTime));
		ZeroMemory(&m_RemoteBatTemp, sizeof(m_RemoteBatTemp));

		ZeroMemory(&m_LocalParam, sizeof(m_LocalParam));
		ZeroMemory(&m_LocalDateTime, sizeof(m_LocalDateTime));
		ZeroMemory(&m_LocalAlarm, sizeof(m_LocalAlarm));

	}
	virtual ~CRemoteConfig() {
		if (m_RemoteAlarmArray)
			free(m_RemoteAlarmArray);
		m_RemoteAlarmArray = NULL;
	}
public:
	BOOL Initialize(CIVError& Error);
	void DeInitialize();
	BOOL LoadSerialConfig(CIVError& Error);
	BOOL Ping(CIVError & Error, HANDLE hWaitEvent = NULL);
	BOOL StartRemoteConfigMon(CIVError& Error);
	void StopRemoteConfigMon();
	void TryLoadRemoteConfig();

	BOOL GetParam(CIVError& Error, remote_control_body_param_t& param);
	BOOL IsParamValid() {
		return m_bRemoteParamValid;
	}
	BOOL GetDateTime(CIVError& Error, remote_control_body_time_t& datetime);
	BOOL IsDateTimeValid() {
		return m_bRemoteDateTimeValid;
	}

	BOOL SetParam(CIVError& Error, const remote_control_body_param_t& param);
	BOOL SetDateTime(CIVError& Error, const remote_control_body_time_t& datetime);
	BOOL SetDateTime(CIVError& Error, const COleDateTime & oleDateTime);
	BOOL GetAlarm(CIVError& Error, remote_control_body_alarm_t& alarm, INT nIndex);
	BOOL IsAlarmValid() {
		return m_bRemoteAlarmValid;
	}
	BOOL SetAlarm(CIVError& Error, const remote_control_body_alarm_t& alarm);
	BOOL GetInfo(CIVError& Error, remote_control_body_info_t& battemp);
	BOOL IsInfoValid() {
		return m_bRemoteInfoValid;
	}

	INT  GetAlarmCnt() {
		return m_nRemoteAlarmCnt;
	}
	INT GetAlarmSndCnt() {
		return m_nRemoteAlarmSndCnt;
	}

	BOOL AddTask(CIVError& Error, CTask::IV_TASK_TYPE_T eTaskType, HWND hCallbackHwnd, UINT nMessage, LPVOID pParam = NULL);

	void IntoTray() {
		m_bInTray = TRUE;
	}

	void LeaveTray() {
		m_bInTray = FALSE;
	}

	void SetParamHwnd(HWND hWnd) { m_hWndParam = hWnd; }
	void SetDateTimeHwnd(HWND hWnd) { m_hWndDateTime = hWnd; }
	void SetAlarmHwnd(HWND hWnd) { m_hWndAlarm = hWnd; }
	void SetInfoHwnd(HWND hWnd) { m_hWndInfo = hWnd; }

	void SetValid(BOOL bValid = TRUE) {
		m_bRemoteParamValid = bValid;
		m_bRemoteDateTimeValid = bValid;
		m_bRemoteAlarmValid = bValid;
		m_bRemoteInfoValid = bValid;
	}
protected:

	// 
	BOOL m_bInTray;
	
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

	// use by LoadRemoteXXX, and GetXXX
	BOOL m_bRemoteParamValid;
	remote_control_body_param_t m_RemoteParam;
	HWND m_hWndParam;

	BOOL m_bRemoteDateTimeValid;
	remote_control_body_time_t  m_RemoteDateTime;
	HWND m_hWndDateTime;

	BOOL m_bRemoteAlarmValid;
	remote_control_body_alarm_t* m_RemoteAlarmArray;
	INT m_nRemoteAlarmCnt;
	INT m_nRemoteAlarmSndCnt;
	HWND m_hWndAlarm;

	BOOL m_bRemoteInfoValid;
	remote_control_body_info_t m_RemoteBatTemp;
	HWND m_hWndInfo;

	// use by SetRemoteXXX, and SetXXX
	remote_control_body_param_t m_LocalParam;
	remote_control_body_time_t  m_LocalDateTime;
	remote_control_body_alarm_t m_LocalAlarm;


	// thread
	CWinThread* m_pRemoteConfigMon;
	BOOL        m_bQuitRemoteConfigMon;
	HANDLE      m_hQuitRemoteConfigMon;
	HANDLE      m_hTaskEvent;

	// task queue
	HANDLE      m_hSerialMutex;
	CTaskQueue  m_TaskQueue;
	COleDateTime m_oleLastSync;

	// last success load or set
	COleDateTime m_oleLastSuccessCom;
protected:

	BOOL ProcessSerialMsg(CSerialPortConnection* pConn, remote_control_msg_t& msg, CIVError& Error);
	BOOL LoadSetConfig(CIVError& Error, HANDLE hWaitEvent, remote_control_cmd_type_t eCmd);

	BOOL SetRemoteConfigParam(CIVError& Error, HANDLE hWaitEvent = NULL);
	BOOL SetRemoteConfigAlarm(CIVError& Error, HANDLE hWaitEvent = NULL);
	BOOL SetRemoteConfigDateTime(CIVError& Error, HANDLE hWaitEvent = NULL);

	BOOL LoadRemoteConfigParam(CIVError& Error, HANDLE hWaitEvent = NULL);
	BOOL LoadRemoteConfigAlarm(CIVError& Error, HANDLE hWaitEvent = NULL);
	BOOL LoadRemoteConfigDateTime(CIVError& Error, HANDLE hWaitEvent = NULL);
	BOOL LoadRemoteConfigInfo(CIVError& Error, HANDLE hWaitEvent = NULL);

	static UINT fnRemoteConfigMon(LPVOID pParam);
	BOOL DealTask(CIVError& Error);
	BOOL ScheduleTimeTask(CIVError& Error);
	BOOL ScheduleRevalidTask(CIVError& Error);
};

