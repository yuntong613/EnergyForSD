#pragma once

// CTCPClient ÃüÁîÄ¿±ê

class CTCPClient : public CAsyncSocket
{
public:
	CTCPClient();
	virtual ~CTCPClient();
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	bool m_bIsConnected;
public:
	BOOL GetSockValid();
	BOOL WriteData(char* pData,int nTotalLen);
	CStringList m_MsgList;
	bool IsConnected(){ return m_bIsConnected; }
	CCriticalSection m_InfoSec;
	CString GetMessageInfo();
	CCriticalSection m_SendSec;
};


