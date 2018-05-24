#pragma once

#include "TCPClient.h"
#include "tinyxml.h"

typedef struct _ENERGY_SUM
{
	double sortA;
	double sortB;
	double sortC;	
	double sortD;
	double sortSum;
	_ENERGY_SUM()
	{
		sortA = -1;
		sortB = -1;
		sortC = -1;
		sortD = -1;
		sortSum = -1;
	};
}EnergySum;

class CProtocolManager
{
public:
	CProtocolManager(void);
	~CProtocolManager(void);
public:
	HWND _hMainWnd;
	CString m_strRemoteIP;
	int m_nRemotePort;
	CString m_strSenderCertCode;

	CString m_strNodeID;
	CString m_strSettingsPath;
	CString GetAppPath();
	void ShowLog(CString strMsg);
public:
	enum CommandType
	{
		Unknown = -1,
		Login = 0,
		Heartbeat = 1,
		UploadDataRequest = 2,
		EnergyData = 3,
		UploadFinishConfirm = 4,
		ConfigFinishConfirm = 5,
		UploadQueryDataRequest = 7,
		UploadQueryDataFinishConfirm = 8,
		SendQueryDataRequest = 9
	};
	BYTE m_cpCenterCert[MAX_DATA_FILE];
	int m_nCenterCertLen;
	//获取证书
	int getSignerCert(BYTE* cpCert,int& nCertLen,CString signerCertThumbprint);
	//组包 帧头+帧尾
	void CompseDatagram(BYTE* cpDataOut,int& dataOutLen,BYTE* body,int nbodyLen,CommandType cmdType);
	//信息签名
	int SignMsg(BYTE* cpSendBuf, int nSendBufLen,CString signerCertThumbprint,BYTE* cpDataOut,int& cpDataOutLen);
	//验证签名
	int VerifyMsg(BYTE* encodedSignedCms, int nEncodedSignedCmsLen, BYTE* decodeData, int& ndecodeDataLen);
	//ZIP文件加密
	int EncryptMsg(BYTE* senderCert,int nSenderCertLen, BYTE* recvCert, int nRecvCertLen, BYTE* Msg, int nMsgLen, BYTE* cpDataOut,int& nOutLen);
	//ZIP文件解密
	int DecryptMsg(BYTE* encodedEnvelopedCms,int encodeLen, BYTE* cpDataOut,int& nOutLen);
	//获取登陆信息帧
	int GetLoginData(CString strNodeID,char* cpDataOut,int& nOutLen);
	//获取心跳信息帧
	int GetHeartData(CString strNodeID,char* cpDataOut,int& nOutLen);
	//获取数据主动上传请求帧
	int GetPacketStartData(CString strNodeID,CString strFileName,BYTE* cpDataOut,int& nOutLen);
	//获取带Zip数据帧
	int GetNHDataData(CString strZipFileName,CString strEncodeFileName,CString& strMD5File);
	//获取MD5值
	BOOL GetMd5(CString FileDirectory,CString &strFileMd5);
	//获取数据主动上传结束帧
	int GetPacketEndData(CString strNodeID,CString strFileMD5,BYTE* cpDataOut,int& nOutLen);
	//获取更新周期帧
	int GetPeriodACKData(CString strNodeID,CString strPeriod,BYTE* cpDataOut,int& nOutLen);
	//获取中心查询时 建筑物端的应答
	int GetReplyStartData(CString strNodeID,CString strFileName,BYTE* cpDataOut,int& nOutLen);
	//获取中心查询时 上传打包数据完成帧
	int GetReplyEndData(CString strNodeID,CString strFileMD5,BYTE* cpDataOut,int& nOutLen);

	//发送登陆信息
	BOOL DoLogin();
public:
	//解析XML信息
	int ReadRecvData(CString strNodeID,BYTE* cpData,int nLen);
	//收到登陆结果
	void RevResult(CString strXMLData);
	//收到周期配置
	void RevPeriod(CString strXMLData);
	//收到时间同步
	void RecvTime(CString strXMLData);
	//收到数据查询
	void RevQuery(CString strStartTime,CString strEndTime);
	//收到数据包主动上传请求确认
	void RecvPacketStartAck(CString strFileName,CString strFileLen);
	//收到数据包主动上传结束确认
	void RecvPacketEndAck(CString strType);
	//收到数据包查询 请求确认
	void RevReplyStartAck(CString strType);
	//收到数据包查询 结束确认
	void RevReplyEndAck(CString strType);
	//发送 最后结束信息帧
	void SendPacketEnd(CString strFileMD5, BOOL Query=FALSE);

public:
	//清空整个路径文件（空目录|非空目录）
	void DeleteDirectory(CString sDirName);
	//组写建筑信息XML
	BOOL SetBuildXML(CString SourceFile, CString DestFile, CString NodeID ,CString StartTime, CString EndTime);
	//组写能耗XML
	BOOL SetEnergyXML(CString SourceFile,CString DestFile, CString NodeID ,CString StartTime, CString EndTime);
public:
	//当前上传文件MD5
	CString m_FNewQueryUploadFileMD5;
	//当前上传文件（帧头+加密+文件+帧尾）
	CString m_FNewQueryUploadEncodeFile;
	//当前上传文件ZIP
	CString m_FNewQueryUploadZipFile;
	//组写ZIP压缩包
	BOOL MakeUploadFile(CString strStartTime="",CString strEndTime="");
public:
	//自动上传当前时间信息
	void BeginUpload(CString StartTime="",CString EndTime="",BOOL Query=FALSE);
public:
	//记录当前上传时间段
	CString m_strCurTimeSpan;
	//自动上传备份目录信息
	void BeginUploadBackUp();
public: //Socket
	//获取登陆状态
	bool LoginSucceed(){ return m_bIsLogined;}
	//重置登陆状态
	void ResetLoginState(){ m_bIsLogined = false;}
	//获取Socket对象
	CTCPClient& GetSocket(){ return m_ClientSock; }
	//发送数据
	BOOL SendData(char* cpData,int nLen);
	//获取 Socket连接状态
	bool SocketIsConnected();
	//关闭Socket
	void KillSocket();
	//连接远端服务器
	BOOL ConnectServer();
	//获取消息队列
	CStringList& GetMsgList();
	//获取消息
	CString GetFirstMsg();
protected:
	bool m_bIsLogined;
	CTCPClient m_ClientSock;
public:
	//获取自动上传时间间隔
	int GetTimerInterval(){ return m_nTimerData; }
	//设置自动上传时间间隔
	void SetTimerInterval(int nInterval){ m_nTimerData = nInterval;}
protected:
	//自动长传时间间隔
	int m_nTimerData;
public:
	//获取心跳计数
	int GetHearBeatSendCount(){ return m_nHeartSendCount; }
	//增加心跳计数
	void IncreaseHeartSendCount(){ m_nHeartSendCount++; }
	//重置心跳计数
	void ResetHeartSendCount(){ m_nHeartSendCount = 0; }
protected:
	//心跳计数
	int m_nHeartSendCount;
public:
	//是否更新时间
	BOOL m_bUpdateTime;
public:
// 	//日志
// 	CLog m_Log;
public:
	//设置主窗口句柄
	void SetMainWnd(HWND hWnd);
public:
	//写文件保存数据
	void WriteFileInfo(CString strFullPath,char* pData,int nSize);
	//读文件数据
	int ReadFileInfo(CString strFullPath,char* pData,int nMaxSize);
public:
	CMapStringToPtr m_mapEnergySum;
	void AddSortItemToMap(CString strAreaCode,CString strSortCode,CString strValue);
	void DeleteEnergySumMap();
};

