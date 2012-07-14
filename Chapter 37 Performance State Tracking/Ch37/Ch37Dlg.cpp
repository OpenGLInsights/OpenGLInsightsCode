
// Ch37Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Ch37.h"
#include "Ch37Dlg.h"
#include "PStateNV.h"
#include "PStateAMD.h"

#include "GL.h"
#pragma comment(lib,"opengl32.lib")

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


// CCh37Dlg dialog




CCh37Dlg::CCh37Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCh37Dlg::IDD, pParent)
	, m_sVendor(_T(""))
	, m_nPStateID(0)
	, m_nGPUClock(0)
	, m_nMemClock(0)
	, m_nShaderClock(0)
	, m_nCoreVoltage(0)
	, m_nGPUUtil(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCh37Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_sVendor);
	DDX_Text(pDX, IDC_EDIT2, m_nPStateID);
	DDX_Text(pDX, IDC_EDIT3, m_nGPUClock);
	DDX_Text(pDX, IDC_EDIT4, m_nMemClock);
	DDX_Text(pDX, IDC_EDIT5, m_nShaderClock);
	DDX_Text(pDX, IDC_EDIT6, m_nCoreVoltage);
	DDX_Text(pDX, IDC_EDIT7, m_nGPUUtil);
}

BEGIN_MESSAGE_MAP(CCh37Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
//	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CCh37Dlg message handlers

BOOL CCh37Dlg::OnInitDialog()
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

	// TODO: Add extra initialization here
	
	CheckVendor();
	m_pPState = NULL;
	if(m_VENDOR == NVIDIA)
		m_pPState = new PStateNV();
	else if(m_VENDOR == AMD)
		m_pPState = new PStateAMD();

	SetTimer(TIMER_REFRESH, 500, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCh37Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCh37Dlg::OnPaint()
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
HCURSOR CCh37Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCh37Dlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == TIMER_REFRESH)
	{
		if(m_pPState)
		{
			PStateStruct state;
			m_pPState->GetCurrentPState(state);
			m_nPStateID = state.ID;
			m_nGPUClock = state.gpuClock;
			m_nMemClock = state.memClock;
			m_nShaderClock = state.shaderClock;
			m_nCoreVoltage = state.coreVoltage;
			m_nGPUUtil = state.gpuUtil;
			UpdateData(FALSE);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CCh37Dlg::CheckVendor()
{
	char* ven = "Other";

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
			HGLRC tempContext = wglCreateContext(pDC->m_hDC); 
			wglMakeCurrent(pDC->m_hDC,tempContext);

			ven = (char*)glGetString(GL_VENDOR);
			m_sVendor = CString(ven);
			wglMakeCurrent(NULL,NULL);
			wglDeleteContext(tempContext);
		}
   	}   
	ReleaseDC(pDC);
	
	if(ven[0] == 'N')
		m_VENDOR = NVIDIA;
	else if(ven[0] == 'A')
		m_VENDOR = AMD;
	else if(ven[0] == 'I')
		m_VENDOR = INTEL;
	else
		m_VENDOR = OTHER;
}

void CCh37Dlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_pPState) delete m_pPState;
}
