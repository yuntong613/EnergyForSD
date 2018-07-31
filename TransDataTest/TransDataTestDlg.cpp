
// TransDataTestDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CTransDataTestDlg 对话框




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


// CTransDataTestDlg 消息处理程序

BOOL CTransDataTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
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


	CString strReloadPath = m_Protocol.GetAppPath()+_T("Reload");//创建复传路径
	CreateDirectory(strReloadPath,NULL);
	CString strUpLoadPath =m_Protocol. GetAppPath()+_T("upload");
	CreateDirectory(strUpLoadPath,NULL);

	CString strCache = m_Protocol.GetAppPath()+_T("Cache");
	CreateDirectory(strCache,NULL);

	m_strSettingsPath = m_Protocol.GetAppPath()+_T("Settings.ini");
	m_Protocol.m_strSettingsPath = m_strSettingsPath;

	LoadSettings();
	m_Protocol._hMainWnd = GetSafeHwnd();

	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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
			ShowLog(_T("3次心跳无回应，准备重连……"));
			PostMessage(WM_RESTART);
			return;
		}
		m_Protocol.IncreaseHeartSendCount();
		ShowLog(_T("正在向服务器发送心跳notify......"));
		char* cpData = new char[MAX_DATA_FILE];
		ZeroMemory(cpData,MAX_DATA_FILE);
		int nDataLen = 0;
		int nRet = m_Protocol.GetHeartData(m_strNodeID, cpData, nDataLen);
		if(nRet!=0){
			CString strLog;
			strLog.Format("%d",nRet);
			ShowLog(_T("心跳notify组包失败，原因：") + strLog + _T("！"));
		}
		else{
			BOOL bSend = m_Protocol.SendData(cpData,nDataLen);
			if(!bSend)
				ShowLog(_T("心跳notify发送失败！"));
			else
				ShowLog(_T("心跳notify已发送，等待服务器回应"));
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
			ShowLog(_T("发送登陆信息失败"));
	}else{
		m_Protocol.KillSocket();
		ShowLog(_T("连接远程服务器失败！"));
	}
}

void CTransDataTestDlg::OnProcReconnect()
{
	if(!m_Protocol.SocketIsConnected() && m_bCanRunRecTimer)
	{
		m_Protocol.ResetLoginState();
		m_nUpLoadDataCount = 0;
		m_bCanRunRecTimer = false;
		ShowLog(_T("远程主机已断开连接"));
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
			ShowLog(_T("解析登陆反馈失败"));
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTransDataTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTransDataTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTransDataTestDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
	ShowLog(_T("开始登陆"));
	ShowLog(_T("正在获取证书……"));

/*	m_Protocol.BeginUpload();*/

	BYTE* data = new BYTE[MAX_DATA_FILE];
	ZeroMemory(data,MAX_DATA_FILE);
	int dataLen = 0;
	int nRet = m_Protocol.getSignerCert(data,dataLen,m_strSenderCert);
	delete[] data;
	if(nRet!=0)
	{
		CString strLog;
		strLog.Format(_T("获取证书失败, 错误代码：%d ！"),nRet);
		ShowLog(strLog);
		return;
	}
	 ShowLog(_T("获取证书成功"));
	 ShowLog(_T("正在连接本地数据库......"));

//	 bool bOpen = g_DBMgr.Init(m_strUserName,m_strPassWord,m_strDSN);
	 bool bOpen = g_DBMgr.Init();
	 if(!bOpen)
	 {
		 ShowLog(_T("连接本地数据库失败！"));
		 return;
	 }
	  ShowLog(_T("连接本地数据库成功"));

	  m_Protocol.ConnectServer();
	  //500ms后检测发送登陆信息
	  SetTimer(TIMER_LOGINDATA,500,NULL);
	  //10ms检测接收信息
	  SetTimer(TIMER_RECV,10,NULL);
	  //1s定时检测自动上传数据
	  SetTimer(TIMER_UPLOADDATA,1000,NULL);
	  //10s定时检测连接状态恢复TCP连接
	  SetTimer(TIMER_RECONNECT,10*1000,NULL);
	  //2分钟检查复传任务
	  SetTimer(TIMER_RELOAD,8*1000,NULL);

	  m_nHeartFlag = m_nHeartBeatCfgTime*60*1000;
	  //心跳
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
	ShowLog(_T("重新启动服务"));
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
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	m_Protocol.m_bUpdateTime = m_bCkBoxUpdateTime;
}



void CTransDataTestDlg::OnBnClickedButtonManulUpdate()
{
	// TODO: 在此添加控件通知处理程序代码
	CSingleLock lk(&m_ReloadSec,TRUE);

	UpdateData();
	CString strStartTime,strEndTime;

	CTime tmStart = CTime(m_tmctlStartDate.GetYear(),m_tmctlStartDate.GetMonth(),m_tmctlStartDate.GetDay(),m_tmctlStartTime.GetHour(),m_tmctlStartTime.GetMinute(),m_tmctlStartTime.GetSecond());
	CTime tmEnd = CTime(m_tmctlEndDate.GetYear(),m_tmctlEndDate.GetMonth(),m_tmctlEndDate.GetDay(),m_tmctlEndTime.GetHour(),m_tmctlEndTime.GetMinute(),m_tmctlEndTime.GetSecond());
	if(tmStart>=tmEnd)
	{
		ShowLog("请检查上传时间内容");
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
	// TODO: 在此添加控件通知处理程序代码
	m_lstInfo.ResetContent();
}


void CTransDataTestDlg::OnEnChangeEditCert()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData();
	m_strSenderCert.Remove(' ');
	UpdateData(FALSE);
}
