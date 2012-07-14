
// Ch38AMDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Ch38AMD.h"
#include "Ch38AMDDlg.h"
#include "wglext.h"
#pragma comment(lib,"opengl32.lib")

#ifndef GL_VBO_FREE_MEMORY_ATI

	#define GL_VBO_FREE_MEMORY_ATI			0x87FB
	#define GL_TEXTURE_FREE_MEMORY_ATI		0x87FC
	#define GL_RENDERBUFFER_FREE_MEMORY_ATI	0x87FD

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


// CCh38AMDDlg dialog




CCh38AMDDlg::CCh38AMDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCh38AMDDlg::IDD, pParent)
	, m_nMaxAmdDedMem(0)
	, m_nMaxAmdAuxMem(0)
	, m_nDedicatedFreeSize(0)
	, m_nAuxiliaryFreeSize(0)
	, m_nDedicatedMaxFreeBlock(0)
	, m_nAuxiliaryMaxFreeBlock(0)
	, m_sDedUtil(_T(""))
	, m_sAuxUtil(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCh38AMDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nDedicatedFreeSize);
	DDX_Text(pDX, IDC_EDIT2, m_nDedicatedMaxFreeBlock);
	DDX_Text(pDX, IDC_EDIT3, m_nAuxiliaryFreeSize);
	DDX_Text(pDX, IDC_EDIT4, m_nAuxiliaryMaxFreeBlock);
	DDX_Text(pDX, IDC_EDIT6, m_nMaxAmdDedMem);
	DDX_Text(pDX, IDC_EDIT7, m_nMaxAmdAuxMem);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgressDedUtil);
	DDX_Control(pDX, IDC_PROGRESS2, m_ctrlProgressAuxUtil);
	DDX_Control(pDX, IDC_COMBO1, m_ctrlPoolCombo);
	DDX_Text(pDX, IDC_DED_UTIL, m_sDedUtil);
	DDX_Text(pDX, IDC_AUX_UTIL, m_sAuxUtil);
}

BEGIN_MESSAGE_MAP(CCh38AMDDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCh38AMDDlg message handlers

BOOL CCh38AMDDlg::OnInitDialog()
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

	m_ctrlPoolCombo.AddString(_T("VBO"));
	m_ctrlPoolCombo.AddString(_T("TEXTURE"));
	m_ctrlPoolCombo.AddString(_T("FRAMEBUFFER"));
	m_ctrlPoolCombo.SetCurSel(0);
	m_nMaxAmdDedMem = 1024 * GetAmdTotalMem(); // GetAmdTotalMem() returns mem size in MB
	GetAmdMemoryPoolStatus(0, m_nDedicatedFreeSize, m_nDedicatedMaxFreeBlock, m_nAuxiliaryFreeSize, m_nAuxiliaryMaxFreeBlock);
	m_nMaxAmdAuxMem = m_nAuxiliaryFreeSize;
	SetTimer(TIMER_REFRESH, 500, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCh38AMDDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCh38AMDDlg::OnPaint()
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
HCURSOR CCh38AMDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


int CCh38AMDDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
			
		}
   	}   
	ReleaseDC(pDC);


	return 0;
}

void CCh38AMDDlg::OnDestroy()
{
	CDialog::OnDestroy();

	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(m_GLRC);
}

void CCh38AMDDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == TIMER_REFRESH)
	{
		int ind = m_ctrlPoolCombo.GetCurSel();
		GetAmdMemoryPoolStatus(ind, m_nDedicatedFreeSize, m_nDedicatedMaxFreeBlock, m_nAuxiliaryFreeSize, m_nAuxiliaryMaxFreeBlock);
		GetAmdMemUtil(m_nDedicatedUtil, m_nAuxiliaryUtil);
		m_ctrlProgressDedUtil.SetPos(m_nDedicatedUtil);
		m_ctrlProgressAuxUtil.SetPos(m_nAuxiliaryUtil);
		m_sDedUtil.Format(_T("%d%%"),m_nDedicatedUtil);
		m_sAuxUtil.Format(_T("%d%%"),m_nAuxiliaryUtil);
		UpdateData(FALSE);
	}

	CDialog::OnTimer(nIDEvent);
}

unsigned int CCh38AMDDlg::GetAmdTotalMem()
{
	UINT maxCount;
	UINT* ID;
	size_t memTotal = 0;
	PFNWGLGETGPUIDSAMDPROC wglGetGPUIDsAMD = (PFNWGLGETGPUIDSAMDPROC) wglGetProcAddress("wglGetGPUIDsAMD");
	if(wglGetGPUIDsAMD)
	{
		maxCount = wglGetGPUIDsAMD(0, 0);
		ID = new UINT[maxCount];
		wglGetGPUIDsAMD(maxCount, ID);
		PFNWGLGETGPUINFOAMDPROC wglGetGPUInfoAMD = (PFNWGLGETGPUINFOAMDPROC) wglGetProcAddress("wglGetGPUInfoAMD");
		if(wglGetGPUInfoAMD)
			wglGetGPUInfoAMD(ID[0], WGL_GPU_RAM_AMD, GL_UNSIGNED_INT, sizeof(size_t), & memTotal);
	}
	return (unsigned int)memTotal;
}

void CCh38AMDDlg::GetAmdMemoryPoolStatus(int pool, int& dedicatedFree, int& dedicatedFreeBlock, int& auxFree, int& auxFreeBlock)
{
	/*	• param[0] - total memory free in kB,
		• param[1] - largest available free block in kB,
		• param[2] - total auxiliary memory free in kB and
		• param[3] - largest auxiliary free block in kB.
	*/

	// pool = 0 (VBO), 1 (Tetxture), 2 (Renderbuffer)
	GLint param[4] = {0,0,0,0};
	if(pool == 0)
		glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, param);
	else if(pool == 1)
		glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, param);
	else if(pool == 2)
		glGetIntegerv(GL_RENDERBUFFER_FREE_MEMORY_ATI, param);

	dedicatedFree		= param[0];
	dedicatedFreeBlock	= param[1];
	auxFree				= param[2];
	auxFreeBlock		= param[3];
}

void CCh38AMDDlg::GetAmdMemUtil(BYTE& memUtil, BYTE& auxUtil)
{
	int param[4];
	glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, param);
	int memAvailable = param[0];
	int auxFree = param[2];
	memUtil = (BYTE)(0.5+100.0f*(float)(m_nMaxAmdDedMem-memAvailable)/(float)m_nMaxAmdDedMem);
	auxUtil = (BYTE)(0.5+100.0f*(float)(m_nMaxAmdAuxMem-auxFree)/(float)m_nMaxAmdAuxMem);
}