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
	//��ȡ֤��
	int getSignerCert(BYTE* cpCert,int& nCertLen,CString signerCertThumbprint);
	//��� ֡ͷ+֡β
	void CompseDatagram(BYTE* cpDataOut,int& dataOutLen,BYTE* body,int nbodyLen,CommandType cmdType);
	//��Ϣǩ��
	int SignMsg(BYTE* cpSendBuf, int nSendBufLen,CString signerCertThumbprint,BYTE* cpDataOut,int& cpDataOutLen);
	//��֤ǩ��
	int VerifyMsg(BYTE* encodedSignedCms, int nEncodedSignedCmsLen, BYTE* decodeData, int& ndecodeDataLen);
	//ZIP�ļ�����
	int EncryptMsg(BYTE* senderCert,int nSenderCertLen, BYTE* recvCert, int nRecvCertLen, BYTE* Msg, int nMsgLen, BYTE* cpDataOut,int& nOutLen);
	//ZIP�ļ�����
	int DecryptMsg(BYTE* encodedEnvelopedCms,int encodeLen, BYTE* cpDataOut,int& nOutLen);
	//��ȡ��½��Ϣ֡
	int GetLoginData(CString strNodeID,char* cpDataOut,int& nOutLen);
	//��ȡ������Ϣ֡
	int GetHeartData(CString strNodeID,char* cpDataOut,int& nOutLen);
	//��ȡ���������ϴ�����֡
	int GetPacketStartData(CString strNodeID,CString strFileName,BYTE* cpDataOut,int& nOutLen);
	//��ȡ��Zip����֡
	int GetNHDataData(CString strZipFileName,CString strEncodeFileName,CString& strMD5File);
	//��ȡMD5ֵ
	BOOL GetMd5(CString FileDirectory,CString &strFileMd5);
	//��ȡ���������ϴ�����֡
	int GetPacketEndData(CString strNodeID,CString strFileMD5,BYTE* cpDataOut,int& nOutLen);
	//��ȡ��������֡
	int GetPeriodACKData(CString strNodeID,CString strPeriod,BYTE* cpDataOut,int& nOutLen);
	//��ȡ���Ĳ�ѯʱ ������˵�Ӧ��
	int GetReplyStartData(CString strNodeID,CString strFileName,BYTE* cpDataOut,int& nOutLen);
	//��ȡ���Ĳ�ѯʱ �ϴ�����������֡
	int GetReplyEndData(CString strNodeID,CString strFileMD5,BYTE* cpDataOut,int& nOutLen);

	//���͵�½��Ϣ
	BOOL DoLogin();
public:
	//����XML��Ϣ
	int ReadRecvData(CString strNodeID,BYTE* cpData,int nLen);
	//�յ���½���
	void RevResult(CString strXMLData);
	//�յ���������
	void RevPeriod(CString strXMLData);
	//�յ�ʱ��ͬ��
	void RecvTime(CString strXMLData);
	//�յ����ݲ�ѯ
	void RevQuery(CString strStartTime,CString strEndTime);
	//�յ����ݰ������ϴ�����ȷ��
	void RecvPacketStartAck(CString strFileName,CString strFileLen);
	//�յ����ݰ������ϴ�����ȷ��
	void RecvPacketEndAck(CString strType);
	//�յ����ݰ���ѯ ����ȷ��
	void RevReplyStartAck(CString strType);
	//�յ����ݰ���ѯ ����ȷ��
	void RevReplyEndAck(CString strType);
	//���� ��������Ϣ֡
	void SendPacketEnd(CString strFileMD5, BOOL Query=FALSE);

public:
	//�������·���ļ�����Ŀ¼|�ǿ�Ŀ¼��
	void DeleteDirectory(CString sDirName);
	//��д������ϢXML
	BOOL SetBuildXML(CString SourceFile, CString DestFile, CString NodeID ,CString StartTime, CString EndTime);
	//��д�ܺ�XML
	BOOL SetEnergyXML(CString SourceFile,CString DestFile, CString NodeID ,CString StartTime, CString EndTime);
public:
	//��ǰ�ϴ��ļ�MD5
	CString m_FNewQueryUploadFileMD5;
	//��ǰ�ϴ��ļ���֡ͷ+����+�ļ�+֡β��
	CString m_FNewQueryUploadEncodeFile;
	//��ǰ�ϴ��ļ�ZIP
	CString m_FNewQueryUploadZipFile;
	//��дZIPѹ����
	BOOL MakeUploadFile(CString strStartTime="",CString strEndTime="");
public:
	//�Զ��ϴ���ǰʱ����Ϣ
	void BeginUpload(CString StartTime="",CString EndTime="",BOOL Query=FALSE);
public:
	//��¼��ǰ�ϴ�ʱ���
	CString m_strCurTimeSpan;
	//�Զ��ϴ�����Ŀ¼��Ϣ
	void BeginUploadBackUp();
public: //Socket
	//��ȡ��½״̬
	bool LoginSucceed(){ return m_bIsLogined;}
	//���õ�½״̬
	void ResetLoginState(){ m_bIsLogined = false;}
	//��ȡSocket����
	CTCPClient& GetSocket(){ return m_ClientSock; }
	//��������
	BOOL SendData(char* cpData,int nLen);
	//��ȡ Socket����״̬
	bool SocketIsConnected();
	//�ر�Socket
	void KillSocket();
	//����Զ�˷�����
	BOOL ConnectServer();
	//��ȡ��Ϣ����
	CStringList& GetMsgList();
	//��ȡ��Ϣ
	CString GetFirstMsg();
protected:
	bool m_bIsLogined;
	CTCPClient m_ClientSock;
public:
	//��ȡ�Զ��ϴ�ʱ����
	int GetTimerInterval(){ return m_nTimerData; }
	//�����Զ��ϴ�ʱ����
	void SetTimerInterval(int nInterval){ m_nTimerData = nInterval;}
protected:
	//�Զ�����ʱ����
	int m_nTimerData;
public:
	//��ȡ��������
	int GetHearBeatSendCount(){ return m_nHeartSendCount; }
	//������������
	void IncreaseHeartSendCount(){ m_nHeartSendCount++; }
	//������������
	void ResetHeartSendCount(){ m_nHeartSendCount = 0; }
protected:
	//��������
	int m_nHeartSendCount;
public:
	//�Ƿ����ʱ��
	BOOL m_bUpdateTime;
public:
// 	//��־
// 	CLog m_Log;
public:
	//���������ھ��
	void SetMainWnd(HWND hWnd);
public:
	//д�ļ���������
	void WriteFileInfo(CString strFullPath,char* pData,int nSize);
	//���ļ�����
	int ReadFileInfo(CString strFullPath,char* pData,int nMaxSize);
public:
	CMapStringToPtr m_mapEnergySum;
	void AddSortItemToMap(CString strAreaCode,CString strSortCode,CString strValue);
	void DeleteEnergySumMap();
};

