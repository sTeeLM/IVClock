#pragma once

// CSerialPort 命令目标

class CSerialPortConnection;

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
        INT     nPortNbr;
    };
    typedef enum {
        STOP_BITS_1,
        STOP_BITS_1_5,
        STOP_BITS_2
    }STOP_BITS_TYPE_T;
	static void EnumerateSerialPorts();
    static INT GetPortNbr(INT nIndex);
    static INT GetPortCount();
    static INT GetBaudRateCount();
    static CString GetBaudRateDesc(INT nIndex);
    static INT GetBaudRate(INT nIndex);
    static INT GetDataBitsCount();
    static CString GetDataBitsDesc(INT nIndex);
    static INT GetDataBits(INT nIndex);
    static INT GetParityCount();
    static CString GetParityDesc(INT nIndex);
    static CHAR GetParity(INT nIndex);
    static INT GetStopBitsCount();
    static CString GetStopBitsDesc(INT nIndex);
    static STOP_BITS_TYPE_T GetStopBits(INT nIndex);
    static CString GetPortFriendlyName(INT nIndex);
    static INT PortNumberToIndex(INT nPortNumber);

    static CSerialPortConnection* OpenSerial(INT nPortNumber, INT nBaudRate, INT nDataBits, INT nParity, INT nStopBits,
        BOOL bDTRDSR, BOOL bRTSCTS, BOOL bXONXOFF
    );

private:
    static void EnumSerialPorts(CArray<CSerialPort::SSerInfo, CSerialPort::SSerInfo&>& asi, BOOL bIgnoreBusyPorts);
    static BOOL EnumPortsWdm(CArray<CSerialPort::SSerInfo, CSerialPort::SSerInfo&>& asi);
    static CArray<SSerInfo, SSerInfo&> m_aryPorts;
    static INT m_aryBaudRates[];
    static INT m_aryDataBits[];
    static CHAR m_aryParitys[];
    static LPCTSTR m_aryStopBits[];
};


class CSerialPortConnection : public CObject
{
public:
    CSerialPortConnection(INT nPortNbr, INT nBaudRate, INT nDataBits, CHAR cParity, CSerialPort::STOP_BITS_TYPE_T eStopBits,
        BOOL bDTRDSR, BOOL bRTSCTS, BOOL bXONXOFF) :
        m_nPortNbr(nPortNbr),
        m_nBaudRate(nBaudRate),
        m_nDataBits(nDataBits),
        m_cParity(cParity),
        m_eStopBits(eStopBits),
        m_bDTRDSR(bDTRDSR),
        m_bRTSCTS(bRTSCTS),
        m_bXONXOFF(bXONXOFF),
        m_hComm(INVALID_HANDLE_VALUE),
        m_dwReadIntervalTimeout(1000),
        m_dwReadTotalTimeoutMultiplier(1000),
        m_dwReadTotalTimeoutConstant(1000),
        m_dwWriteTotalTimeoutMultiplier(1000),
        m_dwWriteTotalTimeoutConstant(1000)
    {
        ZeroMemory(&m_CommTimeouts, sizeof(m_CommTimeouts));
        ZeroMemory(&m_dcb, sizeof(m_dcb));
    }
    virtual ~CSerialPortConnection() {}

    BOOL Open();
    void Close();
    BOOL TestError();
    BOOL WriteData(const LPBYTE pBuffer, SIZE_T nSize);
    BOOL ReadData(LPBYTE pBuffer, SIZE_T nSize);

protected:
    void ProcessError(LPCTSTR szWhere);
    
protected:
    // config
    INT m_nPortNbr;
    INT m_nBaudRate;
    INT m_nDataBits;
    CHAR m_cParity;
    CSerialPort::STOP_BITS_TYPE_T m_eStopBits;
    BOOL m_bDTRDSR;
    BOOL m_bRTSCTS;
    BOOL m_bXONXOFF;

    COMMTIMEOUTS  m_CommTimeouts;
    DCB           m_dcb;
    HANDLE        m_hComm;
    // timeouts
    DWORD m_dwReadIntervalTimeout;
    DWORD m_dwReadTotalTimeoutMultiplier;
    DWORD m_dwReadTotalTimeoutConstant;
    DWORD m_dwWriteTotalTimeoutMultiplier;
    DWORD m_dwWriteTotalTimeoutConstant;
};