#ifndef _DBDEFINE_
#define _DBDEFINE_

struct ElecHourResult{
	CString strItemCode;
	CString strDateTime;
	CString strSumValue;
	CString strSortCode;
	CString strAreaCode;
};
typedef CArray<ElecHourResult,ElecHourResult> ArrElecHourResult;

struct WaterHourResult{
	CString strSumValue;
	CString strDateTime;
};

typedef CArray<WaterHourResult,WaterHourResult> ArrWaterHourResult;


#define	 BoolType(b) b?true:false
#endif