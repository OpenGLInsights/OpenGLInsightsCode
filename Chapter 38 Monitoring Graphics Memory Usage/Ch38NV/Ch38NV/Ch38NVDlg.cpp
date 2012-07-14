
// Ch38NVDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Ch38NV.h"
#include "Ch38NVDlg.h"
#pragma comment(lib,"opengl32.lib")

#ifndef GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX

	#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
	#define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX 0x9048
	#define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049
	#define GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX 0x904A
	#define GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX 0x904B

#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_REFRESH 1001

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCh38NVDlg dialog




CCh38NVDlg::CCh38NVDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCh38NVDlg::IDD, pParent)
	, m_nSize(0)
	, m_nMaxAvail(0)
	, m_nCurAvail(0)
	, m_nEvCount(0)
	, m_nEvMem(0)
	, m_nUtil(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCh38NVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nSize);
	DDX_Text(pDX, IDC_EDIT2, m_nMaxAvail);
	DDX_Text(pDX, IDC_EDIT3, m_nCurAvail);
	DDX_Text(pDX, IDC_EDIT4, m_nEvCount);
	DDX_Text(pDX, IDC_EDIT5, m_nEvMem);
	DDX_Text(pDX, IDC_EDIT6, m_nUtil);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgressUtil);
}

BEGIN_MESSAGE_MAP(CCh38NVDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCh38NVDlg message handlers

BOOL CCh38NVDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetTimer(TIMER_REFRESH, 500, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCh38NVDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCh38NVDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCh38NVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


int CCh38NVDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	PIXELFORMATDESCRIPTOR pfd ;
   	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
   	pfd.nVersion   = 1; 
   	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   
   	pfd.iPixelType = PFD_TYPE_RGBA; 
   	pfd.cColorBits = 32;
   	pfd.cDepthBits = 24; 
   	pfd.iLayerType = PFD_MAIN_PLANE;
	CDC* pDC = GetDC();
	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);

	if (nPixelFormat != 0)
   	{
		BOOL bResult = SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd);
		if(bResult)
		{
			m_GLRC = wglCreateContext(pDC->m_hDC); 
			wglMakeCurrent(pDC->m_hDC,m_GLRC);

			glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &m_nSize);
			glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &m_nMaxAvail);
			
		}
   	}   
	ReleaseDC(pDC);


	return 0;
}

void CCh38NVDlg::OnDestroy()
{
	CDialog::OnDestroy();

	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(m_GLRC);
}

void CCh38NVDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == TIMER_REFRESH)
	{
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &m_nCurAvail);
		glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX, &m_nEvCount);
		glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &m_nEvMem);
		m_nUtil = 100*(m_nMaxAvail-m_nCurAvail)/m_nMaxAvail;
		m_ctrlProgressUtil.SetPos(m_nUtil);
		UpdateData(FALSE);
	}

	CDialog::OnTimer(nIDEvent);
}
