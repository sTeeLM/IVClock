// IVError.cpp: 实现文件
//

#include "pch.h"
#include "IVClock.h"
#include "IVError.h"


// CIVError

CIVError::CIVError()
{
}

CIVError::~CIVError()
{
}

void CIVError::SetError(IVE_TYPE_T eError)
{
	switch (eError) {
	case IVE_NONE:
		m_strError.LoadString(IDS_IVE_NONE);
		break;
	case IVE_CONFIG:
		m_strError.LoadString(IDS_IVE_CONFIG);
		break;
	case IVE_NOMEM:
		m_strError.LoadString(IDS_IVE_NOMEM);
		break;
	case IVE_CANCELED:
		m_strError.LoadString(IDS_IVE_CANCELED);
		break;
	case IVE_INTERNAL:
		m_strError.LoadString(IDS_IVE_INTERNAL);
		break;
	case IVE_SERIAL_CONN:
		m_strError.LoadString(IDS_IVE_SERIAL_CONN);
		break;
	case IVE_SERIAL_WRITE:
		m_strError.LoadString(IDS_IVE_SERIAL_WRITE);
		break;
	case IVE_SERIAL_READ:
		m_strError.LoadString(IDS_IVE_SERIAL_READ);
		break;
	case IVE_SERIAL_PROTOCAL:
		m_strError.LoadString(IDS_IVE_SERIAL_PROTOCAL);
		break;
	case IVE_SERIAL_DEVICE:
		m_strError.LoadString(IDS_IVE_SERIAL_DEVICE);
		break;
	default:
		m_strError.LoadString(IDS_IVE_UNKNOWN);
	}
}


// CIVError 成员函数
