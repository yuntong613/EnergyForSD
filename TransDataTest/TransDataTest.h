
// TransDataTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "Log.h"
#include "DataBaseMgr.h"
extern CDataBaseMgr g_DBMgr;
// CTransDataTestApp:
// �йش����ʵ�֣������ TransDataTest.cpp
//

class CTransDataTestApp : public CWinApp
{
public:
	CTransDataTestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
public:
	CCriticalSection m_LogSec;
	void WriteLog(CString strInfo);
	CLog m_Log;
};

extern CTransDataTestApp theApp;