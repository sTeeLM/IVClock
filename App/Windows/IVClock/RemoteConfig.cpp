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

BOOL CRemoteConfig::Ping(CIVError& Error, HANDLE hWaitEvent/* = NULL */ )
{
	return LoadSetConfig(Error, hWaitEvent, REMOTE_CONTROL_CMD_PING);
}

UINT CRemoteConfig::fnRemoteConfigMon(LPVOID pParam)
{
	CRemoteConfig* pRemoteConfig = (CRemoteConfig*)pParam;
	DWORD dwWaitRes;
	CIVError Error;
	HANDLE aryEvent[2] = {  pRemoteConfig->m_hQuitRemoteConfigMon, pRemoteConfig->m_hTaskEvent };
		
	do {
		dwWaitRes = ::WaitForMultipleObjects(2, aryEvent, FALSE, 10000);
		if (dwWaitRes == WAIT_FAILED) {
			Error.SetError(CIVError::IVE_INTERNAL);
			break;
		}
		else if (dwWaitRes == WAIT_OBJECT_0) {
			break;
		}
		
		if (pRemoteConfig->m_bQuitRemoteConfigMon) {
			break;
		}

		pRemoteConfig->RemoteConfigMon(Error);

	} while (!pRemoteConfig->m_bQuitRemoteConfigMon);

	TRACE(_T("fnRemoteConfigMon quit!\n"));
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

	if (m_hTaskEvent) {
		ResetEvent(m_hTaskEvent);
		CloseHandle(m_hTaskEvent);
		m_hTaskEvent = NULL;
	}
}

BOOL CRemoteConfig::StartRemoteConfigMon(CIVError& Error)
{
	DWORD dwCreateFlag = CREATE_SUSPENDED;

	if (m_hQuitRemoteConfigMon) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}

	m_bQuitRemoteConfigMon = FALSE;

	if (!(m_hQuitRemoteConfigMon = CreateEvent(NULL, TRUE, FALSE, _T("RemoteConfigMon")))) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}

	if (!(m_hTaskEvent = CreateEvent(NULL, TRUE, FALSE, _T("RemoteConfigMonTask")))) {
		Error.SetError(CIVError::IVE_INTERNAL);
		CloseHandle(m_hQuitRemoteConfigMon);
		m_hQuitRemoteConfigMon = NULL;
		return FALSE;
	}
	

	m_pRemoteConfigMon = AfxBeginThread(fnRemoteConfigMon, this, 0, 0, dwCreateFlag, NULL);
	if (m_pRemoteConfigMon) {
		m_pRemoteConfigMon->m_bAutoDelete = FALSE;
		m_pRemoteConfigMon->ResumeThread();
	}
	else {
		CloseHandle(m_hQuitRemoteConfigMon);
		m_hQuitRemoteConfigMon = NULL;
		CloseHandle(m_hTaskEvent);
		m_hTaskEvent = NULL;
		Error.SetError(CIVError::IVE_INTERNAL);
	}
	return m_pRemoteConfigMon != NULL;
}


BOOL CRemoteConfig::LoadSetConfig(CIVError& Error, HANDLE hWaitEvent, remote_control_cmd_type_t eCmd, INT nIndex /* = 0*/)
{
	remote_control_msg_t msg;
	CSerialPortConnection* pConn = NULL;
	BOOL bRet = FALSE;
	DWORD dwWaitRes;

	dwWaitRes = WaitForSingleObject(m_hDataMutex, INFINITE);
	if (dwWaitRes == WAIT_FAILED) {
		Error.SetError(CIVError::IVE_INTERNAL);
		goto err;
	}

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

	if (hWaitEvent) {
		dwWaitRes = WaitForSingleObject(hWaitEvent, 1000);
		if (dwWaitRes == WAIT_FAILED) {
			Error.SetError(CIVError::IVE_INTERNAL);
			goto err;
		}
		else if (dwWaitRes == WAIT_OBJECT_0) {
			Error.SetError(CIVError::IVE_CANCELED);
			goto err;
		}
	}
	else {
		Sleep(1000);
	}

	msg.header.magic = REMOTE_CONTROL_MSG_HEADER_MAGIC;
	msg.header.cmd   = eCmd;
	msg.header.code  = REMOTE_CONTROL_CODE_OK;
	switch (eCmd) {
	case REMOTE_CONTROL_CMD_PING:
		msg.header.length = 0;
		break;
	case REMOTE_CONTROL_CMD_SET_TIME:
		msg.header.length = sizeof(remote_control_body_time_t);
		CopyMemory(&msg.body, &m_DateTime, sizeof(m_DateTime));
		break;
	case REMOTE_CONTROL_CMD_GET_TIME:
		msg.header.length = 0;
		ZeroMemory(&msg.body, sizeof(m_DateTime));
		break;
	case REMOTE_CONTROL_CMD_SET_PARAM:
		msg.header.length = sizeof(remote_control_body_param_t);
		CopyMemory(&msg.body, &m_Param, sizeof(m_Param));
		break;
	case REMOTE_CONTROL_CMD_GET_PARAM:
		msg.header.length = 0;
		ZeroMemory(&msg.body, sizeof(m_Param));
		break;
	case REMOTE_CONTROL_CMD_SET_ALARM:
		msg.header.length = sizeof(remote_control_body_alarm_t);
		msg.body.alarm.alarm_index = nIndex;
		CopyMemory(&msg.body, &m_AlarmArray[nIndex],sizeof(remote_control_body_alarm_t));
		break;
	}

	if (!ProcessSerialMsg(pConn, msg, Error)) {
		goto err;
	}

	switch (eCmd) {
	case REMOTE_CONTROL_CMD_GET_TIME:
		CopyMemory(&m_DateTime, &msg.body, sizeof(m_DateTime));
		break;
	case REMOTE_CONTROL_CMD_GET_PARAM:
		CopyMemory(&m_Param, &msg.body, sizeof(m_Param));
		break;
	}

	bRet = TRUE;
err:
	ReleaseMutex(m_hDataMutex);
	if (pConn)
		pConn->Close();
	return bRet;
}

BOOL CRemoteConfig::SetRemoteConfigParam(CIVError& Error, HANDLE hWaitEvent /* = NULL*/)
{
	return LoadSetConfig(Error, hWaitEvent, REMOTE_CONTROL_CMD_SET_PARAM);
}

BOOL CRemoteConfig::SetRemoteConfigAlarm(INT nAlarmIndex, CIVError& Error, HANDLE hWaitEvent /* = NULL */)
{
	return LoadSetConfig(Error, hWaitEvent, REMOTE_CONTROL_CMD_SET_ALARM, nAlarmIndex);
}

BOOL CRemoteConfig::SetRemoteConfigDateTime(CIVError& Error, HANDLE hWaitEvent /* = NULL */)
{
	return LoadSetConfig(Error, hWaitEvent, REMOTE_CONTROL_CMD_SET_TIME);
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

BOOL CRemoteConfig::LoadRemoteConfigParam(CIVError& Error, HANDLE hWaitEvent/* = NULL*/)
{
	return LoadSetConfig(Error, hWaitEvent, REMOTE_CONTROL_CMD_GET_PARAM);
}


BOOL CRemoteConfig::LoadRemoteConfigAlarm(CIVError& Error, HANDLE hWaitEvent/* = NULL */)
{
	remote_control_msg_t msg;
	CSerialPortConnection* pConn = NULL;
	BOOL bRet = FALSE;
	DWORD dwWaitRes;
	
	dwWaitRes = WaitForSingleObject(m_hDataMutex, INFINITE);
	if (dwWaitRes == WAIT_FAILED) {
		Error.SetError(CIVError::IVE_INTERNAL);
		goto err;
	}

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

	if (hWaitEvent) {
		dwWaitRes = WaitForSingleObject(hWaitEvent, 1000);
		if (dwWaitRes == WAIT_FAILED) {
			Error.SetError(CIVError::IVE_INTERNAL);
			goto err;
		}
		else if (dwWaitRes == WAIT_OBJECT_0) {
			Error.SetError(CIVError::IVE_CANCELED);
			goto err;
		}

	}
	else {
		Sleep(1000);
	}

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

	if (!bRet) {
		if (m_AlarmArray) {
			free(m_AlarmArray);
			m_AlarmArray = NULL;
		}
		m_nAlarmCnt = 0;
	}

	ReleaseMutex(m_hDataMutex);
	if (pConn)
		pConn->Close();
	return bRet;
}

BOOL CRemoteConfig::LoadRemoteConfigDateTime(CIVError& Error, HANDLE hWaitEvent/* = NULL */)
{
	return LoadSetConfig(Error, hWaitEvent, REMOTE_CONTROL_CMD_GET_TIME);
}

BOOL CRemoteConfig::SetParam(CIVError& Error, const remote_control_body_param_t& param)
{
	DWORD dwWaitRes;
	dwWaitRes = WaitForSingleObject(m_hDataMutex, INFINITE);
	if (dwWaitRes == WAIT_FAILED) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}

	CopyMemory(&m_Param, &param, sizeof(m_Param));

	ReleaseMutex(m_hDataMutex);
	return TRUE;
}

BOOL CRemoteConfig::GetParam(CIVError& Error, remote_control_body_param_t& param)
{
	DWORD dwWaitRes;
	dwWaitRes = WaitForSingleObject(m_hDataMutex, INFINITE);
	if (dwWaitRes == WAIT_FAILED) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}

	CopyMemory(&param, &m_Param, sizeof(m_Param));

	ReleaseMutex(m_hDataMutex);
	return TRUE;
}


BOOL CRemoteConfig::SetDateTime(CIVError& Error, const remote_control_body_time_t& datetime)
{
	DWORD dwWaitRes;
	dwWaitRes = WaitForSingleObject(m_hDataMutex, INFINITE);
	if (dwWaitRes == WAIT_FAILED) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}

	CopyMemory(&m_DateTime, &datetime, sizeof(m_DateTime));

	ReleaseMutex(m_hDataMutex);
	return TRUE;
}

BOOL CRemoteConfig::SetDateTime(CIVError& Error, const COleDateTime& oleDateTime)
{
	remote_control_body_time_t datetime;
	datetime.year = oleDateTime.GetYear() - 2000;
	datetime.mon = oleDateTime.GetMonth();
	datetime.date = oleDateTime.GetDay();
	datetime.hour = oleDateTime.GetHour();
	datetime.min = oleDateTime.GetMinute();
	datetime.sec = oleDateTime.GetSecond();
	return SetDateTime(Error, datetime);
}


BOOL CRemoteConfig::GetDateTime(CIVError& Error, remote_control_body_time_t& datetime)
{
	DWORD dwWaitRes;
	dwWaitRes = WaitForSingleObject(m_hDataMutex, INFINITE);
	if (dwWaitRes == WAIT_FAILED) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}

	CopyMemory(&datetime, &m_DateTime, sizeof(m_DateTime));

	ReleaseMutex(m_hDataMutex);
	return TRUE;
}

BOOL CRemoteConfig::AddTask(CIVError& Error, CTask::IV_TASK_TYPE_T eTaskType, HWND hCallbackHwnd, UINT nMessage, LPVOID pParam /* = NULL */ )
{
	CTask* pTask = NULL;
	DWORD dwWaitRes;
	BOOL bRet = FALSE;

	dwWaitRes = WaitForSingleObject(m_hSerialMutex, INFINITE);
	if (dwWaitRes == WAIT_FAILED) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}

	pTask = new CTask(hCallbackHwnd, eTaskType, nMessage, pParam);
	if (pTask) {
		m_TaskQueue.AddToEnd(pTask);
		bRet = TRUE;
	}
	ReleaseMutex(m_hSerialMutex);

	PulseEvent(m_hTaskEvent);

	return bRet;
}

BOOL CRemoteConfig::RemoteConfigMon(CIVError& Error)
{
	DWORD dwWaitRes;
	BOOL bRet = FALSE;
	CTask* pTask = NULL;

	// fetch a task
	dwWaitRes = WaitForSingleObject(m_hSerialMutex, INFINITE);
	if (dwWaitRes == WAIT_FAILED) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}
	pTask = m_TaskQueue.GetFromFront();
	ReleaseMutex(m_hSerialMutex);

	if (!pTask) { // 没有task，是否要执行定时任务？
		if (m_bInTray) { // 最小化了，每特定间隔时间同步一次本地时间
			TRACE(_T("Check if Sync Time\n"));
			CConfigManager::CONFIG_VALUE_T val;
			theApp.m_Config.GetConfig(_T("time_sync"), _T("enable"), val);
			if (val.u8) {
				theApp.m_Config.GetConfig(_T("time_sync"), _T("interval_sec"), val);
				COleDateTime oleNow = COleDateTime::GetCurrentTime();
				if ((oleNow - m_oleLastSync).GetSeconds() > val.u32) {
					COleDateTimeSpan OneSec(0, 0, 0, 1); // 同步需要大约1S
					m_oleLastSync = oleNow + OneSec;
					if (SetDateTime(Error, m_oleLastSync)) {
						pTask = new CTask(m_hWndDateTime, CTask::IV_TASK_SET_TIME, WM_CB_SET_TIME, 0);
						if (!pTask) {
							Error.SetError(CIVError::IVE_NOMEM);
							return FALSE;
						}
					}
					else {
						return FALSE;
					}
				}
			}
		}

		if(!pTask && !m_bInTray) { // 没有最小化，每10s读取一次远程时间
			TRACE(_T("Refresh Time\n"));
			pTask = new CTask(m_hWndDateTime, CTask::IV_TASK_GET_TIME, WM_CB_GET_TIME, 0);
			if (!pTask) {
				Error.SetError(CIVError::IVE_NOMEM);
				return FALSE;
			}
		}
	}

	if (!pTask) {
		return TRUE;
	}

	switch (pTask->m_eTaskType) {
	case CTask::IV_TASK_PING:
		pTask->m_bRes = Ping(pTask->m_Error, m_hQuitRemoteConfigMon);
		break;
	case CTask::IV_TASK_GET_PARAM:
		pTask->m_bRes = LoadRemoteConfigParam(pTask->m_Error, m_hQuitRemoteConfigMon);
		break;
	case CTask::IV_TASK_SET_PARAM:
		pTask->m_bRes = SetRemoteConfigParam(pTask->m_Error, m_hQuitRemoteConfigMon);
		break;
	case CTask::IV_TASK_GET_ALARM:
		pTask->m_bRes = LoadRemoteConfigAlarm(pTask->m_Error, m_hQuitRemoteConfigMon);
		break;
	case CTask::IV_TASK_SET_ALARM:
		pTask->m_bRes = SetRemoteConfigAlarm((UINT)pTask->m_pParam, pTask->m_Error, m_hQuitRemoteConfigMon);
		break;
	case CTask::IV_TASK_GET_TIME:
		pTask->m_bRes = LoadRemoteConfigDateTime(pTask->m_Error, m_hQuitRemoteConfigMon);
		break;
	case CTask::IV_TASK_SET_TIME:
		pTask->m_bRes = SetRemoteConfigDateTime(pTask->m_Error, m_hQuitRemoteConfigMon);
		break;
	default:
		TRACE(_T("UNKNOWN TASK TYPE % d"), pTask->m_eTaskType);
		Error.SetError(CIVError::IVE_INTERNAL);
		delete pTask;
		return FALSE;
	}

	SendMessage(pTask->m_hWnd, pTask->m_nMessage, (WPARAM) pTask, 0);

	delete pTask;

	Error.SetError(CIVError::IVE_NONE);
	return TRUE;
}

void CRemoteConfig::DeInitialize() 
{
	if (m_hSerialMutex) {
		CloseHandle(m_hSerialMutex);
		m_hSerialMutex = NULL;
	}

	if (m_hDataMutex) {
		CloseHandle(m_hDataMutex);
		m_hDataMutex = NULL;
	}

	m_TaskQueue.Clear();
}

BOOL CRemoteConfig::Initialize(CIVError& Error)
{
	CSerialPort::EnumerateSerialPorts();

	if ((m_hSerialMutex = CreateMutex(NULL, FALSE, _T("SerialMutex"))) == NULL) {
		Error.SetError(CIVError::IVE_INTERNAL);
		return FALSE;
	}
	if ((m_hDataMutex = CreateMutex(NULL, FALSE, _T("DataMutex"))) == NULL) {
		Error.SetError(CIVError::IVE_INTERNAL);
		CloseHandle(m_hSerialMutex);
		return FALSE;
	}
	return TRUE;
}

BOOL CRemoteConfig::LoadRemoteConfig(CIVError& Error, HANDLE hWaitEvent/* = NULL */)
{
	if (!LoadRemoteConfigParam(Error, hWaitEvent))
		return FALSE;

	if (!LoadRemoteConfigAlarm(Error, hWaitEvent))
		return FALSE;

	if (!LoadRemoteConfigDateTime(Error, hWaitEvent))
		return FALSE;

	return TRUE;
}


