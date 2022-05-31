#pragma once
#include <afx.h>
#include "ConfigManager.h"
#include "SerialPort.h"
#include "IVError.h"
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
		m_hSerialMutex(NULL)
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

	BOOL Ping(CIVError & Error);

	BOOL StartRemoteConfigMon(CIVError& Error);

	void StopRemoteConfigMon();

	BOOL Initialize(CIVError& Error);
	void DeInitialize();

	BOOL SetRemoteConfigParam(CIVError& Error);
	BOOL SetRemoteConfigAlarm(INT nAlarmIndex, CIVError& Error);
	BOOL SetRemoteConfigDateTime(CIVError& Error);

	BOOL LoadRemoteConfig(CIVError& Error);
	BOOL LoadRemoteConfigParam(CIVError& Error);
	BOOL LoadRemoteConfigAlarm(CIVError& Error);
	BOOL LoadRemoteConfigDateTime(CIVError& Error);

	remote_control_body_param_t& GetParam()
	{
		return m_Param;
	}
protected:
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
	remote_control_body_param_t m_Param;
	remote_control_body_time_t  m_DateTime;
	remote_control_body_alarm_t* m_AlarmArray;
	INT m_nAlarmCnt;

	// thread
	CWinThread* m_pRemoteConfigMon;
	BOOL        m_bQuitRemoteConfigMon;
	HANDLE      m_hQuitRemoteConfigMon;

	// mutex
	HANDLE      m_hSerialMutex;
protected:
	static UINT fnDateTimeMon(LPVOID pParam);
	BOOL ProcessSerialMsg(CSerialPortConnection* pConn, remote_control_msg_t& msg, CIVError& Error);
};

