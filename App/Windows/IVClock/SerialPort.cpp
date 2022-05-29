// SerialPort.cpp: 实现文件
//

#include "pch.h"
#include "IVClock.h"
#include "SerialPort.h"
#include <SetupAPI.h>

#pragma comment(lib,"setupapi")

// CSerialPort

CSerialPort::CSerialPort()
{
}

CSerialPort::~CSerialPort()
{
}


// CSerialPort 成员函数

CArray<CSerialPort::SSerInfo, CSerialPort::SSerInfo&> CSerialPort::m_aryPorts;

void CSerialPort::EnumerateSerialPorts()
{
	EnumSerialPorts(m_aryPorts, FALSE);
}

INT CSerialPort::GetPortCount()
{
	return (INT)m_aryPorts.GetCount();
}

INT CSerialPort::GetPortNbr(INT nIndex)
{
	if (nIndex < GetPortCount())
		return m_aryPorts[nIndex].nPortNbr;
	return 0;
}

INT CSerialPort::m_aryBaudRates[] = {
	110,
	300,
	600,
	1200,
	2400,
	4800,
	9600,
	14400,
	19200,
	38400,
	57600,
	115200,
	230400,
	380400,
	460800,
	921600
};

INT CSerialPort::GetBaudRateCount()
{
	return _countof(m_aryBaudRates);
}

CString CSerialPort::GetBaudRateDesc(INT nIndex)
{
	CString strRet = _T("");
	INT nBaudRate = GetBaudRate(nIndex);
	if(nBaudRate > 0)
		strRet.Format(_T("%d"), nBaudRate);
	return strRet;
}

INT CSerialPort::GetBaudRate(INT nIndex)
{
	if(nIndex >= 0 && nIndex < _countof(m_aryBaudRates))
		return m_aryBaudRates[nIndex];
	return 0;
}


INT CSerialPort::m_aryDataBits[] = {
	5,
	6,
	7,
	8
};

INT CSerialPort::GetDataBitsCount()
{
	return _countof(m_aryDataBits);
}

CString CSerialPort::GetDataBitsDesc(INT nIndex)
{
	CString strRet = _T("");
	INT nDataBits = GetDataBits(nIndex);
	if (nDataBits > 0)
		strRet.Format(_T("%d"), nDataBits);
	return strRet;
}

INT CSerialPort::GetDataBits(INT nIndex)
{
	if (nIndex >= 0 && nIndex < _countof(m_aryDataBits))
		return m_aryDataBits[nIndex];
	return 0;
}

CHAR CSerialPort::m_aryParitys[] = {
	'n',
	'o',
	'e',
	'm',
	's'
};

INT CSerialPort::GetParityCount()
{
	return _countof(m_aryParitys);
}

CString CSerialPort::GetParityDesc(INT nIndex)
{
	CString strRet = _T("");
	CHAR cPartiy = GetParity(nIndex);
	switch (cPartiy) {
	case 'n':
		strRet = _T("None"); break;
	case 'o':
		strRet = _T("Odd"); break;
	case 'e':
		strRet = _T("Even"); break;
	case 'm':
		strRet = _T("Mark"); break;
	case 's':
		strRet = _T("Space"); break;
	}
		
	return strRet;
}

CHAR CSerialPort::GetParity(INT nIndex)
{
	if (nIndex >= 0 && nIndex < _countof(m_aryParitys))
		return m_aryParitys[nIndex];
	return 0;
}

LPCTSTR CSerialPort::m_aryStopBits[] = {
	_T("1"),
	_T("1.5"),
	_T("2")
};

INT CSerialPort::GetStopBitsCount()
{
	return _countof(m_aryStopBits);
}

CString CSerialPort::GetStopBitsDesc(INT nIndex)
{
	if (nIndex >= 0 && nIndex < _countof(m_aryStopBits))
		return CString(m_aryStopBits[nIndex]);
	return CString(_T(""));
}

CSerialPort::STOP_BITS_TYPE_T CSerialPort::GetStopBits(INT nIndex)
{
	if (nIndex >= 0 && nIndex < _countof(m_aryStopBits))
		return (CSerialPort::STOP_BITS_TYPE_T)nIndex;
	return CSerialPort::STOP_BITS_1;
}

INT CSerialPort::PortNumberToIndex(INT nPortNumber)
{
	CString strPortName;
	strPortName.Format(_T("COM%d"), nPortNumber);
	for (INT i = 0; i < m_aryPorts.GetCount(); i++) {
		if (strPortName.CompareNoCase(m_aryPorts[i].strPortName) == 0)
			return i;
	}
	return -1;
}

CString CSerialPort::GetPortFriendlyName(INT nIndex)
{
	if(nIndex >= 0 && nIndex <= GetPortCount())
		return m_aryPorts[nIndex].strFriendlyName;
	return _T("");
}

void CSerialPort::EnumSerialPorts(CArray<CSerialPort::SSerInfo, CSerialPort::SSerInfo&>& asi, BOOL bIgnoreBusyPorts)
{
	// Clear the output array
	asi.RemoveAll();
	if (EnumPortsWdm(asi)) {

		for (int ii = 0; ii < asi.GetSize(); ii++)
		{
			SSerInfo& rsi = asi[ii];
			if (bIgnoreBusyPorts) {
				// Only display ports that can be opened for read/write
				HANDLE hCom = CreateFile(rsi.strDevPath,
					GENERIC_READ | GENERIC_WRITE,
					0,    /* comm devices must be opened w/exclusive-access */
					NULL, /* no security attrs */
					OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
					0,    /* not overlapped I/O */
					NULL  /* hTemplate must be NULL for comm devices */
				);
				if (hCom == INVALID_HANDLE_VALUE) {
					// It can't be opened; remove it.
					asi.RemoveAt(ii);
					ii--;
					continue;
				}
				else {
					// It can be opened! Close it and add it to the list
					::CloseHandle(hCom);
				}
			}

			if (rsi.strFriendlyName.IsEmpty())
			{
				asi.RemoveAt(ii);
				ii--;
				continue;
			}
			CString fName = rsi.strFriendlyName;
			TCHAR* pt, * pt1;
			pt = fName.GetBuffer() + fName.GetLength() - 1;
			while (*pt != _T('('))
			{
				pt--;
			}
			if (_tcsncmp(pt, _T("(COM"), 4))
			{
				asi.RemoveAt(ii);
				ii--;
				continue;
			}

			rsi.strPortName.Empty();
			rsi.strPortName = _T("COM");
			pt1 = pt + 4;
			rsi.nPortNbr = 0;
			while ((*pt1 >= _T('0')) && (*pt1 <= _T('9')))
			{
				rsi.nPortNbr *= 10;
				rsi.nPortNbr += (*pt1 - 48);
				rsi.strPortName += *pt1++;
			}
			
			pt -= 2;
			pt1 = fName.GetBuffer();
			rsi.strPortDesc.Empty();
			while (pt1 <= pt)
			{
				rsi.strPortDesc += *pt1++;
			}
		}
	}
}

extern "C" const GUID DECLSPEC_SELECTANY GUID_CLASS_COMPORT =
{ 0x86e0d1e0L, 0x8089, 0x11d0, { 0x9c, 0xe4,  0x08,  0x00,  0x3e,  0x30,  0x1f,  0x73 } };

BOOL CSerialPort::EnumPortsWdm(CArray<CSerialPort::SSerInfo, CSerialPort::SSerInfo&>& asi)
{
	CString strErr;
	// Create a device information set that will be the container for 
	// the device interfaces.
	GUID* guidDev = (GUID*)&GUID_CLASS_COMPORT;

	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DETAIL_DATA* pDetData = NULL;

	try {
		hDevInfo = SetupDiGetClassDevs(guidDev,
			NULL,
			NULL,
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
		);

		if (hDevInfo == INVALID_HANDLE_VALUE)
		{
			strErr.Format(_T("SetupDiGetClassDevs failed. (err=%lx)"),
				GetLastError());
			throw strErr;
		}

		// Enumerate the serial ports
		BOOL bOk = TRUE;
		SP_DEVICE_INTERFACE_DATA ifcData;
		DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 1024;
		pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new char[dwDetDataSize];
		// This is required, according to the documentation. Yes,
		// it's weird.
		ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		for (DWORD ii = 0; bOk; ii++) {
			bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
				NULL, guidDev, ii, &ifcData);
			if (bOk) {
				// Got a device. Get the details.
				SP_DEVINFO_DATA devdata = { sizeof(SP_DEVINFO_DATA) };
				bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
					&ifcData, pDetData, dwDetDataSize, NULL, &devdata);
				if (bOk) {
					CString strDevPath(pDetData->DevicePath);
					// Got a path to the device. Try to get some more info.
					TCHAR fname[256];
					BOOL bSuccess = SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
						(PBYTE)fname, sizeof(fname), NULL);
					if (bSuccess) {
						// Add an entry to the array
						SSerInfo si;
						si.strDevPath = strDevPath;
						si.strFriendlyName = fname;
						asi.Add(si);
					}

				}
				else {
					strErr.Format(_T("SetupDiGetDeviceInterfaceDetail failed. (err=%lx)"),
						GetLastError());
					throw strErr;
				}
			}
			else {
				DWORD err = GetLastError();
				if (err != ERROR_NO_MORE_ITEMS) {
					strErr.Format(_T("SetupDiEnumDeviceInterfaces failed. (err=%lx)"), err);
					throw strErr;
				}
			}
		}
	}
	catch (CString strCatchErr) {
		strErr = strCatchErr;
	}

	if (pDetData != NULL)
		delete[](char*)pDetData;
	if (hDevInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hDevInfo);

	if (!strErr.IsEmpty()) {
		TRACE(_T("%s\n"), strErr);
		return FALSE;
	}
	return TRUE;
}

CSerialPortConnection* CSerialPort::OpenSerial(INT nPortIndex, INT nBaudRateIndex, INT nDataBitsIndex, INT nParityIndex, INT nStopBitsIndex,
	BOOL bDTRDSR, BOOL bRTSCTS, BOOL bXONXOFF)
{
	INT nPortNbr  = GetPortNbr(nPortIndex);
	INT nBaudRate = GetBaudRate(nBaudRateIndex);
	INT nDataBits  = GetDataBits(nDataBitsIndex);
	CHAR cParity   = GetParity(nParityIndex);
	STOP_BITS_TYPE_T eStopBits = GetStopBits(nStopBitsIndex);

	
	CSerialPortConnection* pSerialPortConnection = new CSerialPortConnection(nPortNbr,
		nBaudRate, nDataBits, cParity, eStopBits, bDTRDSR, bRTSCTS, bXONXOFF);

	if (pSerialPortConnection == NULL)
		return NULL;

	if (!pSerialPortConnection->Open()) {
		pSerialPortConnection->Close();
		pSerialPortConnection = NULL;
	}

	return pSerialPortConnection;
}

void CSerialPortConnection::ProcessError(LPCTSTR szWhere)
{
	TRACE(_T("%s: %d\n"), szWhere,  GetLastError());
}

BOOL CSerialPortConnection::Open()
{
	CHAR szPort[50];
	CHAR szConfig[256];

	snprintf(szPort, sizeof(szPort), "COM%d", m_nPortNbr);
	szPort[sizeof(szPort) - 1] = 0;

	//COMx[:][baud=b][parity=p][data=d][stop=s][to={on|off}][xon={on|off}][odsr={on|off}][octs={on|off}][dtr={on|off|hs}][rts={on|off|hs|tg}][idsr={on|off}]
	snprintf(szConfig, sizeof(szConfig), "baud=%d parity=%c data=%d stop=%s xon=%s dtr=%s rts=%s",
		m_nBaudRate,
		m_cParity,
		m_nDataBits,
		m_eStopBits == CSerialPort::STOP_BITS_1 ? "1" : (m_eStopBits == CSerialPort::STOP_BITS_1_5 ? "1.5" : "2"),
		m_bXONXOFF ? "on" : "off",
		m_bDTRDSR ? "on" : "off",
		m_bRTSCTS ? "on" : "off");
	szConfig[sizeof(szConfig) - 1] = 0;

	if ((m_hComm = CreateFileA(szPort,      // communication port string (COMX)
		GENERIC_READ | GENERIC_WRITE, // read/write types
		0,        // comm devices must be opened with exclusive access
		NULL,       // no security attributes
		OPEN_EXISTING,     // comm devices must use OPEN_EXISTING
		NULL,   // Async I/O
		0))       // template must be 0 for comm devices
		== INVALID_HANDLE_VALUE) {
		ProcessError(_T("CreateFileA"));
		goto err;
	}

	m_CommTimeouts.ReadIntervalTimeout = m_dwReadIntervalTimeout;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = m_dwReadTotalTimeoutMultiplier;
	m_CommTimeouts.ReadTotalTimeoutConstant = m_dwReadTotalTimeoutConstant;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = m_dwWriteTotalTimeoutMultiplier;
	m_CommTimeouts.WriteTotalTimeoutConstant = m_dwWriteTotalTimeoutConstant;

	if (!SetCommTimeouts(m_hComm, &m_CommTimeouts)) {
		ProcessError(_T("SetCommTimeouts"));
		goto err;
	}

	if (!GetCommState(m_hComm, &m_dcb)) {
		ProcessError(_T("GetCommState"));
		goto err;
	}

	if (!BuildCommDCBA(szConfig, &m_dcb)) {
		ProcessError(_T("BuildCommDCBA"));
		goto err;
	}

	if (!SetCommState(m_hComm, &m_dcb)) {
		ProcessError(_T("SetCommState"));
		goto err;
	}

	if (!PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT)) {
		ProcessError(_T("PurgeComm"));
		goto err;
	}

	return TRUE;
err:
	if (m_hComm != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
	return FALSE;
}


void ProcessError(LPCTSTR szWhere)
{


}

void CSerialPortConnection::Close()
{
	if (m_hComm != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
	delete this;
}

BOOL CSerialPortConnection::TestError()
{
	DWORD dwEvtMask;
	if (GetCommMask(m_hComm, &dwEvtMask)) {
		TRACE(_T("GetCommMask return 0x%x on port %d\n"), dwEvtMask, m_nPortNbr);
		if (dwEvtMask & EV_BREAK) {
			TRACE(_T("TestError EV_BREAK"));
			return TRUE;
		}
		if (dwEvtMask & EV_ERR) {
			TRACE(_T("TestError EV_ERR"));
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSerialPortConnection::WriteData(const LPBYTE pBuffer, SIZE_T nSize)
{
	DWORD dwWritten;
	if (TestError()) {
		return FALSE;
	}
	if (nSize == 0) return TRUE;
	if (WriteFile(m_hComm, pBuffer, (DWORD)nSize, &dwWritten, NULL)) {
		if (nSize == dwWritten) {
			return TRUE;
		}
		else {
			ProcessError(_T("WriteFile: size mismatch"));
		}
	}
	else {
		ProcessError(_T("WriteFile"));
	}
	return FALSE;
}

BOOL CSerialPortConnection::ReadData(LPBYTE pBuffer, SIZE_T nSize)
{
	DWORD dwReaded;
	
	if (TestError()) {
		return FALSE;
	}

	if (nSize == 0) return TRUE;
	if (ReadFile(m_hComm, pBuffer, (DWORD)nSize, &dwReaded, NULL)) {
		if (nSize == dwReaded) {
			return TRUE;
		}
		else {
			ProcessError(_T("ReadFile: size mismatch"));
		}
	}
	else {
		ProcessError(_T("ReadFile"));
	}
	return FALSE;
}