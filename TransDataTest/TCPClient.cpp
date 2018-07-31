// TCPClient.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TransDataTest.h"
#include "TCPClient.h"


// CTCPClient

CTCPClient::CTCPClient()
	: m_bIsConnected(false)
{
	int nSendRecvBuf=1024*1024;
	BOOL bRet = SetSockOpt(SO_RCVBUF,(const char*)&nSendRecvBuf,sizeof(int));
	bRet = SetSockOpt(SO_SNDBUF,(const char*)&nSendRecvBuf,sizeof(int));

	LINGER m_sLinger;
	m_sLinger.l_onoff=1;//(��closesocket()����,���ǻ�������û������ϵ�ʱ��������)
	// ���m_sLinger.l_onoff=0;���ܺ�2.)������ͬ;
	m_sLinger.l_linger=5;//(��������ʱ��Ϊ5��)
	bRet = SetSockOpt(SO_LINGER,(const char*)&m_sLinger,sizeof(LINGER));
}

CTCPClient::~CTCPClient()
{
	m_MsgList.RemoveAll();
}


// CTCPClient ��Ա����


void CTCPClient::OnConnect(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(nErrorCode==0)
		m_bIsConnected = true;
	CAsyncSocket::OnConnect(nErrorCode);
}


void CTCPClient::OnClose(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	m_bIsConnected = false;
	CAsyncSocket::OnClose(nErrorCode);
}


void CTCPClient::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	CSingleLock lk(&m_InfoSec,TRUE);
	int nDataLen = MAX_DATA_FILE;
	char* pData = new char[MAX_DATA_FILE];
	ZeroMemory(pData,nDataLen);
	int nRetLen = Receive(pData,nDataLen);
	CString strMsg = CString(pData);
	m_MsgList.AddTail(strMsg);
	delete[] pData;
	CAsyncSocket::OnReceive(nErrorCode);
}

BOOL CTCPClient::GetSockValid()
{
	return m_hSocket!=INVALID_SOCKET;
}

BOOL CTCPClient::WriteData(char* pData,int nTotalLen)
{
	CSingleLock lk(&m_SendSec,TRUE);
	int nIndex = 0;
	BOOL bResult = FALSE;
	int nSendLen = Send(pData,nTotalLen);

	bResult = (nSendLen!=SOCKET_ERROR);

	if(nSendLen==SOCKET_ERROR)
	{
		CString strLog;
		strLog.Format("SocketError : %d",GetLastError());
		return FALSE;
	}
	if(nSendLen==nTotalLen)
		return TRUE;
	else{
		while(bResult &&(nSendLen<nTotalLen))
		{
			nTotalLen = nTotalLen - nSendLen;
			nIndex+=nSendLen;
			nSendLen = Send(pData+nIndex,nTotalLen);
			bResult = (nSendLen!=SOCKET_ERROR);
		}
		if(nTotalLen==0)
			return TRUE;
	}
	return FALSE;
}

CString CTCPClient::GetMessageInfo()
{
	CSingleLock lk(&m_InfoSec,TRUE);
	CString strResult;
	if(!m_MsgList.IsEmpty())
		strResult = m_MsgList.RemoveHead();

	return strResult;
}
