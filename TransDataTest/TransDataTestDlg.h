
// TransDataTestDlg.h : 头文件
//

#pragma once

#include "ProtocolManager.h"
#include "atltime.h"

// CTransDataTestDlg 对话框
class CTransDataTestDlg : public CDialogEx
{
// 构造
public:
	CTransDataTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TRANSDATATEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected://心跳
	int m_nHeartFlag;//心跳时间间隔
	void OnProcHeartBeat();//心跳报文
protected:	
	//定时器登陆处理
	void OnProcLogin();
	//定时器网络重连处理
	void OnProcReconnect();
	//定时器自动上传数据包处理
	void OnProcLoadData();
	//定时器接收处理
	void OnProcRecv();
	//定时复传处理
	void OnProcReload();
public:
	//自动上传计时
	int m_nUpLoadDataCount;
public:
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedCheckUpdatetime();
public:
	void Start();
	void Stop();
public://配置文档
	void LoadSettings();
	void SaveSettings();
	CString m_strSettingsPath;
public://配置信息
	CString m_strRemoteIP;
	int m_nRemotePort;
	int m_nHeartBeatCfgTime;
	CString m_strNodeID;
	CString m_strDSN;
	CString m_strUserName;
	CString m_strPassWord;
	CString m_strSenderCert;
public://日志输出
	void ShowLog(CString strMsg);
	CListBox m_lstInfo;
public:
	bool m_bCanRunRecTimer;//重连判断
public:
	CProtocolManager m_Protocol;
	LRESULT OnShowMsg(WPARAM wParam,LPARAM lParam);
	LRESULT OnRestartMsg(WPARAM wParam,LPARAM lParam);
	BOOL m_bCkBoxUpdateTime;
public:
	//更新编辑框UI
	void UpdateUI(BOOL bReadOnly = TRUE);
	afx_msg void OnBnClickedButtonManulUpdate();
	CTime m_tmctlStartDate;
	CTime m_tmctlEndDate;
	CTime m_tmctlStartTime;
	CTime m_tmctlEndTime;
	afx_msg void OnBnClickedButtonClear();
};
