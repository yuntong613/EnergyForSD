#include "StdAfx.h"
#include "common.h"
#include "ProtocolManager.h"
#include "IniFile.h"
#include "DechnicCrypt.h"
#include <WinCrypt.h>
#include "MyZip.h"
#include "xml.h"
#include "Utils.h"
#include "DataBaseMgr.h"
#include "TransDataTest.h"

CString CProtocolManager::GetAppPath()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString strPath(szPath);
	strPath = strPath.Left(strPath.ReverseFind(_T('\\'))+1);
	return strPath;
}

void CProtocolManager::ShowLog(CString strMsg)
{
	if(_hMainWnd)
	{
		::SendMessage(_hMainWnd,WM_SHOWLOG,(WPARAM)strMsg.GetLength(),(LPARAM)strMsg.GetBuffer());
		strMsg.ReleaseBuffer();
	}
}

CProtocolManager::CProtocolManager(void)
:m_bIsLogined(false)
, m_nTimerData(0)
, m_nHeartSendCount(0)
, m_bUpdateTime(FALSE)
{
	m_strSenderCertCode = "";
	m_strNodeID="";
	m_strSettingsPath="";
	m_strRemoteIP = "";
	m_nRemotePort = 0;
	_hMainWnd = NULL;
	ZeroMemory(m_cpCenterCert,MAX_DATA_FILE);
	m_nCenterCertLen = 0;
}

CProtocolManager::~CProtocolManager(void)
{
	DeleteEnergySumMap();
}

void CProtocolManager::CompseDatagram(BYTE* cpDataOut,int& dataOutLen,BYTE* body,int nbodyLen,CommandType cmdType)
{
	//	if(dataOut==NULL)return;
	BYTE header[] = { 0x8b, 0xae, 0x96, 0x68, 0x68, (BYTE)cmdType, 0 };
	BYTE end[] = { 0x16, 0x16, 0x16, 0x16 };
	memcpy(cpDataOut,header,sizeof(header));
	memcpy(cpDataOut+7,body,nbodyLen);
	memcpy(cpDataOut+7+nbodyLen,end,sizeof(end));
	dataOutLen = sizeof(header)+sizeof(end)+nbodyLen;
}

int CProtocolManager::SignMsg(BYTE* cpSendBuf, int nSendBufLen,CString signerCertThumbprint,BYTE* cpDataOut,int& cpDataOutLen)
{
	CString strMsg;
	strMsg.Format("%s",(char*)cpSendBuf);
	theApp.WriteLog("SignMsg:"+ strMsg);

	BYTE CertData[MAX_CERT_LEN] = {0};
	int nCertLen = 0;
	int nRet = getSignerCert(CertData,nCertLen,signerCertThumbprint);
	BYTE signedDataBuffer[MAX_DATA_FILE] = {0};
	int signedDataLen = 0;
	int result = DECHNIC_PKCS7Sign(CertData, nCertLen,cpSendBuf, nSendBufLen,signedDataBuffer, &signedDataLen);

	memcpy(cpDataOut,signedDataBuffer,signedDataLen);
	cpDataOutLen = signedDataLen;

	return result;
}

int CProtocolManager::getSignerCert(BYTE* cpCert,int& nCertLen,CString signerCertThumbprint)
{
	BYTE certBuffer [MAX_CERT_LEN];
	CUtils util;
	BYTE certTemp[100] = {0};
	int nLen = 0;
	util.StringToHex(signerCertThumbprint,certTemp,nLen);

	int nRet = DECHNIC_GetCertByPrm((BYTE*)certTemp,nLen, certBuffer,&nCertLen);
	signerCertThumbprint.ReleaseBuffer();
	memcpy(cpCert,certBuffer,nCertLen);

	return nRet;
}

int CProtocolManager::VerifyMsg(BYTE* encodedSignedCms, int nEncodedSignedCmsLen, BYTE* decodeData, int& ndecodeDataLen)
{
	BYTE tmpCenterCert[MAX_DATA_FILE] = {0};
	int nCenterCertLen = 0;
	int result = DECHNIC_PKCS7Verify(encodedSignedCms, nEncodedSignedCmsLen, decodeData,&ndecodeDataLen,tmpCenterCert,&nCenterCertLen,NULL,0,NULL,0);
	if(result==0)
	{
		ZeroMemory(m_cpCenterCert,MAX_DATA_FILE);
		memcpy(m_cpCenterCert,tmpCenterCert,nCenterCertLen);
		m_nCenterCertLen = nCenterCertLen;
	}
	return result;
}

int CProtocolManager::EncryptMsg(BYTE* senderCert,int nSenderCertLen, BYTE* recvCert, int nRecvCertLen, BYTE* Msg, int nMsgLen, BYTE* cpDataOut,int& nOutLen)
{
	int encodeDataBufLen = (nMsgLen + 0x1000) * 2;
	BYTE* EncryptData = new BYTE[encodeDataBufLen];
	ZeroMemory(EncryptData,encodeDataBufLen);

	int result = DECHNIC_EncodeEnvelop(senderCert,nSenderCertLen,recvCert,nRecvCertLen,Msg,nMsgLen,EncryptData,&encodeDataBufLen);

	memcpy(cpDataOut,EncryptData,encodeDataBufLen);
	nOutLen = encodeDataBufLen;
	delete[] EncryptData;
	return result;
}

int CProtocolManager::DecryptMsg(BYTE* encodedEnvelopedCms,int encodeLen, BYTE* cpDataOut,int& nOutLen)
{
	int decodeDataBufLen = encodeLen;
	BYTE* DecryptptData = new BYTE[decodeDataBufLen];
	ZeroMemory(DecryptptData,decodeDataBufLen);

	int result = DECHNIC_DecodeEnvelop(encodedEnvelopedCms,encodeLen,DecryptptData,&decodeDataBufLen);

	memcpy(cpDataOut,DecryptptData,decodeDataBufLen);
	nOutLen = decodeDataBufLen;
	delete[] DecryptptData;

	return result;
}

int CProtocolManager::GetLoginData(CString strNodeID,char* cpDataOut,int& nOutLen)
{
	CString   aStr;
	aStr = "<?xml version=\"1.0\" encoding=\"utf-8\" ?><root><common><node_id>"
		+ strNodeID + "</node_id><building_id>0</building_id><gateway_id>0</gateway_id>"
		+ "<type>request</type></common><id_validate operation=\"request\"></id_validate></root>";

	BYTE* cpData = new BYTE[MAX_DATA_FILE];
	ZeroMemory(cpData,MAX_DATA_FILE);
	int nDataLen = 0;
	int nRet = -1;
	nRet = SignMsg((BYTE*)aStr.GetBuffer(),aStr.GetLength(),m_strSenderCertCode,cpData,nDataLen);
	aStr.ReleaseBuffer();
	if(nRet==0)
	{
		CompseDatagram((BYTE*)cpDataOut,nOutLen,(BYTE*)cpData,nDataLen,CommandType::Login);
	}
	delete[] cpData;
	return nRet;
}

int CProtocolManager::GetHeartData(CString strNodeID,char* cpDataOut,int& nOutLen)
{
	CString  aStr;
	aStr = "<?xml version=\"1.0\" encoding=\"utf-8\" ?><root><common><node_id>"
		+ strNodeID + "</node_id><building_id></building_id><gateway_id>0</gateway_id>"
		+ "<type>notify</type></common><heat_beat operation = \"notify\">"
		+ "<time></time></heat_beat></root>";

	BYTE* cpData = new BYTE[MAX_DATA_FILE];
	ZeroMemory(cpData,MAX_DATA_FILE);
	int nDataLen = 0;
	int nRet = SignMsg((BYTE*)aStr.GetBuffer(),aStr.GetLength(),m_strSenderCertCode,cpData,nDataLen);
	aStr.ReleaseBuffer();
	if(nRet==0)
	{
		CompseDatagram((BYTE*)cpDataOut,nOutLen,(BYTE*)cpData,nDataLen,CommandType::Heartbeat);
	}
	delete[] cpData;
	return nRet;
}

int CProtocolManager::GetPacketStartData(CString strNodeID,CString strFileName,BYTE* cpDataOut,int& nOutLen)
{
	CFileFind fileFind;
	BOOL bFind = fileFind.FindFile(strFileName);
	int nRet = -1;
	BYTE* cpData = new BYTE[MAX_DATA_FILE];
	ZeroMemory(cpData,MAX_DATA_FILE);
	int nDataLen = 0;
	CString strTmpPathName;
	CString   aStr;
	CString strFileLen;
	ULONGLONG nFileLen = 0;
	if(bFind)
	{
		while(bFind)
		{
			bFind = fileFind.FindNextFile();
			strTmpPathName = fileFind.GetFilePath();
			if(strTmpPathName==strFileName)
			{
				nFileLen =  fileFind.GetLength();
				
				strFileLen.Format("%llu",nFileLen);
				aStr = "<?xml version=\"1.0\" encoding=\"utf-8\" ?><root><common><node_id>"
					+ strNodeID + "</node_id><building_id>0</building_id><gateway_id>0</gateway_id>"
					+ "<type>packet_start</type></common><packet operation=\"packet_start\">"
					+ "<filename>" + fileFind.GetFileName() + "</filename><filelength>" + strFileLen + "</filelength>"
					+ "</packet></root>";
				nRet = SignMsg((BYTE*)aStr.GetBuffer(),aStr.GetLength(),m_strSenderCertCode,cpData,nDataLen);
				aStr.ReleaseBuffer();
				if(nRet==0)
				{
					CompseDatagram((BYTE*)cpDataOut,nOutLen,(BYTE*)cpData,nDataLen,CommandType::UploadDataRequest);
					break;
				}
			}
		}
	}
	fileFind.Close();
	delete[] cpData;
	return nRet;
}

BOOL CProtocolManager::GetMd5(CString FileDirectory,CString &strFileMd5)  
{  
	HANDLE hFile=CreateFile(FileDirectory,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
	if (hFile==INVALID_HANDLE_VALUE)                                        //如果CreateFile调用失败  
	{   
		//提示CreateFile调用失败，并输出错误号。visual studio中可在“工具”>“错误查找”中利用错误号得到错误信息。  
		CloseHandle(hFile);
		return FALSE;
	}
	HCRYPTPROV hProv=NULL;
	if(CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)==FALSE)       //获得CSP中一个密钥容器的句柄
	{
		return FALSE;
	}
	HCRYPTPROV hHash=NULL;
	//初始化对数据流的hash，创建并返回一个与CSP的hash对象相关的句柄。这个句柄接下来将被    CryptHashData调用。
	if(CryptCreateHash(hProv,CALG_MD5,0,0,&hHash)==FALSE)
	{
		return FALSE;
	}
	DWORD dwFileSize=GetFileSize(hFile,0);    //获取文件的大小
	if (dwFileSize==0xFFFFFFFF)               //如果获取文件大小失败  
	{
		return FALSE;
	}
	byte* lpReadFileBuffer=new byte[dwFileSize];
	DWORD lpReadNumberOfBytes;
	if (ReadFile(hFile,lpReadFileBuffer,dwFileSize,&lpReadNumberOfBytes,NULL)==0)        //读取文件  
	{
		return FALSE;
	}
	if(CryptHashData(hHash,lpReadFileBuffer,lpReadNumberOfBytes,0)==FALSE)      //hash文件  
	{
		return FALSE;
	}
	delete[] lpReadFileBuffer;
	CloseHandle(hFile);          //关闭文件句柄
	BYTE *pbHash;
	DWORD dwHashLen=sizeof(DWORD);
	if (!CryptGetHashParam(hHash,HP_HASHVAL,NULL,&dwHashLen,0)) //我也不知道为什么要先这样调用CryptGetHashParam，这块是参照的msdn       
	{
		return FALSE;
	}
	pbHash=(byte*)malloc(dwHashLen);
	if(CryptGetHashParam(hHash,HP_HASHVAL,pbHash,&dwHashLen,0))//获得md5值 
	{
		for(DWORD i=0;i<dwHashLen;i++)         //输出md5值 
		{
			TCHAR str[3]={0};
			CString strFilePartM=_T("");
			_stprintf(str,_T("%02x"),pbHash[i]);
			strFileMd5+=str;
		}
	} 

	//善后工作
	if(CryptDestroyHash(hHash)==FALSE)          //销毁hash对象  
	{
		return FALSE;
	}
	if(CryptReleaseContext(hProv,0)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

int CProtocolManager::GetNHDataData(CString strZipFileName,CString strEncodeFileName,CString& strMD5File)
{
	int nRet = -1;
	if(PathFileExists(strZipFileName))
	{
#pragma region 文件加密
		char* fileChar = new char[MAX_DATA_FILE];
		ZeroMemory(fileChar,MAX_DATA_FILE);

		BYTE* senderCert = new BYTE[MAX_DATA_FILE];
		ZeroMemory(senderCert,MAX_DATA_FILE);
		int senderCertLen = 0;

		CFileStatus st = {0};
		CFile::GetStatus(strZipFileName,st);
		if(st.m_size>=MAX_DATA_FILE)
		{
			ShowLog("上传文件超出200K");
			return -1;
		}
		int nFileLen = ReadFileInfo(strZipFileName,fileChar,MAX_DATA_FILE);
		nRet = getSignerCert(senderCert,senderCertLen,m_strSenderCertCode);
		if(nRet!=0)return -1;

		if(nRet==0)//文件加密时，获取证书成功
		{
			char* SignData = new char[MAX_DATA_FILE*2];
			ZeroMemory(SignData,MAX_DATA_FILE*2);
			int SignDataLen = 0;

			nRet = EncryptMsg(senderCert,senderCertLen, m_cpCenterCert, m_nCenterCertLen,(BYTE*)fileChar,nFileLen,(BYTE*)SignData,SignDataLen);

			if(nRet==0)//文件加密成功
			{
				CString strTempName = strZipFileName.Left(strZipFileName.GetLength()-4)+"tempMD5.dat";
				WriteFileInfo(strTempName,SignData,SignDataLen);
				CString strMD5;
				if(GetMd5(strTempName,strMD5))
				{
					strMD5File = strMD5;
					char* cpDataOut = new char[MAX_DATA_FILE];
					ZeroMemory(cpDataOut,MAX_DATA_FILE);
					int dataOutLen = 0;
					CompseDatagram((BYTE*)cpDataOut,dataOutLen,(BYTE*)SignData,SignDataLen,CommandType::EnergyData);
					WriteFileInfo(strEncodeFileName,cpDataOut,dataOutLen);
					delete[] cpDataOut;
				}
				DeleteFile(strTempName);
			}
			delete[] SignData;
		}else{
			ShowLog(_T("文件加密时，获取证书失败"));
		}
		delete[] senderCert;
		delete[] fileChar;
#pragma endregion 文件加密
	}
	return nRet;
}

int CProtocolManager::GetPacketEndData(CString strNodeID,CString strFileMD5,BYTE* cpDataOut,int& nOutLen)
{
	CString   aStr;
	aStr = "<?xml version=\"1.0\" encoding=\"utf-8\" ?><root><common><node_id>"
		+ strNodeID + "</node_id><building_id>0</building_id><gateway_id>0</gateway_id>"
		+ "<type>packet_end</type></common><packet operation=\"packet_end\">"
		+ "<md5>" + strFileMD5 + "</md5></packet></root>";

	BYTE* cpData = new BYTE[MAX_DATA_FILE];
	ZeroMemory(cpData,MAX_DATA_FILE);
	int nDataLen = 0;
	int nRet = -1;
	nRet = SignMsg((BYTE*)aStr.GetBuffer(),aStr.GetLength(),m_strSenderCertCode,cpData,nDataLen);
	aStr.ReleaseBuffer();
	if(nRet==0)
	{
		CompseDatagram((BYTE*)cpDataOut,nOutLen,(BYTE*)cpData,nDataLen,CommandType::UploadFinishConfirm);
	}
	delete[] cpData;
	return nRet;
}

int CProtocolManager::GetPeriodACKData(CString strNodeID,CString strPeriod,BYTE* cpDataOut,int& nOutLen)
{
	int nRet = -1;
	CString aStr = "<?xml version=\"1.0\" encoding=\"utf-8\" ?><root><common><node_id>"
		+ strNodeID + "</node_id><building_id>0</building_id><gateway_id>0</gateway_id>"
		+ "<type>period_ack</type></common><config operation=\"period_ack\"><period>"
		+ strPeriod + "</period></config></root>";

	BYTE* cpData = new BYTE[MAX_DATA_FILE];
	ZeroMemory(cpData,MAX_DATA_FILE);
	int nDataLen = 0;
	nRet = SignMsg((BYTE*)aStr.GetBuffer(),aStr.GetLength(),m_strSenderCertCode,cpData,nDataLen);
	aStr.ReleaseBuffer();
	if(nRet==0)
	{
		CompseDatagram((BYTE*)cpDataOut,nOutLen,(BYTE*)cpData,nDataLen,CommandType::ConfigFinishConfirm);
	}
	delete[] cpData;
	return nRet;
}

int CProtocolManager::GetReplyStartData(CString strNodeID,CString strFileName,BYTE* cpDataOut,int& nOutLen)
{
	int nRet = -1;
	CFileFind fileFind;
	BOOL bFind = fileFind.FindFile(strFileName);
	BYTE* cpData = new BYTE[MAX_DATA_FILE];
	ZeroMemory(cpData,MAX_DATA_FILE);
	if(bFind)
	{
		CString strTmpPath;
		while(bFind)
		{
			ZeroMemory(cpData,MAX_DATA_FILE);
			bFind = fileFind.FindNextFile();
			strTmpPath = fileFind.GetFilePath();
			if(strTmpPath==strFileName)
			{
				ULONGLONG nFileLen = fileFind.GetLength();
				CString strFileLen;
				strFileLen.Format("%llu",nFileLen);
				CString   aStr = "<?xml version=\"1.0\" encoding=\"utf-8\" ?><root><common><node_id>"
					+ strNodeID + "</node_id><building_id>0</building_id><gateway_id>0</gateway_id>"
					+ "<type>reply_start</type></common><query operation=\"reply_start\">"
					+ "<filename>" + fileFind.GetFileName() + "</filename><filelength>"
					+ strFileLen + "</filelength></query></root>";

				int nDataLen = 0;
				nRet = SignMsg((BYTE*)aStr.GetBuffer(),aStr.GetLength(),m_strSenderCertCode,cpData,nDataLen);
				aStr.ReleaseBuffer();
				if(nRet==0)
				{
					CompseDatagram((BYTE*)cpDataOut,nOutLen,(BYTE*)cpData,nDataLen,CommandType::UploadQueryDataRequest);
					break;
				}
			}
		}
	}
	fileFind.Close();
	delete[] cpData;
	return nRet;
}

int CProtocolManager::GetReplyEndData(CString strNodeID,CString strFileMD5,BYTE* cpDataOut,int& nOutLen)
{
	int nRet = -1;
	CString   aStr = "<?xml version=\"1.0\" encoding=\"utf-8\" ?><root><common><node_id>"
		+ strNodeID + "</node_id><building_id>0</building_id><gateway_id>0</gateway_id>"
		+ "<type>reply_end</type></common><query operation=\"reply_end\"><md5>"
		+ strFileMD5 + "</md5></query></root>";
	BYTE* cpData = new BYTE[MAX_DATA_FILE];
	ZeroMemory(cpData,MAX_DATA_FILE);
	int nDataLen = 0;
	nRet = SignMsg((BYTE*)aStr.GetBuffer(),aStr.GetLength(),m_strSenderCertCode,cpData,nDataLen);
	aStr.ReleaseBuffer();
	if(nRet==0)
	{
		CompseDatagram((BYTE*)cpDataOut,nOutLen,(BYTE*)cpData,nDataLen,CommandType::UploadQueryDataFinishConfirm);
	}
	delete[] cpData;
	return nRet;
}

int CProtocolManager::ReadRecvData(CString strNodeID,BYTE* cpData,int nLen)
{
	int nRet = -1;
	CString strXML = CString(cpData);
	theApp.WriteLog("Recv:"+strXML);

	strXML+="\r\n";
	CXML m_xml;
	int nParsed = m_xml.ParseXmlStr(strXML);
	if(nParsed)
	{
		TiXmlElement* pRoot =  m_xml.getRootElement();
		TiXmlElement* pTmp = NULL;
		pTmp = m_xml.getElement(pRoot,_T("common"));
		if(pTmp)
		{
			CString strType,strXMLData;
			m_xml.getElementValue(pTmp,_T("type"),strType);
			if(strType==_T("result"))
			{
				m_xml.getFirstElementValue(strType,strXMLData);
				RevResult(strXMLData);
			}else if(strType==_T("time"))
			{
				m_xml.getFirstElementValue(strType,strXMLData);
				RecvTime(strXMLData);
			}
			else if(strType==_T("period"))
			{
				m_xml.getFirstElementValue(strType,strXMLData);
				RevPeriod(strXMLData);
			}
			else if(strType==_T("packet_start_ack"))
			{
				CString strFileName,strFileLen;
				m_xml.getFirstElementValue(_T("filename"),strFileName);
				m_xml.getFirstElementValue(_T("filelength"),strFileLen);	
				RecvPacketStartAck(strFileName,strFileLen);
			}
			else if(strType==_T("packet_end_ack"))
			{
				RecvPacketEndAck(strType);
			}
			else if(strType==_T("query"))
			{
				CString strStartTime,strEndTime;
				m_xml.getFirstElementValue(_T("starttime"),strStartTime);
				m_xml.getFirstElementValue(_T("endtime"),strEndTime);	
				RevQuery(strStartTime,strEndTime);
			}
			else if (strType==_T("reply_start_ack"))
			{
				RevReplyStartAck(strType);
			}
			else if (strType==_T("reply_end_ack"))
			{
				RevReplyEndAck(strType);
			}
		}
	}
	m_xml.Clear();
	return nRet;
}

void CProtocolManager::RevResult(CString strXMLData)
{
	CString strLog;
	if(strXMLData==_T("pass"))
	{
		m_bIsLogined = true;
		strLog = _T("服务器登录成功");
		ShowLog(strLog);
		CIniFile iniFile(m_strSettingsPath);
		int nHour = iniFile.GetInt(_T("Period"),_T("Hour"),1);
		SetTimerInterval(nHour*60*60*1000);
	}else{
		strLog = _T("服务器登录请求被拒绝，拒绝原因：")+ strXMLData + _T("！");
		m_bIsLogined = false;
	}
}

void CProtocolManager::RevPeriod(CString strXMLData)
{
	CString strLog;
	int nHour = atoi(strXMLData.Left(strXMLData.GetLength()-1));
	SetTimerInterval(nHour*60*60*1000);
	CString strPeriod;
	strPeriod.Format("%d",nHour);
	strLog = _T("收到period，上传间隔设置为：") + strPeriod + _T("小时") ;
	strLog = _T("正在period_ack......");
	CIniFile iniFile(m_strSettingsPath);
	iniFile.WriteInt(_T("Setting"),_T("Period"),nHour);
	BYTE* cpData = new BYTE[MAX_DATA_FILE];
	ZeroMemory(cpData,MAX_DATA_FILE);
	int nLen = 0;
	int nRet = GetPeriodACKData(m_strNodeID,strPeriod,cpData,nLen);
	if(nRet!=-1)
	{
		if(!SendData((char*)cpData,nLen))
			ShowLog(_T("发送上传周期确认失败"));
	}
	delete[] cpData;
}

void CProtocolManager::RecvTime(CString strXMLData)
{
	CString strTimeSet = strXMLData;
	int nStrLen = strTimeSet.GetLength();
	ShowLog(_T("收到心跳校时信息:")+strTimeSet);

// 	if(nStrLen!=16){
// 		ShowLog(_T("收到心跳校时信息错误"));
// 		return;
// 	}
	int nYear = 0,nMonth = 0,nDay = 0,nHour = 0,nMinute = 0,nSecond = 0;

	int nFormat = sscanf(strTimeSet,"%d/%d/%d %d:%d:%d",&nYear,&nMonth,&nDay,&nHour,&nMinute,&nSecond);
	if(nFormat!=6){
		ShowLog(_T("收到心跳校时信息错误"));
		return;
	}
	if(m_bUpdateTime)
	{
		CTime tm(nYear,nMonth,nDay,nHour,nMinute,nSecond);
		SYSTEMTIME st;
		tm.GetAsSystemTime(st);
		BOOL bSet = SetLocalTime(&st);
	}
	m_nHeartSendCount = 0;
}

void CProtocolManager::RevQuery(CString strStartTime,CString strEndTime)
{
	ShowLog(_T("收到Query，起始时间：" )+ strStartTime + _T("结束时间：") + strEndTime);
	BeginUpload(strStartTime, strEndTime,TRUE);
}

void CProtocolManager::SendPacketEnd(CString strFileMD5, BOOL Query)
{
	BYTE* pData = new BYTE[MAX_DATA_FILE];
	ZeroMemory(pData,MAX_DATA_FILE);
	int DataLen = 0;
	if(Query)
		ShowLog(_T("正在发送reply_end......"));
	else
		ShowLog(_T("正在发送packet_end......"));

	int nRet = -1;
	if(Query)
		nRet= GetReplyEndData(m_strNodeID,strFileMD5,pData, DataLen);
	else
		nRet = GetPacketEndData(m_strNodeID,strFileMD5,pData, DataLen);
	if(nRet!=0)
	{
		CString strError;
		strError.Format("%d",nRet);
		if(Query)
			ShowLog("reply_end组包失败，原因：" + strError + "！");
		else
			ShowLog("packet_end组包失败，原因：" + strError + "！");
	}else{
		if (!SendData((char*)pData,DataLen))
		{
			if(Query)
				ShowLog(_T("reply_end发送失败！"));
			else
				ShowLog(_T("packet_end发送失败！"));
		}else{
			if(Query)
				ShowLog(_T("reply_end已发送，等待服务器回应"));
			else
				ShowLog(_T("packet_end已发送，等待服务器回应"));
		}
	}
	delete[] pData;
}

void CProtocolManager::RecvPacketStartAck(CString strFileName,CString strFileLen)
{
	BOOL Query = FALSE;
// 	CFileFind fileFind;
// 	BOOL bFind =  fileFind.FindFile(GetAppPath()+"upload\\"+strFileName);
// 	fileFind.Close();

	if(m_FNewQueryUploadEncodeFile.IsEmpty()){
		ShowLog(_T("收到reply_start_ack，但是本地没有待上传的文件！"));
		return;
	}

// 	if(!bFind || (m_FNewQueryUploadZipFile!=strFileName))
// 	{
// 		ShowLog(_T("收到packet_start_ack，但是本地没有待上传的文件！"));
// 		return;
// 	}
	if(m_FNewQueryUploadFileMD5.IsEmpty())
	{
		ShowLog(_T("收到packet_start_ack，但是MD5数据为空！"));
		return;
	}

	CFile file;
	BOOL bOpen =  file.Open(m_FNewQueryUploadEncodeFile,CFile::modeRead);
	if(!bOpen)
	{
		ShowLog(_T("读取上传文件 ")+ m_FNewQueryUploadEncodeFile +_T(" 失败！"));
		return;
	}
	char* pData = new char[MAX_DATA_FILE];
	ZeroMemory(pData,MAX_DATA_FILE);
	UINT nReadCount = file.Read(pData,MAX_DATA_FILE);
	ShowLog(_T("开始上传文件......."));
	file.Close();
	if(!SendData(pData,nReadCount))
	{
		ShowLog(_T("文件上传失败！"));
		CString strReloadTask = GetAppPath()+"Reload\\Task.ini";
		CIniFile iniFile(strReloadTask);
		int nCount = iniFile.GetInt("ReloadTask","Count",0);
		nCount++;
		iniFile.WriteInt("ReloadTask","Count",nCount);
		CString strSec;
		strSec.Format("T%d",nCount);
		iniFile.WriteString(strSec,"TimeSpan",m_strCurTimeSpan);
	}
	else
	{
		ShowLog(_T("文件上传成功"));
		SendPacketEnd(m_FNewQueryUploadFileMD5, Query);
	}
	DeleteDirectory(GetAppPath()+_T("upload"));
	delete[] pData;
}

void CProtocolManager::RecvPacketEndAck(CString strType)
{
	ShowLog(_T("收到packet_end_ack，文件发送成功"));
}

void CProtocolManager::RevReplyStartAck(CString strType)
{

	if(m_FNewQueryUploadEncodeFile.IsEmpty()){
		ShowLog(_T("收到reply_start_ack，但是本地没有待上传的文件！"));
		return;
	}

	if(!PathFileExists(m_FNewQueryUploadEncodeFile))
	{
		ShowLog(_T("收到reply_start_ack，但是本地没有待上传的文件！"));
	}
	if(m_FNewQueryUploadFileMD5.IsEmpty())
	{
		ShowLog(_T("收到reply_start_ack，但是MD5数据为空！"));
	}

	char* pData = new char[MAX_DATA_FILE];
	ZeroMemory(pData,MAX_DATA_FILE);

	UINT nReadCount =ReadFileInfo(m_FNewQueryUploadEncodeFile,pData,MAX_DATA_FILE);
	if(nReadCount==-1){
		ShowLog(_T("读取上传文件 ")+ m_FNewQueryUploadEncodeFile +_T(" 失败！"));
	}else{
		ShowLog(_T("开始上传文件......."));

		if(!SendData(pData,nReadCount))
		{
			ShowLog(_T("文件上传失败！"));
			//准备处理备份复传
			CString strReloadTask = GetAppPath()+"Reload\\Task.ini";
			CIniFile iniFile(strReloadTask);
			int nCount = iniFile.GetInt("ReloadTask","Count",0);
			nCount++;
			iniFile.WriteInt("ReloadTask","Count",nCount);
			CString strSec;
			strSec.Format("T%d",nCount);
			iniFile.WriteString(strSec,"TimeSpan",m_strCurTimeSpan);
		}
		else
		{
			ShowLog(_T("文件上传成功"));
			SendPacketEnd(m_FNewQueryUploadFileMD5, TRUE);
		}
		DeleteDirectory(GetAppPath()+_T("upload"));
	}
	delete[] pData;
}

void CProtocolManager::RevReplyEndAck(CString strType)
{
	ShowLog(_T("收到reply_end_ack，文件发送成功"));
}

BOOL CProtocolManager::SendData(char* cpData,int nLen)
{
	return m_ClientSock.WriteData(cpData,nLen);
}

bool CProtocolManager::SocketIsConnected()
{
	return m_ClientSock.IsConnected();
}

void CProtocolManager::KillSocket()
{
	m_ClientSock.Close();
	m_ClientSock.m_bIsConnected = false;
	m_bIsLogined = false;
}

BOOL CProtocolManager::ConnectServer()
{
	ShowLog(_T("正在连接远程服务器......"));
	if(m_ClientSock.m_hSocket!=INVALID_SOCKET)
		m_ClientSock.Close();
	m_ClientSock.Create();

	m_ClientSock.Connect(m_strRemoteIP,m_nRemotePort);

	return TRUE;
}

CString CProtocolManager::GetFirstMsg()
{
	CString strHead = m_ClientSock.GetMessageInfo();
	return strHead;
}

BOOL CProtocolManager::DoLogin()
{
	m_ClientSock.m_bIsConnected = true;
	ShowLog(_T("连接远程服务器成功"));
	ShowLog(_T("正在向服务器发送request......"));
	char* data = new char[MAX_DATA_FILE];
	ZeroMemory(data,MAX_DATA_FILE);
	int dataLen = 0;
	BOOL bSuccessed = FALSE;
	int nRet = GetLoginData(m_strNodeID,data,dataLen);
	if(nRet==-1){
		CString strLog;
		strLog.Format(_T("request组包失败，原因：%d"),nRet);
		ShowLog(strLog);
		bSuccessed = FALSE;
	}else{
		if(!SendData(data,dataLen))
		{
			ShowLog(_T("request发送失败！"));
			bSuccessed = FALSE;
		}else
			ShowLog(_T("request已发送，等待服务器回应"));
		bSuccessed = TRUE;
	}
	delete[] data;
	return bSuccessed;
}

#define  CESHI 0

BOOL CProtocolManager::MakeUploadFile(CString strStartTime,CString strEndTime)
{
 	CTime tm = CTime::GetCurrentTime();
	if(strStartTime=="")
	{
		CTime tmStart = CTime(tm.GetYear(),tm.GetMonth(),tm.GetDay(),tm.GetHour(),0,0);
		CTime tmEnd = tmStart + CTimeSpan(0,1,0,0);
		
// 		CIniFile iniFile(m_strSettingsPath);
// 		int Interval = iniFile.GetInt("Period","Hour",1);

		CTimeSpan timSpan(0,3,0,0);
		tmStart = tmStart - timSpan;
		tmEnd = tmEnd - timSpan;

		strStartTime = tmStart.Format("%Y-%m-%d %H:%M:%S");
		strEndTime = tmEnd.Format("%Y-%m-%d %H:%M:%S");
	}
	m_strCurTimeSpan = strStartTime+"|"+strEndTime;
 	CString strTempID;
	ShowLog(_T("时间范围：") + strStartTime + _T("--") + strEndTime);

	CString strReloadPath = GetAppPath()+_T("Reload");

  	CString strUpLoadPath = GetAppPath()+_T("upload");

	CString strCachePath = GetAppPath()+_T("Cache");

	BOOL bFind = PathFileExists(strUpLoadPath);
	if(bFind)
		DeleteDirectory(strUpLoadPath);
	else
		CreateDirectory(strUpLoadPath,NULL);

	bFind = PathFileExists(strCachePath);
	if(!bFind)
		CreateDirectory(strCachePath,NULL);

	CString strCacheDir = strCachePath+"\\";

	CString strTimeNow = tm.Format("%Y%m%d");
 	strTempID = m_strNodeID.Left(6)+strTimeNow;
 	CString strZipDir = strUpLoadPath+"\\";
	//建筑信息
	CString SourceBuildFile = GetAppPath() + "xml\\Build.xml";

	if(!PathFileExists(SourceBuildFile))
	{
		ShowLog(_T("没找到Build.xml，文件打包失败！"));
		return FALSE;
	}

	CString BuildFile = strZipDir + strTempID + "Build.xml";
	if (PathFileExists(BuildFile))
		DeleteFile(BuildFile);

	BOOL bSetBuild= SetBuildXML(SourceBuildFile, BuildFile, m_strNodeID ,strStartTime, strEndTime);

	//能耗信息
	CString SourceEnergyFile = GetAppPath() + "xml\\Energy.xml";

	if(!PathFileExists(SourceEnergyFile))
	{
		ShowLog(_T("没找到Energy.xml，文件打包失败！"));
		return FALSE;
	}

	CString EnergytFile = strZipDir + strTempID + "Energy.xml";

	if (PathFileExists(EnergytFile))
		DeleteFile(EnergytFile);

	BOOL bSetEnergy = SetEnergyXML(SourceEnergyFile,EnergytFile,m_strNodeID,strStartTime,strEndTime);

	m_FNewQueryUploadZipFile = strZipDir + strTempID + ".zip";

 #pragma region 打包文件
 	CMyZip mZip;
 	BOOL bZip = FALSE;
 	mZip.InitPackZip(m_FNewQueryUploadZipFile);
 	CString strAccessoryPath =  GetAppPath() + "xml\\Accessory";
	ShowLog(_T("打包附件信息"));
 	bZip = mZip.Zip_PackDirectory(strAccessoryPath);
 	if(!bZip)
 	{	
 		ShowLog(_T("文件压缩失败！"));
 		mZip.RealsePackZip();
 		return FALSE;
 	}
 	if(bSetBuild)
 	{
		ShowLog(_T("打包建筑信息"));
 		bZip = mZip.Zip_PackFile(BuildFile);
 		if(!bZip)
 		{
 			ShowLog(_T("文件压缩失败！"));
 			mZip.RealsePackZip();
 			return FALSE;
 		}
 	}
 	if(bSetEnergy)
 	{
		ShowLog(_T("打包能耗信息"));
 		bZip = mZip.Zip_PackFile(EnergytFile);
 		if(!bZip)
 		{
 			ShowLog(_T("文件压缩失败！"));
 			mZip.RealsePackZip();
 			return FALSE;
 		}
 	}
 	bZip = mZip.RealsePackZip();
 	if(!bZip)
 	{
 		ShowLog(_T("文件压缩失败！"));
 		return FALSE;
 	}


	//复制文件到历史缓存
/*	DeleteCathe(strCacheDir);*/
	CString strLastFileName;
	CString strFlag = strStartTime;
	strFlag.Remove('-');
	strFlag.Remove(':');
	strFlag.Remove(' ');
	strLastFileName.Format("%s",strFlag);

	CString strLastFile = strCacheDir+strLastFileName+".zip";

//	CString strLastFile = strCacheDir+"Last.zip";
//	DeleteFile(strLastFile);

	CopyFile(m_FNewQueryUploadZipFile,strLastFile,TRUE);

#if CESHI
	m_FNewQueryUploadZipFile = "D:\\Work\\Work\\数据上传服务\\TransDataTest\\Debug\\ceshi\\37000020171115.zip";
#endif
	
 #pragma endregion 打包文件
	ShowLog(_T("开始对文件进行加密......"));
	m_FNewQueryUploadEncodeFile = strZipDir + strTempID + ".tmp";

	m_FNewQueryUploadFileMD5 = "";

	int nRet = GetNHDataData(m_FNewQueryUploadZipFile, m_FNewQueryUploadEncodeFile, m_FNewQueryUploadFileMD5);

	if(nRet!=0)
	{
		CString strRetError;
		strRetError.Format("%d",nRet);
		ShowLog(_T("文件加密失败，原因：") + strRetError + _T("！"));

		m_FNewQueryUploadEncodeFile = "";
		m_FNewQueryUploadFileMD5 = "";
		m_FNewQueryUploadZipFile ="";
		return FALSE;
	}
	return TRUE;
}

void CProtocolManager::DeleteCathe(CString strDir)
{
	CFileFind ff; 
	CString strDeleteFile;
	strDir += "*.* "; 
	int ncount =0; 
	BOOL res = ff.FindFile(strDir); 
	CTime lastWriteTime;
	CTime tmDelete = CTime::GetCurrentTime();
	CTime ctMinTime = tmDelete - CTimeSpan(3,0,0,0);
	while(res) 
	{ 
		if(ff.GetLastWriteTime(lastWriteTime))
		{
			if(lastWriteTime<ctMinTime)
			{
				DeleteFile(ff.GetFilePath());
			}
		}
		res = ff.FindNextFile(); 
	}
}

void CProtocolManager::BeginUpload(CString StartTime,CString EndTime,BOOL Query)
{
	m_FNewQueryUploadEncodeFile = "";
	m_FNewQueryUploadFileMD5 = "";
	m_FNewQueryUploadZipFile = "";
	ShowLog(_T("准备上传数据，正在打包文件......"));
	BOOL bRet = MakeUploadFile(StartTime,EndTime);

	BOOL bFind = PathFileExists(m_FNewQueryUploadEncodeFile);
	
	if(!bRet || !bFind)
	{
		ShowLog(_T("文件打包失败！"));
		return;
	}
	if(Query)
		ShowLog(_T("向服务器发送reply_start......"));
	else
		ShowLog(_T("向服务器发送packet_start......"));

	BYTE* pData = new BYTE[MAX_DATA_FILE];
	ZeroMemory(pData,MAX_DATA_FILE);
	int DataLen = 0;
	int nRet = -1;
	if(Query)
		nRet = GetReplyStartData(m_strNodeID, m_FNewQueryUploadZipFile ,pData, DataLen);
	else
		nRet = GetPacketStartData(m_strNodeID, m_FNewQueryUploadZipFile ,pData, DataLen);
	if(nRet!=0)
	{
		CString strError;
		strError.Format("%d",nRet);
		if(Query)
			ShowLog("reply_start组包失败，原因：" + strError + "！");
		else
			ShowLog("packet_start组包失败，原因：" + strError + "！");
	}else{
		if (!SendData((char*)pData,DataLen))
		{
			if(Query)
				ShowLog(_T("reply_start发送失败！"));
			else
				ShowLog(_T("packet_start发送失败！"));

		}else{
			if(Query)
				ShowLog(_T("reply_start已发送，等待服务器回应"));
			else
				ShowLog(_T("packet_start已发送，等待服务器回应"));
		}
	}
	delete[] pData;
}

void CProtocolManager::BeginUploadBackUp()
{
	CString strReloadTask = GetAppPath()+"Reload\\Task.ini";
	CIniFile iniFile(strReloadTask);
	int nCount = iniFile.GetInt("ReloadTask","Count",0);
	if(nCount==0)return;
	CString strSec;
	strSec.Format("T%d",nCount);
	CString strCurTimeSpan = iniFile.GetString(strSec,"TimeSpan");
	iniFile.DeleteSection(strSec);
	nCount--;
	if(nCount<0)nCount=0;
	iniFile.WriteInt("ReloadTask","Count",nCount);
	if (strCurTimeSpan.IsEmpty())
	{
		return;
	}
	int nPos = 0;
	CString strStartTime = strCurTimeSpan.Tokenize("|",nPos);
	CString strEndTime = strCurTimeSpan.Tokenize("|",nPos);
	BeginUpload(strStartTime,strEndTime);
}

void CProtocolManager::DeleteDirectory(CString sDirName)
{
	char sPath[200];
	strcpy(sPath, sDirName);

	CFileFind   ff;
	BOOL   bFound;
	char sTempFileFind[200];
	strcpy(sTempFileFind, sPath);
	strcat(sTempFileFind, _T("\\*.*"));

	bFound   =   ff.FindFile(sTempFileFind);

	while(bFound)
	{
		bFound   =   ff.FindNextFile();
		CString  sFilePath   =   ff.GetFilePath();

		if(ff.IsDirectory())
		{
			if(!ff.IsDots())
				DeleteDirectory((LPSTR)(LPCTSTR)sFilePath);
		}
		else
		{
			if(ff.IsReadOnly())
			{
				SetFileAttributes(sFilePath,   FILE_ATTRIBUTE_NORMAL);
			}
			DeleteFile(sFilePath);
		}
	}
	ff.Close();
	SetFileAttributes(sPath,   FILE_ATTRIBUTE_NORMAL);
	if (!strcmp(sPath,sDirName))
	{
		return;
	}
	RemoveDirectory(sPath);
}

BOOL CProtocolManager::SetBuildXML(CString SourceFile, CString DestFile, CString NodeID ,CString StartTime, CString EndTime)
{
	CXML xml;
	int nParse = xml.ParseXmlFile(SourceFile);

	if(nParse)
	{
		xml.saveFile(DestFile);
		return TRUE;
	}	
	return FALSE;
}

BOOL CProtocolManager::SetEnergyXML(CString SourceFile,CString DestFile, CString NodeID ,CString StartTime, CString EndTime)
{
	CXML m_Xml;
	CTime tm = CTime::GetCurrentTime();
	CString strTimeNow = tm.Format("%Y-%m-%d %H:%M:%S");
	CString strTemp;
	int nRet = m_Xml.ParseXmlFile(SourceFile);
	if(!nRet){m_Xml.Clear();return FALSE;}

	TiXmlElement* pRoot = m_Xml.getRootElement();
	if(!pRoot){m_Xml.Clear(); return FALSE; }

	TiXmlElement* pCommon = m_Xml.getElement(pRoot,"common");
	if(!pCommon){ m_Xml.Clear(); return FALSE; }

	TiXmlElement* pTemp = NULL;
// 	pTemp = m_Xml.getElement(pCommon,"UploadDataCenterID");
// 	if(!pTemp){	m_Xml.Clear(); return FALSE; }

// 	pTemp = m_Xml.getElement(pCommon,"CreateTime");
// 	pTemp->FirstChild()->SetValue(strTimeNow);

	TiXmlElement* pData = m_Xml.getElement(pRoot,"data");
	
#pragma region 多个建筑

	ArrElecHourResult arrElecItems;
	bool bGetRecord = g_DBMgr.GetElecHourResult(StartTime,EndTime,arrElecItems);

	if(bGetRecord)
	{
		ShowLog("查询能耗数据成功");
		CString strStartHour,strEndHour;
		ElecHourResult ElecItem;
		CString strIndex,strSortCode,strSumValue,strDate,strBuildID,strAreaCode;
		CString strXmlBuildID;
		int nYear=0,nMonth=0,nDay=0,nHour=0,nMinute=0,nSecond=0;
		TiXmlElement* pBuild = NULL;
		TiXmlElement* pElecHour = NULL;
		int nResultID = 1;

		strStartHour = StartTime;

		sscanf(StartTime,"%04d-%02d-%02d %02d:%02d:%02d",&nYear,&nMonth,&nDay,&nHour,&nMinute,&nSecond);

		if(nHour==23)
			strTemp.Format("%04d-%02d-%02d %02d:%02d:%02d",nYear,nMonth,nDay+1,0,0,0);
		else
			strTemp.Format("%04d-%02d-%02d %02d:%02d:%02d",nYear,nMonth,nDay,nHour+1,0,0);

		strEndHour = strTemp;
		DeleteEnergySumMap();

		bool bHasSum01000 = false;

		for (int i=0;i<arrElecItems.GetCount();i++)
		{
			ElecItem = arrElecItems.GetAt(i);
			strBuildID = ElecItem.strItemCode;
			strAreaCode = ElecItem.strAreaCode;
			
			if(strAreaCode.IsEmpty())
				continue;
			else
				strXmlBuildID = strAreaCode;

			pBuild = m_Xml.getElementByMarkAttribute(pData,"Build","id",strXmlBuildID);
			if(pBuild==NULL)
			{
				pBuild = m_Xml.addXmlChildElement(pData,"Build");
				pBuild->SetAttribute("id",strXmlBuildID);
			}

			strSortCode = ElecItem.strSortCode;
			strSumValue = ElecItem.strSumValue;
			strDate = ElecItem.strDateTime;
			
			if(strSortCode == "01000")
			{
				bHasSum01000 = true;
			}
			else{
				AddSortItemToMap(strXmlBuildID,strSortCode,strSumValue);
			}

			pElecHour = m_Xml.addXmlChildElement(pBuild,"EnergyItemHourResult");
			pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourResultID");
			strIndex.Format("%09d",nResultID);
			nResultID++;
			m_Xml.addElementValue(pTemp,strXmlBuildID + "2" + strIndex);

			pTemp = m_Xml.addXmlChildElement(pElecHour,"F_EnergyItemCode");
			m_Xml.addElementValue(pTemp,strSortCode);

			pTemp = m_Xml.addXmlChildElement(pElecHour,"F_StartHour");
			m_Xml.addElementValue(pTemp,strStartHour);

			pTemp = m_Xml.addXmlChildElement(pElecHour,"F_EndHour");
			m_Xml.addElementValue(pTemp,strEndHour);

			pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourValue");
			m_Xml.addElementValue(pTemp,strSumValue);

			strTemp.Format("%.2f",atof(strSumValue)*ValueTurn);

			pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourEquValue");
			m_Xml.addElementValue(pTemp,strTemp);

			pTemp = m_Xml.addXmlChildElement(pElecHour,"F_State");
			m_Xml.addElementValue(pTemp,"1");
		}
		arrElecItems.RemoveAll();

		CString strKey;
		EnergySum* pSum = NULL;
		POSITION nPos = m_mapEnergySum.GetStartPosition();
		while(nPos)
		{
			m_mapEnergySum.GetNextAssoc(nPos,strKey,(void*&)pSum);
			if (pSum)
			{
				pBuild = m_Xml.getElementByMarkAttribute(pData,"Build","id",strKey);
				if(pBuild==NULL)
				{
					pBuild = m_Xml.addXmlChildElement(pData,"Build");
					pBuild->SetAttribute("id",strKey);
				}
				//01A00
				if(pSum->sortA >= 0)
				{
					pElecHour = m_Xml.addXmlChildElement(pBuild,"EnergyItemHourResult");
					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourResultID");
					strIndex.Format("%09d",nResultID);
					nResultID++;
					m_Xml.addElementValue(pTemp,strXmlBuildID + "2" + strIndex);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_EnergyItemCode");
					m_Xml.addElementValue(pTemp,"01A00");

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_StartHour");
					m_Xml.addElementValue(pTemp,strStartHour);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_EndHour");
					m_Xml.addElementValue(pTemp,strEndHour);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourValue");
					strSumValue.Format("%.2f",pSum->sortA);
					m_Xml.addElementValue(pTemp,strSumValue);

					strTemp.Format("%.2f",atof(strSumValue)*ValueTurn);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourEquValue");
					m_Xml.addElementValue(pTemp,strTemp);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_State");
					m_Xml.addElementValue(pTemp,"1");
				}
				if(pSum->sortB >= 0)
				{
					//01B00
					pElecHour = m_Xml.addXmlChildElement(pBuild,"EnergyItemHourResult");
					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourResultID");
					strIndex.Format("%09d",nResultID);
					nResultID++;
					m_Xml.addElementValue(pTemp,strXmlBuildID + "2" + strIndex);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_EnergyItemCode");
					m_Xml.addElementValue(pTemp,"01B00");

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_StartHour");
					m_Xml.addElementValue(pTemp,strStartHour);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_EndHour");
					m_Xml.addElementValue(pTemp,strEndHour);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourValue");
					strSumValue.Format("%.2f",pSum->sortB);
					m_Xml.addElementValue(pTemp,strSumValue);

					strTemp.Format("%.2f",atof(strSumValue)*ValueTurn);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourEquValue");
					m_Xml.addElementValue(pTemp,strTemp);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_State");
					m_Xml.addElementValue(pTemp,"1");
				}

				if (pSum->sortC>=0)
				{
					//01C00
					pElecHour = m_Xml.addXmlChildElement(pBuild,"EnergyItemHourResult");
					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourResultID");
					strIndex.Format("%09d",nResultID);
					nResultID++;
					m_Xml.addElementValue(pTemp,strXmlBuildID + "2" + strIndex);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_EnergyItemCode");
					m_Xml.addElementValue(pTemp,"01C00");

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_StartHour");
					m_Xml.addElementValue(pTemp,strStartHour);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_EndHour");
					m_Xml.addElementValue(pTemp,strEndHour);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourValue");
					strSumValue.Format("%.2f",pSum->sortC);
					m_Xml.addElementValue(pTemp,strSumValue);

					strTemp.Format("%.2f",atof(strSumValue)*ValueTurn);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourEquValue");
					m_Xml.addElementValue(pTemp,strTemp);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_State");
					m_Xml.addElementValue(pTemp,"1");

				}

				if (pSum->sortD>=0)
				{
					//01D00
					pElecHour = m_Xml.addXmlChildElement(pBuild,"EnergyItemHourResult");
					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourResultID");
					strIndex.Format("%09d",nResultID);
					nResultID++;
					m_Xml.addElementValue(pTemp,strXmlBuildID + "2" + strIndex);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_EnergyItemCode");
					m_Xml.addElementValue(pTemp,"01D00");

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_StartHour");
					m_Xml.addElementValue(pTemp,strStartHour);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_EndHour");
					m_Xml.addElementValue(pTemp,strEndHour);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourValue");
					strSumValue.Format("%.2f",pSum->sortD);
					m_Xml.addElementValue(pTemp,strSumValue);

					strTemp.Format("%.2f",atof(strSumValue)*ValueTurn);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourEquValue");
					m_Xml.addElementValue(pTemp,strTemp);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_State");
					m_Xml.addElementValue(pTemp,"1");
				}
				
				if ((!bHasSum01000) && pSum->sortSum>=0)
				{
					//01000
					pElecHour = m_Xml.addXmlChildElement(pBuild,"EnergyItemHourResult");
					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourResultID");
					strIndex.Format("%09d",nResultID);
					nResultID++;
					m_Xml.addElementValue(pTemp,strXmlBuildID + "2" + strIndex);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_EnergyItemCode");
					m_Xml.addElementValue(pTemp,"01000");

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_StartHour");
					m_Xml.addElementValue(pTemp,strStartHour);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_EndHour");
					m_Xml.addElementValue(pTemp,strEndHour);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourValue");
					strSumValue.Format("%.2f",pSum->sortSum);
					m_Xml.addElementValue(pTemp,strSumValue);

					strTemp.Format("%.2f",atof(strSumValue)*ValueTurn);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_HourEquValue");
					m_Xml.addElementValue(pTemp,strTemp);

					pTemp = m_Xml.addXmlChildElement(pElecHour,"F_State");
					m_Xml.addElementValue(pTemp,"1");
				}

			}
		}
	}

#pragma endregion 单个建筑
	m_Xml.saveFile(DestFile);
	m_Xml.Clear();
	return TRUE;
}

void CProtocolManager::SetMainWnd(HWND hWnd)
{
	_hMainWnd = hWnd;
}

void CProtocolManager::WriteFileInfo(CString strFullPath,char* pData,int nSize)
{
	CFile file;
	BOOL bOpen = file.Open(strFullPath,CFile::modeCreate|CFile::modeReadWrite);
	if(bOpen){
		file.Write(pData,nSize);
	}
	file.Close();
}

int CProtocolManager::ReadFileInfo(CString strFullPath,char* pData,int nMaxSize)
{
	int nSize = -1;
	CFile file;
	BOOL bOpen = file.Open(strFullPath,CFile::modeRead);
	if(bOpen){
		nSize = file.Read(pData,nMaxSize);
	}
	file.Close();
	return nSize;
}

void CProtocolManager::AddSortItemToMap(CString strAreaCode,CString strSortCode,CString strValue)
{
	EnergySum* pSum = NULL;
	BOOL bFind = m_mapEnergySum.Lookup(strAreaCode,(void*&)pSum);
	if(!bFind)
	{
		pSum = new EnergySum;
		m_mapEnergySum.SetAt(strAreaCode,pSum);
	}
	if (pSum)
	{
		if (strSortCode.Left(3) == "01A" ) //照明插座用电
		{
			if (pSum->sortA == -1)
				pSum->sortA =  atof(strValue);
			else
				pSum->sortA += atof(strValue);
		}
		if (strSortCode.Left(3) == "01B" ) //空调用电
		{
			if (pSum->sortB == -1)
				pSum->sortB =  atof(strValue);
			else
				pSum->sortB += atof(strValue);
		}
		if (strSortCode.Left(3) == "01C" ) //动力用电
		{
			if (pSum->sortC == -1)
				pSum->sortC =  atof(strValue);
			else
				pSum->sortC += atof(strValue);
		}
		if (strSortCode.Left(3) == "01D" ) //其它用电
		{
			if (pSum->sortD == -1)
				pSum->sortD =  atof(strValue);
			else
				pSum->sortD += atof(strValue);
		}

		if (pSum->sortSum == -1)
			pSum->sortSum =  atof(strValue);
		else
			pSum->sortSum += atof(strValue);
	}
}

void CProtocolManager::DeleteEnergySumMap()
{
	CString strKey;
	EnergySum* pSum = NULL;
	POSITION nPos = m_mapEnergySum.GetStartPosition();
	while(nPos)
	{
		m_mapEnergySum.GetNextAssoc(nPos,strKey,(void*&)pSum);
		if (pSum)
		{
			delete pSum;
			pSum = NULL;
		}
	}
	m_mapEnergySum.RemoveAll();
}

