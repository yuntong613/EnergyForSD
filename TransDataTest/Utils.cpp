#include "stdafx.h"
#include "Utils.h"
#include <memory>
#include <iostream>
#include <memory>

using std::auto_ptr;


CUtils::CUtils(void)
{
}


CUtils::~CUtils(void)
{
}

CString CUtils::GetAppPath() const
{
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString strPath(szPath);
	strPath = strPath.Left(strPath.ReverseFind(_T('\\'))+1);
	return strPath;
}

CString CUtils::HexToString(const BYTE *pBuffer,size_t iBytes)
{
	CString result;
	for (size_t i = 0; i < iBytes; i++)
	{
		BYTE c ;
		BYTE b = pBuffer[i] >> 4;
		if (9 >= b)
			c = b + '0';
		else
			c = (b - 10) + 'A';
		result += (TCHAR)c;
		b = pBuffer[i] & 0x0f;
		if (9 >= b)
			c = b + '0';
		else
			c = (b - 10) + 'A';
		result += (TCHAR)c;
	}
	return result;
}

BYTE CUtils::Hex2Bin(CString strHex)
{
	int iDec = 0;
	if(strHex.GetLength() == 2){
		char cCh = strHex[0];
		if((cCh >= '0') && (cCh <= '9'))iDec = cCh - '0';
		else if((cCh >= 'A') && (cCh <= 'F'))iDec = cCh - 'A' + 10;
		else if((cCh >= 'a') && (cCh <= 'f'))iDec = cCh - 'a' + 10;
		else return 0;
		iDec *= 16;
		cCh = strHex[1];
		if((cCh >= '0') && (cCh <= '9'))iDec += cCh - '0';
		else if((cCh >= 'A') && (cCh <= 'F'))iDec += cCh - 'A' + 10;
		else if((cCh >= 'a') && (cCh <= 'f'))iDec += cCh - 'a' + 10;
		else return 0;
	}
	return iDec & 0xff;
}

void CUtils::StringToHex(const CString &ts,BYTE *pBuffer,int& nBytes)
{
	CString strByte;
	for(int i=0;i<ts.GetLength();i+=2){
		strByte = ts.Mid(i,2);
		pBuffer[i/2] = Hex2Bin(strByte);
	}
	nBytes = ts.GetLength() / 2;
}

CString CUtils::ToHex(BYTE c)
{
	TCHAR hex[3];

	const int val = c;

	_stprintf(hex, _T("%02X"), val);

	return hex;
}

inline bool CUtils::BOOL_to_bool(const BOOL bResult)
{
	// Convert a make believe BOOL into a real bool.
	// Removes warning C4800...
	return (TRUE == bResult);
}

CString CUtils::GetCurDirectory()
{
	DWORD size = ::GetCurrentDirectory(0, 0);

	auto_ptr<TCHAR> spBuf(new TCHAR[size]);

	if (0 == ::GetCurrentDirectory(size, spBuf.get()))
	{
		return "";
	}
	return CString(spBuf.get());
}

CString CUtils::GetDateStamp(int nType)
{
	CTime tm = CTime::GetCurrentTime();
	CString s;
	switch(nType)
	{
	case 0:
		s = tm.Format("%Y-%m-%d %H:%M:%S");
		break;
	case 1:
		s = tm.Format("%Y%m%d%H%M%S");
		break;
	case 2:
		s = tm.Format("%Y%m%d");
		break;
	case 3:
		s = tm.Format("%Y-%m-%d");
		break;
	case 4:
		s = tm.Format("%H%M%S");
		break;
	case 5:
		s = tm.Format("%H:%M:%S");
		break;
	default:
		s = tm.Format("%Y-%m-%d %H:%M:%S");
	}
	return s;
}