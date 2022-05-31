#pragma once

// CIVError 命令目标

class CIVError : public CObject
{
public:
	CIVError();
	virtual ~CIVError();
	typedef enum {
		IVE_NONE,
		IVE_CONFIG,
		IVE_NOMEM,
		IVE_CANCELED,
		IVE_INTERNAL,
		IVE_SERIAL_CONN,
		IVE_SERIAL_WRITE,
		IVE_SERIAL_READ,
		IVE_SERIAL_PROTOCAL,
		IVE_SERIAL_DEVICE
	}IVE_TYPE_T;
	void SetError(IVE_TYPE_T eError) {}
	CString GetErrorStr() { return _T("NONE"); }
};


