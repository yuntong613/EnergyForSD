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
			//如果是一个子目录，用递归继续往深一层找 
			CString strChildPath = ff.GetFilePath(); 

			CString subPath; 
			CString strPathName = ff.GetFileName();
			GetRelativePath(strRootPath,strChildPath,subPath); 
			//将文件添加到ZIP文件 
			ZipAdd(hz,subPath,0,0,ZIP_FOLDER); 
			BrowseFile(strChildPath); 
		} 
		else if(!ff.IsDirectory() && !ff.IsDots()) 
		{ 
			//显示当前访问的文件(完整路径) 
			CString strChildPath = ff.GetFilePath(); 
			CString subPath; 
			GetRelativePath(strRootPath,strChildPath,subPath); 
			//将文件添加到ZIP文件 
			ZipAdd(hz,subPath,(void*)(LPCTSTR)strChildPath,0,ZIP_FILENAME); 
		} 
	} 

	//关闭 
	ff.Close(); 
} 

void CMyZip::GetRelativePath(CString strRootPath,CString pChildPath,CString& pSubString) 
{ 
	int nPos = m_ZipPackDir.ReverseFind('\\');
	pSubString = pChildPath.Right(pChildPath.GetLength()-nPos-1);
} 

BOOL CMyZip::Zip_UnPackFiles(CString mZipFileFullPath, CString mUnPackPath) 
{ 
	//参数错误 
	if ((mUnPackPath == L"") || (mZipFileFullPath == L"")) 
	{ 
		//路径异常返回 
		return FALSE ; 
	} 

	CFileFind tFFind; 
	if (!tFFind.FindFile(mZipFileFullPath)) 
	{ 
		//压缩失败(未发现压缩文件) 
		return FALSE; 
	} 

	//如果解压缩的路径不存在 试图创建它 
	CString tZipFilePath = mUnPackPath; 
	if(!FolderExist(tZipFilePath)) 
	{ 
		//解压后存放的文件夹不存在 创建它 
		char* temp=tZipFilePath.GetBuffer(tZipFilePath.GetLength()); 
		if (FALSE == CreatedMultipleDirectory(temp)) 
		{ 
			//创建目录失败 
			return FALSE; 
		} 
	} 
	/************************************************************************/ 
	//打开ZIP文件 
	hz=OpenZip((void*)(LPCTSTR)mZipFileFullPath,0,ZIP_FILENAME); 
	if(hz == 0) 
	{ 
		//打开Zip文件失败 
		return FALSE; 
	} 
	ZIPENTRY ze;  //Zip文件入口 
	ZRESULT zr  =SetUnzipBaseDir(hz,mUnPackPath); 
	if(zr != ZR_OK) 
	{ 
		//打开Zip文件失败 
		CloseZip(hz); 
		return FALSE;       
	} 

	zr=GetZipItem(hz,-1,&ze); 
	if(zr != ZR_OK) 
	{ 
		//获取Zip文件内容失败 
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
			//获取Zip文件内容失败 
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
		//如果存在并类型是文件夹 
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