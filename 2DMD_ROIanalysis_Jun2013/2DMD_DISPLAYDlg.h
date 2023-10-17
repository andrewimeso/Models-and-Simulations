// 2DMD_DISPLAYDlg.h : header file
//

#if !defined(AFX_2DMD_DISPLAYDLG_H__B8D0F9F1_43E7_4FBA_9976_B17F8E518264__INCLUDED_)
#define AFX_2DMD_DISPLAYDLG_H__B8D0F9F1_43E7_4FBA_9976_B17F8E518264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageObject.h"

/////////////////////////////////////////////////////////////////////////////
//  functions added by JMZ & AM MAY 2009

int DoCheckFile(char *);
int DoLoadFile(int, int, int, char *, int *, int *);
int ImageRegion(int, int, int);
void TimeAvrg();

/////////////////////////////////////////////////////////////////////////////
// CMy2DMD_DISPLAYDlg dialog

class CMy2DMD_DISPLAYDlg : public CDialog
{
// Construction
public:
	CMy2DMD_DISPLAYDlg(CWnd* pParent = NULL);	// standard constructor
	CImageObject * l_pImageObject;
	CImageObject * m_pImageObject;
	CImageObject * n_pImageObject;

// Dialog Data
	//{{AFX_DATA(CMy2DMD_DISPLAYDlg)
	enum { IDD = IDD_MY2DMD_DISPLAY_DIALOG };
	CString	m_in_inputname;
	CString	m_in_intensgain;
	CString	m_in_intenshift;
	CString m_in_status_hist;
	int		m_hist_type;
	int		m_in_firstn;
	int		m_in_lastn;
	int		m_out_circ;
	int		m_in_circ;
	CString	m_in_threshold;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy2DMD_DISPLAYDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMy2DMD_DISPLAYDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeINinputname();
	afx_msg void OnChangeINgain();
	afx_msg void OnChangeINshift();
	afx_msg void OnLoadresp();
	afx_msg void OnSegresp();
	afx_msg void OnShowHist();
	afx_msg void OnCalcHist();
	afx_msg void OnChangeINfirst();
	afx_msg void OnChangeINlast();
	afx_msg void OnAverresp();
	afx_msg void OnChangeINthresh();
	afx_msg void OnChangeINhisttype();
	afx_msg void OnChangeINoutcirc();
	afx_msg void OnChangeINincirc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCalccombh();
	afx_msg void OnBnClickedShowcombh();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DMD_DISPLAYDLG_H__B8D0F9F1_43E7_4FBA_9976_B17F8E518264__INCLUDED_)
