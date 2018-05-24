
// TransDataTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TransDataTest.h"
#include "TransDataTestDlg.h"
#include "afxdialogex.h"
#include "common.h"
#include "IniFile.h"
#include "Utils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTransDataTestDlg �Ի���




CTransDataTestDlg::CTransDataTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTransDataTestDlg::IDD, pParent)
	, m_strRemoteIP(_T(""))
	, m_nRemotePort(0)
	, m_nHeartBeatCfgTime(0)
	, m_strNodeID(_T(""))
	, m_strDSN(_T(""))
	, m_strUserName(_T(""))
	, m_strPassWord(_T(""))
	, m_strSenderCert(_T(""))
	, m_nHeartFlag(0)
	, m_nUpLoadDataCount(0)
	, m_bCanRunRecTimer(true)
	, m_bCkBoxUpdateTime(FALSE)
	, m_tmctlStartDate(0)
	, m_tmctlEndDate(0)
	, m_tmctlStartTime(0)
	, m_tmctlEndTime(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTransDataTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_REMOTEIP, m_strRemoteIP);
	DDX_Text(pDX, IDC_EDIT_REMOTEPORT, m_nRemotePort);
	DDX_Text(pDX, IDC_EDIT_BEATTIME, m_nHeartBeatCfgTime);
	DDX_Text(pDX, IDC_EDIT_NODEID, m_strNodeID);
	DDX_Text(pDX, IDC_EDIT_DSN, m_strDSN);
	DDX_Text(pDX, IDC_EDIT_USER, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_PASS, m_strPassWord);
	DDX_Text(pDX, IDC_EDIT_CERT, m_strSenderCert);
	DDX_Control(pDX, IDC_LIST_INFO, m_lstInfo);
	DDX_Check(pDX, IDC_CHECK_UPDATETIME, m_bCkBoxUpdateTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START_DATE, m_tmctlStartDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END_DATE, m_tmctlEndDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START_TIME, m_tmctlStartTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END_TIME, m_tmctlEndTime);
}

BEGIN_MESSAGE_MAP(CTransDataTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START, &CTransDataTestDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTransDataTestDlg::OnBnClickedButtonStop)
	ON_MESSAGE(WM_SHOWLOG,OnShowMsg)
	ON_MESSAGE(WM_RESTART,OnRestartMsg)
	ON_BN_CLICKED(IDC_CHECK_UPDATETIME, &CTransDataTestDlg::OnBnClickedCheckUpdatetime)
	ON_BN_CLICKED(IDC_BUTTON_MANUL_UPDATE, &CTransDataTestDlg::OnBnClickedButtonManulUpdate)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CTransDataTestDlg::OnBnClickedButtonClear)
	ON_EN_CHANGE(IDC_EDIT_CERT, &CTransDataTestDlg::OnEnChangeEditCert)
END_MESSAGE_MAP()


// CTransDataTestDlg ��Ϣ�������

BOOL CTransDataTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MANUL_UPDATE)->EnableWindow(FALSE);

	CTime tmNow =  CTime::GetCurrentTime();
	m_tmctlStartDate = tmNow;
	m_tmctlEndDate = tmNow;

	CTimeSpan timSpan(0,3,0,0);
	CTime tmStart = CTime(tmNow.GetYear(),tmNow.GetMonth(),tmNow.GetDay(),tmNow.GetHour(),0,0);
	CTime tmEnd = tmStart + CTimeSpan(0,1,0,0);
	
	m_tmctlStartDate = tmStart - timSpan;
	m_tmctlEndDate =  tmEnd - timSpan;

	m_tmctlStartTime = tmStart - timSpan;
	m_tmctlEndTime = tmEnd - timSpan;


	CString strReloadPath = m_Protocol.GetAppPath()+_T("Reload");//��������·��
	CreateDirectory(strReloadPath,NULL);
	CString strUpLoadPath =m_Protocol. GetAppPath()+_T("upload");
	CreateDirectory(strUpLoadPath,NULL);

	CString strCache = m_Protocol.GetAppPath()+_T("Cache");
	CreateDirectory(strCache,NULL);

	m_strSettingsPath = m_Protocol.GetAppPath()+_T("Settings.ini");
	m_Protocol.m_strSettingsPath = m_strSettingsPath;

	LoadSettings();
	m_Protocol._hMainWnd = GetSafeHwnd();

	

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTransDataTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_HEARTBEAT:
		OnProcHeartBeat();
		break;
	case TIMER_UPLOADDATA:
		OnProcLoadData();
		break;
	case TIMER_RECONNECT:
		OnProcReconnect();
		break;
	case TIMER_RECV:
		OnProcRecv();
		break;
	case TIMER_LOGINDATA:
		OnProcLogin();
		break;
	case TIMER_RELOAD:
		OnProcReload();
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CTransDataTestDlg::OnProcHeartBeat()
{
	if(!m_Protocol.SocketIsConnected())return;

	m_nHeartFlag-=1000;
	if(m_nHeartFlag<=0)
	{
		m_nHeartFlag = m_nHeartBeatCfgTime*60*1000;
		if(m_Protocol.GetHearBeatSendCount()>=2)
		{
			m_Protocol.ResetHeartSendCount();
			m_Protocol.KillSocket();
			m_Protocol.ResetLoginState();
			ShowLog(_T("3�������޻�Ӧ��׼����������"));
			PostMessage(WM_RESTART);
			return;
		}
		m_Protocol.IncreaseHeartSendCount();
		ShowLog(_T("�������������������notify......"));
		char* cpData = new char[MAX_DATA_FILE];
		ZeroMemory(cpData,MAX_DATA_FILE);
		int nDataLen = 0;
		int nRet = m_Protocol.GetHeartData(m_strNodeID, cpData, nDataLen);
		if(nRet!=0){
			CString strLog;
			strLog.Format("%d",nRet);
			ShowLog(_T("����notify���ʧ�ܣ�ԭ��") + strLog + _T("��"));
		}
		else{
			BOOL bSend = m_Protocol.SendData(cpData,nDataLen);
			if(!bSend)
				ShowLog(_T("����notify����ʧ�ܣ�"));
			else
				ShowLog(_T("����notify�ѷ��ͣ��ȴ���������Ӧ"));
		}
		delete[] cpData;
	}
}

void CTransDataTestDlg::OnProcLogin()
{
	KillTimer(TIMER_LOGINDATA);
	m_Protocol.ResetLoginState();
	if(m_Protocol.SocketIsConnected())
	{
		BOOL bRet = m_Protocol.DoLogin();
		if(!bRet)
			ShowLog(_T("���͵�½��Ϣʧ��"));
	}else{
		m_Protocol.KillSocket();
		ShowLog(_T("����Զ�̷�����ʧ�ܣ�"));
	}
}

void CTransDataTestDlg::OnProcReconnect()
{
	if(!m_Protocol.SocketIsConnected() && m_bCanRunRecTimer)
	{
		m_Protocol.ResetLoginState();
		m_nUpLoadDataCount = 0;
		m_bCanRunRecTimer = false;
		ShowLog(_T("Զ�������ѶϿ�����"));
		m_Protocol.ConnectServer();
		m_bCanRunRecTimer = true;
		SetTimer(TIMER_LOGINDATA,2000,NULL);
	}
}

void CTransDataTestDlg::OnProcLoadData()
{
	if(m_Protocol.LoginSucceed() && m_Protocol.SocketIsConnected())
	{
		m_nUpLoadDataCount-=1000;
		if(m_nUpLoadDataCount<=0)
		{
			m_nUpLoadDataCount = m_Protocol.GetTimerInterval();
			m_Protocol.BeginUpload();
		}
	}
}

void CTransDataTestDlg::OnProcRecv()
{
	CString strData = m_Protocol.GetFirstMsg();
	if (strData.IsEmpty())
	{
		return ;
	}
	if(m_Protocol.LoginSucceed()==false)
	{
		BYTE* pData = new BYTE[MAX_DATA_FILE];
		ZeroMemory(pData,MAX_DATA_FILE);
		int nLen = 0;
		BYTE Cert[MAX_CERT_LEN] = {0};
		int CertLen = 0;
		int nRet = m_Protocol.getSignerCert(Cert,CertLen,m_strSenderCert);

		int nVerify = m_Protocol.VerifyMsg((BYTE*)strData.GetBuffer(),strData.GetLength(), pData,nLen);
		strData.ReleaseBuffer();		
		if(nVerify==0)
			m_Protocol.ReadRecvData(m_strNodeID, pData, nLen);
		else{
			ShowLog(_T("������½����ʧ��"));
		}
		delete[] pData;
	}else{
		m_Protocol.ReadRecvData(m_strNodeID,(BYTE*)strData.GetBuffer(),strData.GetLength());
		strData.ReleaseBuffer();
	}
}

void CTransDataTestDlg::OnProcReload()
{
	CSingleLock lk(&m_ReloadSec,TRUE);
	CString strUploadPath = m_Protocol.GetAppPath()+"upload";
	BOOL bEmpty = PathIsDirectoryEmpty(strUploadPath);
	if(bEmpty && m_Protocol.SocketIsConnected() && m_Protocol.LoginSucceed()){
		m_Protocol.BeginUploadBackUp();
	}
}

void CTransDataTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTransDataTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTransDataTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTransDataTestDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	Stop();
	m_lstInfo.ResetContent();
	CDialogEx::OnClose();
}

void CTransDataTestDlg::LoadSettings()
{
	CIniFile iniFile(m_strSettingsPath);
	m_strRemoteIP = iniFile.GetString(_T("Setting"),_T("ServerIP"));
	m_nRemotePort = iniFile.GetInt(_T("Setting"),_T("ServerPort"),1000);
	m_nHeartBeatCfgTime = iniFile.GetInt(_T("Setting"),_T("HeartInterval"),15);
	m_strDSN = iniFile.GetString(_T("Setting"),_T("DSN"));
	m_strUserName = iniFile.GetString(_T("Setting"),_T("UserName"));
	m_strPassWord = iniFile.GetString(_T("Setting"),_T("PassWord"));
	m_strNodeID = iniFile.GetString(_T("Setting"),_T("NodeID"));
	m_strSenderCert = iniFile.GetString(_T("Setting"),_T("PrmID"));

	UpdateData(FALSE);
}

void CTransDataTestDlg::SaveSettings()
{
	UpdateData();

	CIniFile iniFile(m_strSettingsPath);
	iniFile.WriteString(_T("Setting"),_T("ServerIP"),m_strRemoteIP);
	iniFile.WriteInt(_T("Setting"),_T("ServerPort"),m_nRemotePort);
	iniFile.WriteInt(_T("Setting"),_T("HeartInterval"),m_nHeartBeatCfgTime);
	iniFile.WriteString(_T("Setting"),_T("DSN"),m_strDSN);
	iniFile.WriteString(_T("Setting"),_T("UserName"),m_strUserName);
	iniFile.WriteString(_T("Setting"),_T("PassWord"),m_strPassWord);
	iniFile.WriteString(_T("Setting"),_T("NodeID"),m_strNodeID);
	iniFile.WriteString(_T("Setting"),_T("PrmID"),m_strSenderCert);
}

void CTransDataTestDlg::Start()
{
	SaveSettings();
	m_Protocol.m_strSenderCertCode = m_strSenderCert;
	m_Protocol.m_strNodeID = m_strNodeID;
	m_Protocol.m_strRemoteIP = m_strRemoteIP;
	m_Protocol.m_nRemotePort = m_nRemotePort;

	ShowLog(_T("----------------------------------------------------------"));
	ShowLog(_T("��ʼ��½"));
	ShowLog(_T("���ڻ�ȡ֤�顭��"));

/*	m_Protocol.BeginUpload();*/

	BYTE* data = new BYTE[MAX_DATA_FILE];
	ZeroMemory(data,MAX_DATA_FILE);
	int dataLen = 0;
	int nRet = m_Protocol.getSignerCert(data,dataLen,m_strSenderCert);
	delete[] data;
	if(nRet!=0)
	{
		CString strLog;
		strLog.Format(_T("��ȡ֤��ʧ��, ������룺%d ��"),nRet);
		ShowLog(strLog);
		return;
	}
	 ShowLog(_T("��ȡ֤��ɹ�"));
	 ShowLog(_T("�������ӱ������ݿ�......"));

//	 bool bOpen = g_DBMgr.Init(m_strUserName,m_strPassWord,m_strDSN);
	 bool bOpen = g_DBMgr.Init();
	 if(!bOpen)
	 {
		 ShowLog(_T("���ӱ������ݿ�ʧ�ܣ�"));
		 return;
	 }
	  ShowLog(_T("���ӱ������ݿ�ɹ�"));

	  m_Protocol.ConnectServer();
	  //500ms���ⷢ�͵�½��Ϣ
	  SetTimer(TIMER_LOGINDATA,500,NULL);
	  //10ms��������Ϣ
	  SetTimer(TIMER_RECV,10,NULL);
	  //1s��ʱ����Զ��ϴ�����
	  SetTimer(TIMER_UPLOADDATA,1000,NULL);
	  //10s��ʱ�������״̬�ָ�TCP����
	  SetTimer(TIMER_RECONNECT,10*1000,NULL);
	  //2���Ӽ�鸴������
	  SetTimer(TIMER_RELOAD,8*1000,NULL);

	  m_nHeartFlag = m_nHeartBeatCfgTime*60*1000;
	  //����
	  SetTimer(TIMER_HEARTBEAT,1000,NULL);

	  UpdateUI();

/*	  m_Protocol.MakeUploadFile();*/
//	  m_Protocol.MakeUploadFile("2016-08-23 00:00:00","2016-08-23 01:59:59");
//	  m_Protocol.BeginUploadBackUp();
}

void CTransDataTestDlg::Stop()
{
	UpdateUI(FALSE);
	
	KillTimer(TIMER_RELOAD);
	KillTimer(TIMER_RECV);
	KillTimer(TIMER_HEARTBEAT);
	KillTimer(TIMER_RECONNECT);
	KillTimer(TIMER_UPLOADDATA);
	KillTimer(TIMER_LOGINDATA);

	m_nUpLoadDataCount = 0;

	m_Protocol.KillSocket();
	m_Protocol.ResetLoginState();
	g_DBMgr.Release();
}

void CTransDataTestDlg::ShowLog(CString strMsg)
{
	theApp.WriteLog(strMsg);
	CTime tm = CTime::GetCurrentTime();
	CString strTime = tm.Format("%Y-%m-%d %H:%M:%S");
	CString strTemp = strTime + _T(" ---- ")+strMsg;
	int nItems = m_lstInfo.GetCount();
	int n = m_lstInfo.AddString(strTemp);
	m_lstInfo.SetCaretIndex(n);
	if(nItems>=100)
	{
		m_lstInfo.ResetContent();
	}
}

void CTransDataTestDlg::OnBnClickedButtonStart()
{
	Start();
}

void CTransDataTestDlg::OnBnClickedButtonStop()
{
	Stop();
}

LRESULT CTransDataTestDlg::OnShowMsg(WPARAM wParam,LPARAM lParam)
{
	CString strInfo = CString((char*)lParam);
	ShowLog(strInfo);
	return 0;
}

LRESULT CTransDataTestDlg::OnRestartMsg(WPARAM wParam,LPARAM lParam)
{
	ShowLog(_T("������������"));
	Stop();
	Start();
	return 0;
}


void CTransDataTestDlg::UpdateUI(BOOL bReadOnly /*= TRUE*/)
{
	if (bReadOnly)
	{
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_REMOTEIP))->SetReadOnly();
		((CEdit*)GetDlgItem(IDC_EDIT_REMOTEPORT))->SetReadOnly();
		((CEdit*)GetDlgItem(IDC_EDIT_BEATTIME))->SetReadOnly();
		((CEdit*)GetDlgItem(IDC_EDIT_NODEID))->SetReadOnly();
		((CEdit*)GetDlgItem(IDC_EDIT_CERT))->SetReadOnly();
		((CEdit*)GetDlgItem(IDC_EDIT_DSN))->SetReadOnly();
		((CEdit*)GetDlgItem(IDC_EDIT_USER))->SetReadOnly();
		((CEdit*)GetDlgItem(IDC_EDIT_PASS))->SetReadOnly();
		GetDlgItem(IDC_BUTTON_MANUL_UPDATE)->EnableWindow(TRUE);
		
	}else{
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_REMOTEIP))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_REMOTEPORT))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_BEATTIME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_NODEID))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_CERT))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_DSN))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_USER))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_PASS))->SetReadOnly(FALSE);
		GetDlgItem(IDC_BUTTON_MANUL_UPDATE)->EnableWindow(FALSE);
	}
}

void CTransDataTestDlg::OnBnClickedCheckUpdatetime()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	m_Protocol.m_bUpdateTime = m_bCkBoxUpdateTime;
}



void CTransDataTestDlg::OnBnClickedButtonManulUpdate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSingleLock lk(&m_ReloadSec,TRUE);

	UpdateData();
	CString strStartTime,strEndTime;

	CTime tmStart = CTime(m_tmctlStartDate.GetYear(),m_tmctlStartDate.GetMonth(),m_tmctlStartDate.GetDay(),m_tmctlStartTime.GetHour(),m_tmctlStartTime.GetMinute(),m_tmctlStartTime.GetSecond());
	CTime tmEnd = CTime(m_tmctlEndDate.GetYear(),m_tmctlEndDate.GetMonth(),m_tmctlEndDate.GetDay(),m_tmctlEndTime.GetHour(),m_tmctlEndTime.GetMinute(),m_tmctlEndTime.GetSecond());
	if(tmStart>=tmEnd)
	{
		ShowLog("�����ϴ�ʱ������");
		return;
	}


	CString strReloadTask = m_Protocol.GetAppPath()+"Reload\\Task.ini";
	CIniFile iniFile(strReloadTask);
	int nCount = iniFile.GetInt("ReloadTask","Count",0);

	int nTimes = 0;
	CString strSec = 0;
	CTime tmTmp = tmStart;
	while (tmTmp < tmEnd)
	{
		strStartTime = tmTmp.Format("%Y-%m-%d %H:00:00");
		
		tmTmp = tmTmp + CTimeSpan(0,1,0,0);

		strEndTime = tmTmp.Format("%Y-%m-%d %H:00:00");

		if (tmTmp>= tmEnd)
		{
			strEndTime = tmEnd.Format("%Y-%m-%d %H:00:00");
		}
		nTimes++;

		strSec.Format("T%d",nTimes+nCount);

		iniFile.WriteString(strSec,"TimeSpan",strStartTime+"|"+strEndTime);

		TRACE("%s--%s\r\n",strStartTime,strEndTime);
/*		m_Protocol.BeginUpload(strStartTime,strEndTime);*/
	}

	iniFile.WriteInt("ReloadTask","Count",nTimes+nCount);

/*	m_Protocol.BeginUploadBackUp();*/
}


void CTransDataTestDlg::OnBnClickedButtonClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_lstInfo.ResetContent();
}


void CTransDataTestDlg::OnEnChangeEditCert()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	m_strSenderCert.Remove(' ');
	UpdateData(FALSE);
}
