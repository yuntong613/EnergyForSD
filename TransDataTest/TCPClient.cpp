// TCPClient.cpp : 实现文件
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
	m_sLinger.l_onoff=1;//(在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
	// 如果m_sLinger.l_onoff=0;则功能和2.)作用相同;
	m_sLinger.l_linger=5;//(容许逗留的时间为5秒)
	bRet = SetSockOpt(SO_LINGER,(const char*)&m_sLinger,sizeof(LINGER));
}

CTCPClient::~CTCPClient()
{
	m_MsgList.RemoveAll();
}


// CTCPClient 成员函数


void CTCPClient::OnConnect(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(nErrorCode==0)
		m_bIsConnected = true;
	CAsyncSocket::OnConnect(nErrorCode);
}


void CTCPClient::OnClose(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	m_bIsConnected = false;
	CAsyncSocket::OnClose(nErrorCode);
}


void CTCPClient::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
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
