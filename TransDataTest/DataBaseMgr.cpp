#include "StdAfx.h"
#include "DataBaseMgr.h"
#include "Utils.h"
#define  SQL_BUFFER_LEN 1024

CDataBaseMgr::CDataBaseMgr(void)
{
}


CDataBaseMgr::~CDataBaseMgr(void)
{
}

bool CDataBaseMgr::Init(const char* szConnect /*= NULL*/)
{
	CString strConnectStr = szConnect;
	bool bRet = false;
	if(!m_DbAdo.CreateInstance())
	{
		return false;
	}
	if(szConnect == NULL)
	{
		CUtils utl;
		CString strFilePath = utl.GetAppPath()+"db.ini";
		char connectStr[200] = {0};
		DWORD dwRet = GetPrivateProfileString("DBInfo","ConnectString", "", connectStr, 200, strFilePath);
		if (dwRet<=0)
		{
			return false;
		}
		strConnectStr = connectStr;
	}
	m_DbAdo.SetConnectionString(strConnectStr);
	if(!m_DbAdo.OpenConnection())
	{
		return false;
	}
	return true;
}

bool CDataBaseMgr::Init(CString strUID,CString strPassWord,CString strDSN)
{
	bool bRet = false;
	if(!m_DbAdo.CreateInstance())
	{
		return false;
	}
	m_DbAdo.SetConnectionString(strDSN,strUID,strPassWord);
	if(!m_DbAdo.OpenConnection())
	{
		return false;
	}
	return true;
}

void CDataBaseMgr::Release()
{
	m_DbAdo.CloseConnection();
}

bool CDataBaseMgr::ExcuteSQL(CString strSql)
{
	char szSQL[SQL_BUFFER_LEN] = {0};
	memcpy(szSQL,strSql.GetBuffer(strSql.GetLength()),strSql.GetLength());
	strSql.ReleaseBuffer();
	if(!m_DbAdo.IsConnecting())
	{
		m_DbAdo.OpenConnection();
		return false;
	}
	return m_DbAdo.Execute(szSQL);
}

bool CDataBaseMgr::GetElecHourResult(CString strStartTime,CString strEndTime,ArrElecHourResult& result)
{
	CString strTime = strStartTime;
	int year,month,day,hour,min,sec = 0;
	sscanf(strTime,"%d-%d-%d %d:%d:%d",&year,&month,&day,&hour,&min,&sec);
	CString strTable;
	strTable.Format("energy_history_data_hour_%04d%02d",year,month);
	CString strSql,strFromat;
	strFromat = "SELECT\n"
		"	result.DATE_TIME,\n"
		"	SUM(result.SUM_VALUE) AS ALL_VALUE,\n"
		"	result.SORT_CODE,\n"
		"	result.AREA_CODE\n"
		"FROM\n"
		"	(\n"
		"		SELECT\n"
		"			tHistory.ITEM_CODE,\n"
		"			tHistory.DATE_TIME,\n"
		"			SUM(tHistory. VALUE) AS SUM_VALUE,\n"
		"			tSort. CODE AS SORT_CODE,\n"
		"			tArea.AREA_CODE\n"
		"		FROM\n"
		"			%s tHistory,\n"
		"			energy_item tItem,\n"
		"			energy_item_sort tSort,\n"
		"			energy_build_area tArea\n"
		"		WHERE\n"
		"			tHistory.type = 14\n"
		"		AND tItem.item_type = 14\n"
		"		AND tHistory.item_code = tItem.item_code\n"
		"		AND tItem.sort_id = tSort.id\n"
		"		AND tItem.area_id = tArea.id\n"
		"		AND tHistory.date_time >= \'%s\' \n"
		"		AND tHistory.date_time < \'%s\' \n"
		"		GROUP BY\n"
		"			tHistory.item_code,\n"
		"			tSort. CODE,\n"
		"			tHistory.date_time\n"
		"	) result\n"
		"GROUP BY\n"
		"	result.DATE_TIME,\n"
		"	result.SORT_CODE,\n"
		"	result.AREA_CODE\n"
		"ORDER BY\n"
		"	result.AREA_CODE,\n"
		"	result.SORT_CODE,\n"
		"	result.DATE_TIME";

	strSql.Format(strFromat,strTable,strStartTime,strEndTime);
	char szSQL[SQL_BUFFER_LEN] = {0};
	memcpy(szSQL,strSql.GetBuffer(strSql.GetLength()),strSql.GetLength());
	strSql.ReleaseBuffer();
	bool bOpen = m_DbAdo.OpenRecordset(szSQL);

	if(!bOpen)
	{
		if(!m_DbAdo.IsConnecting())
		{
			m_DbAdo.OpenConnection();
			return false;
		}
		return false;
	}

	if(bOpen)
	{
		for (int i=0;i<m_DbAdo.GetRecordCount();i++)
		{
			ElecHourResult Items;
			m_DbAdo.GetFieldValue("DATE_TIME", Items.strDateTime);
			m_DbAdo.GetFieldValue("ALL_VALUE", Items.strSumValue);
			m_DbAdo.GetFieldValue("SORT_CODE", Items.strSortCode);
			m_DbAdo.GetFieldValue("AREA_CODE", Items.strAreaCode);
			result.Add(Items);
			m_DbAdo.MoveToNext() ;
		}
	}

	return true;
}

bool CDataBaseMgr::GetWaterHourResult(CString strNodeID,CString strStartTime,CString strEndTime,ArrWaterHourResult& result)
{
	CString strSql;
	strSql = _T("select sum(VALUE) AS SUM_VALUE,date_time from ed_water_energy where item_name like \'")+ strNodeID +_T("%\' and DATE_TIME >= \'") + strStartTime + _T("\'  and DATE_TIME < \'")+strEndTime +_T("\' group by date_time");


	char szSQL[SQL_BUFFER_LEN] = {0};
	memcpy(szSQL,strSql.GetBuffer(strSql.GetLength()),strSql.GetLength());
	strSql.ReleaseBuffer();

	bool bOpen = m_DbAdo.OpenRecordset(szSQL);
	if(!bOpen)
	{
		if(!m_DbAdo.IsConnecting())
		{
			m_DbAdo.OpenConnection();
			return false;
		}
		return false;
	}

	if(bOpen)
	{
		WaterHourResult Items;
		m_DbAdo.GetFieldValue("SUM_VALUE", Items.strSumValue);
		m_DbAdo.GetFieldValue("DATE_TIME", Items.strDateTime);
		result.Add(Items);
		m_DbAdo.MoveToNext() ;
	}
	return true;
}
