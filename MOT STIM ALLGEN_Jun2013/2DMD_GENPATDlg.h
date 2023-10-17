// 2DMD_GENPATDlg.h : header file
//
#if !defined(AFX_2DMD_GENPATDLG_H__47AB02B4_97A2_457F_8A61_D9C28B2C3081__INCLUDED_)
#define AFX_2DMD_GENPATDLG_H__47AB02B4_97A2_457F_8A61_D9C28B2C3081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageObject.h"

/////////////////////////////////////////////////////////////////////////////
//  functions added by JMZ 
int GreyVal(double);

/////////////////////////////////////////////////////////////////////////////
// CMy2DMD_GENPATDlg dialog

class CMy2DMD_GENPATDlg : public CDialog
{
// Construction
public:
	CMy2DMD_GENPATDlg(CWnd* pParent = NULL);	// standard constructor
	CImageObject * m_pImageObject;

// Dialog Data
	//{{AFX_DATA(CMy2DMD_GENPATDlg)
	enum { IDD = IDD_MY2DMD_GENPAT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy2DMD_GENPATDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMy2DMD_GENPATDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGendots();
	afx_msg void OnGensegments();
	afx_msg void OnGenwnoise();
	afx_msg void OnGenpplaids();
	afx_msg void OnGenreplaids();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	// horizontal image size
	CString m_in_sizeX;
	afx_msg void OnEnChangeInsizex();
	// vertical image size
	CString m_in_sizeY;
	afx_msg void OnEnChangeInsizey();
	// number of rames in sequence 
	CString m_in_numFrames;
	afx_msg void OnEnChangeInnumframes();
	// // stem of file name
	CString m_in_filename;
	CString m_in_progress;
	afx_msg void OnEnChangeInfilename();
	// horizontal displacement
	CString m_in_disp; CString m_in_disp2;
	afx_msg void OnEnChangeIndispx();
	// vertical displacement
	CString m_in_dir;
	afx_msg void OnEnChangeIndsipy();
	// number of dots in frame
	int m_in_numDots;
	afx_msg void OnEnChangeNumdots();
	// number of motion directions
	int m_in_numDirs;
	afx_msg void OnEnChangeNumdirs();
	afx_msg void OnBnClickedGendots();
	// direction difference for multiple directions
	CString m_in_dirdiff; 
	int m_in_dotsiz; 
	CString m_in_contype;
	CString m_in_m_amp;
	CString m_in_freq_mod;
	CString m_in_freq_carr;
	CString m_in_modtype;
	afx_msg void OnEnChangeIndirdiff();
		
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DMD_GENPATDLG_H__47AB02B4_97A2_457F_8A61_D9C28B2C3081__INCLUDED_)
