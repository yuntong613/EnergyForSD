
// TransDataTestDlg.h : ͷ�ļ�
//

#pragma once

#include "ProtocolManager.h"
#include "atltime.h"

// CTransDataTestDlg �Ի���
class CTransDataTestDlg : public CDialogEx
{
// ����
public:
	CTransDataTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TRANSDATATEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected://����
	int m_nHeartFlag;//����ʱ����
	void OnProcHeartBeat();//��������
protected:	
	//��ʱ����½����
	void OnProcLogin();
	//��ʱ��������������
	void OnProcReconnect();
	//��ʱ���Զ��ϴ����ݰ�����
	void OnProcLoadData();
	//��ʱ�����մ���
	void OnProcRecv();
	//��ʱ��������
	void OnProcReload();
public:
	//�Զ��ϴ���ʱ
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
public://�����ĵ�
	void LoadSettings();
	void SaveSettings();
	CString m_strSettingsPath;
public://������Ϣ
	CString m_strRemoteIP;
	int m_nRemotePort;
	int m_nHeartBeatCfgTime;
	CString m_strNodeID;
	CString m_strDSN;
	CString m_strUserName;
	CString m_strPassWord;
	CString m_strSenderCert;
public://��־���
	void ShowLog(CString strMsg);
	CListBox m_lstInfo;
public:
	bool m_bCanRunRecTimer;//�����ж�
public:
	CProtocolManager m_Protocol;
	LRESULT OnShowMsg(WPARAM wParam,LPARAM lParam);
	LRESULT OnRestartMsg(WPARAM wParam,LPARAM lParam);
	BOOL m_bCkBoxUpdateTime;
public:
	//���±༭��UI
	void UpdateUI(BOOL bReadOnly = TRUE);
	afx_msg void OnBnClickedButtonManulUpdate();
	CTime m_tmctlStartDate;
	CTime m_tmctlEndDate;
	CTime m_tmctlStartTime;
	CTime m_tmctlEndTime;
	afx_msg void OnBnClickedButtonClear();
};
