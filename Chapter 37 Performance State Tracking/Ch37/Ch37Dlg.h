
// Ch37Dlg.h : header file
//

#pragma once

#define NVIDIA	1
#define AMD		2
#define INTEL	3
#define OTHER	4

class PState;

// CCh37Dlg dialog
class CCh37Dlg : public CDialog
{
// Construction
public:
	CCh37Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CH37_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	PState* m_pPState;
	int m_VENDOR;
	void CheckVendor();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_sVendor;
	UINT m_nPStateID;
	UINT m_nGPUClock;
	UINT m_nMemClock;
	UINT m_nShaderClock;
	UINT m_nCoreVoltage;
	UINT m_nGPUUtil;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnClose();
protected:
//	virtual void OnOK();
public:
	afx_msg void OnDestroy();
};
