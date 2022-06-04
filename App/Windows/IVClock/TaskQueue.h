#pragma once

#include <afxtempl.h>

#include "IVError.h"

class CTask : public CObject
{
public:
    typedef enum _IV_TASK_TYPE_T {
        IV_TASK_PING,
        IV_TASK_GET_PARAM,
        IV_TASK_SET_PARAM,
        IV_TASK_GET_ALARM,
        IV_TASK_SET_ALARM,
        IV_TASK_GET_TIME,
        IV_TASK_SET_TIME,
        IV_TASK_GET_BATTEMP
    }IV_TASK_TYPE_T;

    CTask(HWND hWnd, IV_TASK_TYPE_T eTaskType, UINT nMessage, LPVOID pParam)
        : m_hWnd(hWnd)
        , m_eTaskType(eTaskType)
        , m_nMessage(nMessage)
        , m_bRes(FALSE)
        , m_pParam(pParam)
    {}
    virtual ~CTask() {}

public:
    HWND m_hWnd;
    IV_TASK_TYPE_T m_eTaskType;
    UINT m_nMessage;
    CIVError m_Error;
    BOOL     m_bRes;
    LPVOID   m_pParam;
};


class CTaskQueue :
	public CTypedPtrList< CObList, CTask* >

{
public:
    // Go to the end of the line
    void AddToEnd(CTask* newTask)
    {
        AddTail(newTask);
    }        // End of the queue

// Get first element in line
    CTask * GetFromFront()
    {
        return IsEmpty() ? NULL : RemoveHead();
    }

    void Clear() 
    {
        CTask* pTask = NULL;
        do {
            pTask = GetFromFront();
            if (pTask) {
                delete pTask;
            }
        } while (pTask != NULL);
    }
};

