#include "StdAfx.h"
#include "MyZip.h"
#include <direct.h> 
#include <vector> 
#include <xstring> 

CMyZip::CMyZip(void)
{
}


CMyZip::~CMyZip(void)
{
}

void CMyZip::InitPackZip(CString strZipFileName)
{
	hz = CreateZip((void*)(LPCTSTR)strZipFileName,0,ZIP_FILENAME);
	m_ZipFIleName = strZipFileName;
}

BOOL CMyZip::RealsePackZip()
{
	ZRESULT zr = CloseZip(hz);
	if(zr!=ZR_OK)
		return FALSE;
	return TRUE;
}

BOOL CMyZip::Zip_PackFile(CString strFilePath)
{
	CFileFind fileFind;
	BOOL bFind = fileFind.FindFile(strFilePath);
	ZRESULT zr = ZR_OK;
	if(bFind)
	{
		int nPos = strFilePath.ReverseFind('\\');
		CString strName = strFilePath.Right(strFilePath.GetLength()-nPos-1);
		zr = ZipAdd(hz,strName,(void*)(LPCTSTR)strFilePath,0,ZIP_FILENAME);
	}
	fileFind.Close();
	if(zr!=ZR_OK)
		return FALSE;
	return TRUE;
}

BOOL CMyZip::Zip_PackDirectory(CString strPath)
{
	CFileFind fileFind;
	BOOL bFind = fileFind.FindFile(strPath);
	fileFind.Close();
	if(!bFind)
	{
		return FALSE;
	}
	m_ZipPackDir = strPath;
	BrowseFile(m_ZipPackDir);
	return TRUE;
}

void CMyZip::BrowseFile(CString strRootPath) 
{ 
	CFileFind ff; 
	CString szDir = strRootPath; 

	if(szDir.Right(1) != "\\") 
		szDir += "\\"; 

	szDir += "*.*"; 

	BOOL res = ff.FindFile(szDir); 
	while(res) 
	{ 
		res = ff.FindNextFile(); 
		if(ff.IsDirectory() && !ff.IsDots()) 
		{ 
			//�����һ����Ŀ¼���õݹ��������һ���� 
			CString strChildPath = ff.GetFilePath(); 

			CString subPath; 
			CString strPathName = ff.GetFileName();
			GetRelativePath(strRootPath,strChildPath,subPath); 
			//���ļ���ӵ�ZIP�ļ� 
			ZipAdd(hz,subPath,0,0,ZIP_FOLDER); 
			BrowseFile(strChildPath); 
		} 
		else if(!ff.IsDirectory() && !ff.IsDots()) 
		{ 
			//��ʾ��ǰ���ʵ��ļ�(����·��) 
			CString strChildPath = ff.GetFilePath(); 
			CString subPath; 
			GetRelativePath(strRootPath,strChildPath,subPath); 
			//���ļ���ӵ�ZIP�ļ� 
			ZipAdd(hz,subPath,(void*)(LPCTSTR)strChildPath,0,ZIP_FILENAME); 
		} 
	} 

	//�ر� 
	ff.Close(); 
} 

void CMyZip::GetRelativePath(CString strRootPath,CString pChildPath,CString& pSubString) 
{ 
	int nPos = m_ZipPackDir.ReverseFind('\\');
	pSubString = pChildPath.Right(pChildPath.GetLength()-nPos-1);
} 

BOOL CMyZip::Zip_UnPackFiles(CString mZipFileFullPath, CString mUnPackPath) 
{ 
	//�������� 
	if ((mUnPackPath == L"") || (mZipFileFullPath == L"")) 
	{ 
		//·���쳣���� 
		return FALSE ; 
	} 

	CFileFind tFFind; 
	if (!tFFind.FindFile(mZipFileFullPath)) 
	{ 
		//ѹ��ʧ��(δ����ѹ���ļ�) 
		return FALSE; 
	} 

	//�����ѹ����·�������� ��ͼ������ 
	CString tZipFilePath = mUnPackPath; 
	if(!FolderExist(tZipFilePath)) 
	{ 
		//��ѹ���ŵ��ļ��в����� ������ 
		char* temp=tZipFilePath.GetBuffer(tZipFilePath.GetLength()); 
		if (FALSE == CreatedMultipleDirectory(temp)) 
		{ 
			//����Ŀ¼ʧ�� 
			return FALSE; 
		} 
	} 
	/************************************************************************/ 
	//��ZIP�ļ� 
	hz=OpenZip((void*)(LPCTSTR)mZipFileFullPath,0,ZIP_FILENAME); 
	if(hz == 0) 
	{ 
		//��Zip�ļ�ʧ�� 
		return FALSE; 
	} 
	ZIPENTRY ze;  //Zip�ļ���� 
	ZRESULT zr  =SetUnzipBaseDir(hz,mUnPackPath); 
	if(zr != ZR_OK) 
	{ 
		//��Zip�ļ�ʧ�� 
		CloseZip(hz); 
		return FALSE;       
	} 

	zr=GetZipItem(hz,-1,&ze); 
	if(zr != ZR_OK) 
	{ 
		//��ȡZip�ļ�����ʧ�� 
		CloseZip(hz); 
		return FALSE;       
	} 

	int numitems=ze.index; 
	for (int i=0; i<numitems; i++) 
	{ 
		zr=GetZipItem(hz,i,&ze); 
		zr=UnzipItem(hz,i,ze.name,1024*1024,ZIP_FILENAME); 

		if(zr != ZR_OK) 
		{ 
			//��ȡZip�ļ�����ʧ�� 
			CloseZip(hz); 
			return FALSE;       
		} 
	} 

	CloseZip(hz); 
	return TRUE; 
} 

BOOL CMyZip::FolderExist(CString& strPath) 
{ 
	CString sCheckPath = strPath; 

	if(sCheckPath.Right(1) != "\\") 
		sCheckPath += "\\"; 

	sCheckPath += "*.*"; 

	WIN32_FIND_DATA wfd; 
	BOOL rValue = FALSE; 

	HANDLE hFind = FindFirstFile(sCheckPath, &wfd); 

	if ((hFind!=INVALID_HANDLE_VALUE) && 
		(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) || (wfd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE)) 
	{ 
		//������ڲ��������ļ��� 
		rValue = TRUE; 
	} 

	FindClose(hFind); 
	return rValue; 
} 

BOOL CMyZip::CreatedMultipleDirectory(char* direct) 
{ 
	std::string Directoryname = direct; 

	if (Directoryname[Directoryname.length() - 1] !=  '\\') 
	{ 
		Directoryname.append(1, '\\'); 
	} 
	std::vector< std::string> vpath; 
	std::string strtemp; 
	BOOL  bSuccess = FALSE; 
	int nLen = Directoryname.length();
	for (int i = 0; i < nLen; i++) 
	{ 
		if ( Directoryname[i] != '\\') 
		{ 
			strtemp.append(1,Directoryname[i]);    
		} 
		else 
		{ 
			vpath.push_back(strtemp); 
			strtemp.append(1, '\\'); 
		} 
	} 
	std::vector<std::string>:: const_iterator vIter; 
	for (vIter = vpath.begin();vIter != vpath.end(); vIter++) 
	{ 
		bSuccess = CreateDirectory(vIter->c_str(), NULL) ? TRUE :FALSE; 
	} 

	return bSuccess; 
}