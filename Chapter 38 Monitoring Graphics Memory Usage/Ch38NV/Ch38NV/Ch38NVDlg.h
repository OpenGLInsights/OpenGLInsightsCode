
// Ch38NVDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "GL.h"

// CCh38NVDlg dialog
class CCh38NVDlg : public CDialog
{
// Construction
public:
	CCh38NVDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CH38NV_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	HGLRC m_GLRC;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	GLint m_nSize;
	GLint m_nMaxAvail;
	GLint m_nCurAvail;
	GLint m_nEvCount;
	GLint m_nEvMem;
	BYTE m_nUtil;
	CProgressCtrl m_ctrlProgressUtil;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
