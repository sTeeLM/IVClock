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
	return m_aryPorts.GetCount();
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


INT CSerialPort::GetDataBits(INT nIndex)
{
	if (nIndex >= 0 && nIndex < _countof(m_aryDataBits))
		return m_aryDataBits[nIndex];
	return 0;
}

LPCTSTR CSerialPort::m_aryParitys[] = {
	_T("None"),
	_T("Odd"),
	_T("Even"),
	_T("Mark"),
	_T("Space")
};

INT CSerialPort::GetParityCount()
{
	return _countof(m_aryParitys);
}

CString CSerialPort::GetParity(INT nIndex)
{
	if (nIndex >= 0 && nIndex < _countof(m_aryParitys))
		return m_aryParitys[nIndex];
	return _T("");
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

CString CSerialPort::GetStopBits(INT nIndex)
{
	if (nIndex >= 0 && nIndex < _countof(m_aryStopBits))
		return m_aryStopBits[nIndex];
	return 0;
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
			while ((*pt1 >= _T('0')) && (*pt1 <= _T('9')))
			{
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

CSerialPortConnection* CSerialPort::OpenSerial(INT nPort, INT nBaudRate, INT nDataBits, INT nParity, INT nStopBits,
	BOOL bDTRDSR, BOOL bRTSCTS, BOOL bXONXOFF
)
{
	return new CSerialPortConnection();
}