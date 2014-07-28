/////////////////////////////////////////////////////////////
//
// StressMarkDlg.cpp : 主对话框类的实现代码
//
// created by Wang Yong Gang, 2001-08-07 ( version 1.0 )
// modified by Wang Yong Gang, 2002-02-16 ( version 1.1 )
//
/////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StressMark.h"

#include "StressCase.h"
#include "StressMan.h"
#include "StressMarkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStressMarkDlg dialog

CStressMarkDlg::CStressMarkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStressMarkDlg::IDD, pParent), m_StressMan(m_list)
{
	//{{AFX_DATA_INIT(CStressMarkDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStressMarkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStressMarkDlg)
	DDX_Control(pDX, IDC_CLOCK, m_clock);
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStressMarkDlg, CDialog)
	//{{AFX_MSG_MAP(CStressMarkDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_REPORT, OnReport)
	ON_MESSAGE(WM_USER_ALL_FINISHED, OnAllFinished)    
	ON_BN_CLICKED(IDC_KILL, OnKill)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStressMarkDlg message handlers

BOOL CStressMarkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	if (!m_StressMan.InitializeCaseList())
	{
		// 如果初始化失败，则退出程序
		EndDialog(0);
		return TRUE;
	}

	if (!m_fontClock.CreateFont(-13, 0, 0, 0, FW_BOLD, 0, 
		0, 0, 0, 0, 0, 0, 0, "Arial"))
	{
		// 如果创建字体失败，则退出程序
		EndDialog(0);
		return TRUE;
	}

	SetTimer(1001, 1000, NULL);
	m_clock.SetFont(&m_fontClock, FALSE);
	m_clock.SetWindowText(_T("StressMark"));
	m_nState = ready;
	UpdateButtonState();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CStressMarkDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStressMarkDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CStressMarkDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CStressMarkDlg::OnClose() 
{
	if (m_nState == ready)	
		CDialog::OnClose();
}

void CStressMarkDlg::OnTimer(UINT nIDEvent) 
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	TCHAR temp[100];
	wsprintf(temp, _T("%04d-%02d-%02d %02d:%02d:%02d"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	m_clock.SetWindowText(temp);

	CDialog::OnTimer(nIDEvent);
}

void CStressMarkDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	KillTimer(1001);
	m_list.DeleteAllItems();
}

void CStressMarkDlg::UpdateButtonState()
{
	switch(m_nState)
	{
	case ready:
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_KILL)->EnableWindow(FALSE);
		GetDlgItem(IDC_REPORT)->EnableWindow(TRUE);
		break;
	case starting:
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_KILL)->EnableWindow(FALSE);
		GetDlgItem(IDC_REPORT)->EnableWindow(FALSE);
		break;
	case running:	
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_KILL)->EnableWindow(FALSE);
		GetDlgItem(IDC_REPORT)->EnableWindow(FALSE);
		break;	
	case stopping:
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_KILL)->EnableWindow(TRUE);
		GetDlgItem(IDC_REPORT)->EnableWindow(FALSE);
		break;	
	}
}

void CStressMarkDlg::OnOK() 
{
	// 启动
	m_nState = starting;
	UpdateButtonState();	
	m_StressMan.StartSuite();
	m_nState = running;
	UpdateButtonState();	
}

void CStressMarkDlg::OnStop() 
{
	// 强制停止
	m_nState = stopping;
	UpdateButtonState();	

	m_StressMan.StopSuite();		
}

void CStressMarkDlg::OnCancel() 
{
	// 退出
	if (m_nState == ready && 
			AfxMessageBox(_T("现在退出？"), MB_YESNO) == IDYES )
		CDialog::OnCancel();	
}

void CStressMarkDlg::OnReport() 
{
	// 报表
	CFileDialog dlg(FALSE);
	if (dlg.DoModal() == IDOK)
		m_StressMan.ReportSuite(dlg.GetPathName());
}

LRESULT CStressMarkDlg::OnAllFinished(WPARAM, LPARAM)
{
	// 所有的线程都已执行完毕
	m_nState = ready;
	UpdateButtonState();

	return 0;
}

void CStressMarkDlg::OnKill() 
{
	// 无法终止的情况下杀线程
	m_StressMan.KillSuite();

	m_nState = ready;
	UpdateButtonState();
}
