
// TransDataTest.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "Log.h"
#include "DataBaseMgr.h"
extern CDataBaseMgr g_DBMgr;
// CTransDataTestApp:
// 有关此类的实现，请参阅 TransDataTest.cpp
//

class CTransDataTestApp : public CWinApp
{
public:
	CTransDataTestApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
public:
	CCriticalSection m_LogSec;
	void WriteLog(CString strInfo);
	CLog m_Log;
};

extern CTransDataTestApp theApp;