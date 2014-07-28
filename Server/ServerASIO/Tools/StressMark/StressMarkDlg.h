//////////////////////////////////////////////////////////////////
//
// StressMarkDlg.h : 主对话框类的头文件
//
// created by Wang Yong Gang, 2001-08-07 ( version 1.0 )
// modified by Wang Yong Gang, 2002-02-16 ( version 1.1 )
//
//////////////////////////////////////////////////////////////////

#if !defined(AFX_STRESSMARKDLG_H__46DF8932_2F96_4E3E_8D1C_8CB94B180818__INCLUDED_)
#define AFX_STRESSMARKDLG_H__46DF8932_2F96_4E3E_8D1C_8CB94B180818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CStressMarkDlg dialog

#define WM_USER_ALL_FINISHED	(WM_USER+1)

class CStressMarkDlg : public CDialog
{
// Construction
public:
	void UpdateButtonState();
	CStressMarkDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CStressMarkDlg)
	enum { IDD = IDD_STRESSMARK_DIALOG };
	CEdit	m_clock;
	CListCtrl	m_list;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStressMarkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CFont m_fontClock;

	// 测试管理对象
	StressMan m_StressMan;	

	// 当前测试状态的定义
	static enum
	{
		ready,		// 就绪
		starting,	// 启动中
		running,	// 运行
		stopping	// 停止中
	};

	// 当前测试状态
	int m_nState;

	// Generated message map functions
	//{{AFX_MSG(CStressMarkDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnStop();
	afx_msg void OnReport();
	afx_msg LRESULT OnAllFinished(WPARAM, LPARAM);
	afx_msg void OnKill();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRESSMARKDLG_H__46DF8932_2F96_4E3E_8D1C_8CB94B180818__INCLUDED_)
