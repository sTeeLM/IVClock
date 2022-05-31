#include "pch.h"
#include "IVClock.h"
#include "RemoteConfig.h"

BOOL CRemoteConfig::LoadSerialConfig(CIVError& Error)
{
	CConfigManager::CONFIG_VALUE_T val;

	Error.SetError(CIVError::IVE_NONE);

	theApp.m_Config.GetConfig(_T("connect"), _T("port"), val);
	m_nPort = val.u8;

	theApp.m_Config.GetConfig(_T("connect"), _T("baudrate"), val);
	m_nBaudRate = val.n8 < CSerialPort::GetBaudRateCount() ? val.u8 : 0;

	theApp.m_Config.GetConfig(_T("connect"), _T("databits"), val);
	m_nDataBits = val.n8 < CSerialPort::GetDataBitsCount() ? val.u8 : 0;

	theApp.m_Config.GetConfig(_T("connect"), _T("parity"), val);
	m_nParity = val.n8 < CSerialPort::GetParityCount() ? val.u8 : 0;

	theApp.m_Config.GetConfig(_T("connect"), _T("stopbits"), val);
	m_nStopBits = val.n8 < CSerialPort::GetStopBitsCount() ? val.u8 : 0;

	theApp.m_Config.GetConfig(_T("connect"), _T("dtr_dsr"), val);
	m_bRTSCTS = val.u8;

	theApp.m_Config.GetConfig(_T("connect"), _T("dts_cts"), val);
	m_bDTRDSR = val.u8;

	theApp.m_Config.GetConfig(_T("connect"), _T("xon_xoff"), val);
	m_bXONXOFF = val.u8;

	return TRUE;
}

BOOL CRemoteConfig::Ping(CIVError& Error)
{
	remote_control_msg_t msg;
	CSerialPortConnection* pConn = NULL;
	BOOL bRet = FALSE;

	WaitForSingleObject(m_hSerialMutex, INFINITE);

	if (!LoadSerialConfig(Error)) {
		Error.SetError(CIVError::IVE_CONFIG);
		goto err;
	}

	if ((pConn = CSerialPort::OpenSerial(
		m_nPort,
		m_nBaudRate,
		m_nDataBits,
		m_nParity,
		m_nStopBits,
		m_bRTSCTS,
		m_bDTRDSR,
		m_bXONXOFF
	)) == NULL) {
		Error.SetError(CIVError::IVE_SERIAL_CONN);
		goto err;
	}

	TRACE(_T("sizeof msg header %d\n"), sizeof(msg.header));
	ZeroMemory(&msg.header, sizeof(msg.header));
	msg.header.magic = REMOTE_CONTROL_MSG_HEADER_MAGIC;
	msg.header.cmd = REMOTE_CONTROL_CMD_PING;
	msg.header.length = 0;
	
	if (!ProcessSerialMsg(pConn, msg, Error)) {
		goto err;
	}
	
	bRet = TRUE;
err:
	ReleaseMutex(m_hSerialMutex);
	if (pConn)
		pConn->Close();
	return bRet;
}

UINT CRemoteConfig::fnDateTimeMon(LPVOID pParam)
{
	CRemoteConfig* pRemoteConfig = (CRemoteConfig*)pParam;

	DWORD dwWaitRes;
	CIVError Error;
		
	do {
		dwWaitRes = WaitForSingleObject(pRemoteConfig->m_hQuitRemoteConfigMon, 500);
		if (dwWaitRes == WAIT_FAILED) {
			break;
		}
	
		pRemoteConfig->LoadRemoteConfig(Error);

	} while (!pRemoteConfig->m_bQuitRemoteConfigMon);

	TRACE(_T("fnDateTimeMon quit!\n"));
	return 0;
}

void CRemoteConfig::StopRemoteConfigMon()
{
	if (m_pRemoteConfigMon) {
		m_bQuitRemoteConfigMon = TRUE;
		SetEvent(m_hQuitRemoteConfigMon);
		WaitForSingleObject(m_pRemoteConfigMon->m_hThread, INFINITE);
		delete m_pRemoteConfigMon;
		m_pRemoteConfigMon = NULL;
		m_bQuitRemoteConfigMon = FALSE;
	}

	if (m_hQuitRemoteConfigMon) {
		ResetEvent(m_hQuitRemoteConfigMon);
		CloseHandle(m_hQuitRemoteConfigMon);
		m_hQuitRemoteConfigMon = NULL;
	}
}

BOOL CRemoteConfig::StartRemoteConfigMon(CIVError& Error)
{
	DWORD dwCreateFlag = CREATE_SUSPENDED;

	m_bQuitRemoteConfigMon = FALSE;

	if (m_hQuitRemoteConfigMon) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}

	if (!(m_hQuitRemoteConfigMon = CreateEvent(NULL, TRUE, FALSE, _T("DateTimeMon")))) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}


	m_pRemoteConfigMon = AfxBeginThread(fnDateTimeMon, this, 0, 0, dwCreateFlag, NULL);
	if (m_pRemoteConfigMon) {
		m_pRemoteConfigMon->m_bAutoDelete = FALSE;
		m_pRemoteConfigMon->ResumeThread();
	}
	else {
		Error.SetError(CIVError::IVE_INTERNAL);
	}
	return m_pRemoteConfigMon != NULL;
}

BOOL CRemoteConfig::SetRemoteConfigParam(CIVError& Error)
{
	remote_control_msg_t msg;
	CSerialPortConnection* pConn = NULL;
	BOOL bRet = FALSE;

	WaitForSingleObject(m_hSerialMutex, INFINITE);

	if (!LoadSerialConfig(Error)) {
		Error.SetError(CIVError::IVE_CONFIG);
		goto err;
	}
	if ((pConn = CSerialPort::OpenSerial(
		m_nPort,
		m_nBaudRate,
		m_nDataBits,
		m_nParity,
		m_nStopBits,
		m_bRTSCTS,
		m_bDTRDSR,
		m_bXONXOFF
	)) == NULL) {
		Error.SetError(CIVError::IVE_SERIAL_CONN);
		goto err;
	}

	Sleep(1000);

	msg.header.magic = REMOTE_CONTROL_MSG_HEADER_MAGIC;
	msg.header.cmd = REMOTE_CONTROL_CMD_SET_PARAM;
	msg.header.length = sizeof(remote_control_body_param_t);

	CopyMemory(&msg.body, &m_Param, sizeof(msg.body));

	if (!ProcessSerialMsg(pConn, msg, Error)) {
		goto err;
	}

	bRet = TRUE;
err:
	ReleaseMutex(m_hSerialMutex);
	if (pConn)
		pConn->Close();
	return bRet;
}

BOOL CRemoteConfig::SetRemoteConfigAlarm(INT nAlarmIndex, CIVError& Error)
{
	return TRUE;
}

BOOL CRemoteConfig::SetRemoteConfigDateTime(CIVError& Error)
{
	return TRUE;
}

BOOL CRemoteConfig::ProcessSerialMsg(CSerialPortConnection* pConn, remote_control_msg_t &msg, CIVError& Error)
{
	uint8_t cmd = msg.header.cmd;
	PBYTE pBody = (PBYTE)&msg;
	pBody += sizeof(msg.header);

	if (msg.header.magic != REMOTE_CONTROL_MSG_HEADER_MAGIC
		|| msg.header.length > sizeof(remote_control_msg_t)
		- sizeof(remote_control_msg_header_t)) {
		TRACE(_T("ERROR ProcessSerialMsg:IVE_SERIAL_PROTOCAL\n"));
		Error.SetError(CIVError::IVE_SERIAL_PROTOCAL);
		goto err;
	}

	if (!pConn->WriteData((LPBYTE)&msg, sizeof(msg.header))) {
		TRACE(_T("ERROR ProcessSerialMsg:IVE_SERIAL_WRITE\n"));
		Error.SetError(CIVError::IVE_SERIAL_WRITE);
		goto err;
	}

	Sleep(100);

	if (msg.header.length > 0) {
		if (!pConn->WriteData((LPBYTE)pBody, msg.header.length)) {
			TRACE(_T("ERROR ProcessSerialMsg:IVE_SERIAL_WRITE\n"));
			Error.SetError(CIVError::IVE_SERIAL_WRITE);
			goto err;
		}
	}

	if (!pConn->ReadData((LPBYTE)&msg, sizeof(msg.header))) {
		TRACE(_T("ERROR ProcessSerialMsg:IVE_SERIAL_READ\n"));
		Error.SetError(CIVError::IVE_SERIAL_READ);
		goto err;
	}

	if (msg.header.res != cmd + REMOTE_CONTROL_RES_BASE
		|| msg.header.magic != REMOTE_CONTROL_MSG_HEADER_MAGIC
		|| msg.header.code != REMOTE_CONTROL_CODE_OK) {
		TRACE(_T("ERROR ProcessSerialMsg:IVE_SERIAL_PROTOCAL\n"));
		Error.SetError(CIVError::IVE_SERIAL_PROTOCAL);
		goto err;
	}

	if (msg.header.length > 0 && msg.header.length <= sizeof(remote_control_msg_t)
		- sizeof(remote_control_msg_header_t)) {
		if (!pConn->ReadData(pBody, msg.header.length)) {
			TRACE(_T("ERROR ProcessSerialMsg:IVE_SERIAL_READ\n"));
			Error.SetError(CIVError::IVE_SERIAL_READ);
			goto err;
		}
	}

	return TRUE;
err:
	return FALSE;
}

BOOL CRemoteConfig::LoadRemoteConfigParam(CIVError& Error)
{
	remote_control_msg_t msg;
	CSerialPortConnection* pConn = NULL;
	BOOL bRet = FALSE;

	WaitForSingleObject(m_hSerialMutex, INFINITE);

	if (!LoadSerialConfig(Error)) {
		Error.SetError(CIVError::IVE_CONFIG);
		goto err;
	}
	if ((pConn = CSerialPort::OpenSerial(
		m_nPort,
		m_nBaudRate,
		m_nDataBits,
		m_nParity,
		m_nStopBits,
		m_bRTSCTS,
		m_bDTRDSR,
		m_bXONXOFF
	)) == NULL) {
		Error.SetError(CIVError::IVE_SERIAL_DEVICE);
		goto err;
	}

	Sleep(1000);

	msg.header.magic = REMOTE_CONTROL_MSG_HEADER_MAGIC;
	msg.header.cmd = REMOTE_CONTROL_CMD_GET_PARAM;
	msg.header.length = sizeof(remote_control_body_param_t);
	if (!ProcessSerialMsg(pConn, msg, Error)) {
		goto err;
	}
	CopyMemory(&m_Param, &msg.body.param, sizeof(remote_control_body_param_t));

	bRet = TRUE;
err:
	ReleaseMutex(m_hSerialMutex);
	if (pConn)
		pConn->Close();
	return bRet;
}


BOOL CRemoteConfig::LoadRemoteConfigAlarm(CIVError& Error)
{
	remote_control_msg_t msg;
	CSerialPortConnection* pConn = NULL;
	BOOL bRet = FALSE;
	
	WaitForSingleObject(m_hSerialMutex, INFINITE);

	if (!LoadSerialConfig(Error)) {
		Error.SetError(CIVError::IVE_CONFIG);
		goto err;
	}

	
	if ((pConn = CSerialPort::OpenSerial(
		m_nPort,
		m_nBaudRate,
		m_nDataBits,
		m_nParity,
		m_nStopBits,
		m_bRTSCTS,
		m_bDTRDSR,
		m_bXONXOFF
	)) == NULL) {
		Error.SetError(CIVError::IVE_SERIAL_DEVICE);
		goto err;
	}

	Sleep(1000);

	// get alarm count
	msg.header.magic = REMOTE_CONTROL_MSG_HEADER_MAGIC;
	msg.header.cmd = REMOTE_CONTROL_CMD_GET_ALARM;
	msg.header.length = sizeof(remote_control_body_alarm_t);
	msg.body.alarm.alarm_index = (uint8_t)(-1);
	if (!ProcessSerialMsg(pConn, msg, Error)) {
		goto err;
	}
	TRACE(_T("LoadRemoteConfigAlarm alarm cnt is %d\n"), msg.body.alarm.alarm_index);
	// alloc alarm memory ??
	if (m_AlarmArray != NULL && m_nAlarmCnt != msg.body.alarm.alarm_index
		|| m_AlarmArray == NULL) {
		if (m_AlarmArray) {
			free(m_AlarmArray);
			m_AlarmArray = NULL;
			m_nAlarmCnt = 0;
		}
		m_AlarmArray = (remote_control_body_alarm_t*)
			malloc(sizeof(remote_control_body_alarm_t) * msg.body.alarm.alarm_index);

		if (!m_AlarmArray) {
			Error.SetError(CIVError::IVE_NOMEM);
			goto err;
		}
		m_nAlarmCnt = msg.body.alarm.alarm_index;
		ZeroMemory(m_AlarmArray, sizeof(remote_control_body_alarm_t) * m_nAlarmCnt);
	}

	// read alarm
	for (INT i = 0; i < m_nAlarmCnt; i++) {
		msg.header.magic = REMOTE_CONTROL_MSG_HEADER_MAGIC;
		msg.header.cmd = REMOTE_CONTROL_CMD_GET_ALARM;
		msg.header.length = sizeof(remote_control_body_alarm_t);

		msg.body.alarm.alarm_index = i;
		if (!ProcessSerialMsg(pConn, msg, Error)) {
			//goto err;
		}

		CopyMemory(m_AlarmArray + i, &msg.body.alarm, sizeof(remote_control_body_alarm_t));
	}
	bRet = TRUE;
err:
	ReleaseMutex(m_hSerialMutex);
	if (pConn)
		pConn->Close();
	return bRet;
}

BOOL CRemoteConfig::LoadRemoteConfigDateTime(CIVError& Error)
{
	remote_control_msg_t msg;
	CSerialPortConnection* pConn = NULL;
	BOOL bRet = FALSE;

	WaitForSingleObject(m_hSerialMutex, INFINITE);

	if (!LoadSerialConfig(Error)) {
		Error.SetError(CIVError::IVE_CONFIG);
		goto err;
	}

	if ((pConn = CSerialPort::OpenSerial(
		m_nPort,
		m_nBaudRate,
		m_nDataBits,
		m_nParity,
		m_nStopBits,
		m_bRTSCTS,
		m_bDTRDSR,
		m_bXONXOFF
	)) == NULL) {
		Error.SetError(CIVError::IVE_SERIAL_DEVICE);
		goto err;
	}

	Sleep(1000);

	msg.header.magic = REMOTE_CONTROL_MSG_HEADER_MAGIC;
	msg.header.cmd = REMOTE_CONTROL_CMD_GET_TIME;
	msg.header.length = sizeof(remote_control_body_time_t);
	if (!ProcessSerialMsg(pConn, msg, Error)) {
		goto err;
	}
	CopyMemory(&m_DateTime, &msg.body.time, sizeof(remote_control_body_time_t));

	bRet = TRUE;
err:
	ReleaseMutex(m_hSerialMutex);
	if (pConn)
		pConn->Close();
	return bRet;
}

void CRemoteConfig::DeInitialize() 
{
	if (m_hSerialMutex) {
		CloseHandle(m_hSerialMutex);
		m_hSerialMutex = NULL;
	}
}

BOOL CRemoteConfig::Initialize(CIVError& Error)
{
	CSerialPort::EnumerateSerialPorts();

	if ((m_hSerialMutex = CreateMutex(NULL, FALSE, _T("SerialMutex"))) == NULL) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}
	return TRUE;
}

BOOL CRemoteConfig::LoadRemoteConfig(CIVError& Error)
{
	if (!LoadRemoteConfigParam(Error))
		return FALSE;

	if (!LoadRemoteConfigAlarm(Error))
		return FALSE;

	if (!LoadRemoteConfigDateTime(Error))
		return FALSE;

	return TRUE;
}
