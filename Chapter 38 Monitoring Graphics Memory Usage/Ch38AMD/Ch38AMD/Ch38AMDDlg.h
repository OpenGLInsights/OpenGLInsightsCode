
// Ch38AMDDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "GL.h"
#include "afxwin.h"

// CCh38AMDDlg dialog
class CCh38AMDDlg : public CDialog
{
// Construction
public:
	CCh38AMDDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CH38AMD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	HGLRC m_GLRC;
	unsigned int	GetAmdTotalMem();
	void			GetAmdMemoryPoolStatus(int pool, int& dedicatedFree, int& dedicatedFreeBlock, int& auxFree, int& auxFreeBlock);
	void			GetAmdMemUtil(BYTE& memUtil, BYTE& auxUtil);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	GLint m_nMaxAmdDedMem;
	GLint m_nMaxAmdAuxMem;
	GLint m_nDedicatedFreeSize;
	GLint m_nAuxiliaryFreeSize;
	GLint m_nDedicatedMaxFreeBlock;
	GLint m_nAuxiliaryMaxFreeBlock;
	/*GLint m_nEvCount;
	GLint m_nEvMem;*/
	BYTE m_nDedicatedUtil;
	BYTE m_nAuxiliaryUtil;
	CProgressCtrl m_ctrlProgressDedUtil;
	CProgressCtrl m_ctrlProgressAuxUtil;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CComboBox m_ctrlPoolCombo;
	//CString m_sMemPool;
	CString m_sDedUtil;
	CString m_sAuxUtil;
};
