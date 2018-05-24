#pragma once

#include "DBAdo.h"
#include "DBDefine.h"

class CDataBaseMgr
{
public:
	CDataBaseMgr(void);
	~CDataBaseMgr(void);
	bool Init(CString strUID,CString strPassWord,CString strDSN);
	void Release();
protected:
	CDBAdo m_DbAdo;
public:
	bool ExcuteSQL(CString strSql);

	bool GetElecHourResult(CString strStartTime,CString strEndTime,ArrElecHourResult& result);
	bool GetWaterHourResult(CString strNodeID,CString strStartTime,CString strEndTime,ArrWaterHourResult& result);
};

