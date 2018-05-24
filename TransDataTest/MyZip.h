#pragma once
#include "XZip.h"
#include "XUnzip.h"

class CMyZip
{
public:
	CMyZip(void);
	~CMyZip(void);
	CString m_ZipPackDir;
	CString m_ZipFIleName;
	HZIP hz;
public:
	void InitPackZip(CString strZipFileName);
	BOOL RealsePackZip();
	BOOL Zip_PackDirectory(CString strPath);
	BOOL Zip_PackFile(CString strFilePath);
	void BrowseFile(CString strRootPath);
	void GetRelativePath(CString strRootPath,CString pChildPath,CString& pSubString);
public:
	BOOL Zip_UnPackFiles(CString mZipFileFullPath, CString mUnPackPath);
	static BOOL FolderExist(CString& strPath); 
	BOOL CreatedMultipleDirectory(char* direct); 
};

