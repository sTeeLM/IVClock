#pragma once

// CSerialPort 命令目标

class CSerialPortConnection : public CObject
{
public:
    CSerialPortConnection() {}
    virtual ~CSerialPortConnection() {}

public:


};

class CSerialPort : public CObject
{
public:
	CSerialPort();
	virtual ~CSerialPort();

public:
    struct SSerInfo {
        CString strDevPath;          // Device path for use with CreateFile()
        CString strPortName;         // Simple name (i.e. COM1)
        CString strFriendlyName;     // Full name to be displayed to a user
        CString strPortDesc;         // friendly name without the COMx
    };
	static void EnumerateSerialPorts();
    static INT GetPortCount();
    static INT GetBaudRateCount();
    static INT GetBaudRate(INT nIndex);
    static INT GetDataBitsCount();
    static INT GetDataBits(INT nIndex);
    static INT GetParityCount();
    static CString GetParity(INT nIndex);
    static INT GetStopBitsCount();
    static CString GetStopBits(INT nIndex);
    static CString GetPortFriendlyName(INT nIndex);

    static CSerialPortConnection* OpenSerial(INT nPort, INT nBaudRate, INT nDataBits, INT nParity, INT nStopBits,
        BOOL bDTRDSR, BOOL bRTSCTS, BOOL bXONXOFF
    );

private:
    static void EnumSerialPorts(CArray<CSerialPort::SSerInfo, CSerialPort::SSerInfo&>& asi, BOOL bIgnoreBusyPorts);
    static BOOL EnumPortsWdm(CArray<CSerialPort::SSerInfo, CSerialPort::SSerInfo&>& asi);
    static CArray<SSerInfo, SSerInfo&> m_aryPorts;
    static INT m_aryBaudRates[];
    static INT m_aryDataBits[];
    static LPCTSTR m_aryParitys[];
    static LPCTSTR m_aryStopBits[];
};


