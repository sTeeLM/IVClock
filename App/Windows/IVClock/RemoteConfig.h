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
		m_nAlarmCnt(0)
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

	typedef enum {
		REMOTE_CONFIG_TYPE_ALL,
		REMOTE_CONFIG_TYPE_PARAM,
		REMOTE_CONFIG_TYPE_ALARM,
		REMOTE_CONFIG_TYPE_DATETIME,
	}REMOTE_CONFIG_TYPE_T;

	BOOL SetRemoteConfig(REMOTE_CONFIG_TYPE_T eType, CIVError& Error);
	BOOL LoadRemoteConfig(CIVError& Error);
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

	BOOL ProcessSerialMsg(CSerialPortConnection* pConn, remote_control_msg_t& msg, CIVError& Error);
};

