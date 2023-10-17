// 2DMD_SIMLATEDlg.h : header file
//

#if !defined(AFX_2DMD_SIMLATEDLG_H__A71B675A_8C57_4EC5_AD0D_0FBFD54BDD17__INCLUDED_)
#define AFX_2DMD_SIMLATEDLG_H__A71B675A_8C57_4EC5_AD0D_0FBFD54BDD17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageObject.h"

/////////////////////////////////////////////////////////////////////////////
//  functions added by JMZ 

void	DoCheckinput(void);
void	DoLoadinput(int, int);
void	DoCopy3Dto2D(unsigned short int *, unsigned short int *, int, int);	
void	DoCopy2Dto3D(unsigned short int *, unsigned short int *, int, int);	
void	DoFilterArray(int, int, int, int, double);	
void	DoNonlinArray(int, int, int);	
void	SaveFilteredArray(int, int, int, int);	
void	DoMotionArrays(int, int, int, int);
void	DoLowPass(double *, double *, int, double);
void	SaveMotionArrays(int, int)	;

int		ImageRegion(int, int, int, int);	
double	MakeFilter(int, double, double);	

/////////////////////////////////////////////////////////////////////////////
// CMy2DMD_SIMLATEDlg dialog

class CMy2DMD_SIMLATEDlg : public CDialog
{
// Construction
public:
	CMy2DMD_SIMLATEDlg(CWnd* pParent = NULL);	// standard constructor
	CImageObject * m_pImageObject;
	CImageObject * n_pImageObject;

// Dialog Data
	//{{AFX_DATA(CMy2DMD_SIMLATEDlg)
	enum { IDD = IDD_MY2DMD_SIMLATE_DIALOG };
	CString	m_in_filename;
	int		m_in_first_n;
	int		m_in_last_n;
	CString	m_in_emdgain;
	CString	m_in_filtergain;
	CString	m_in_emddist;
	CString	m_in_emdtimc;
	CString	m_in_emddist2;
	CString	m_in_emdtimc2;
	CString	m_in_emddist3;
	CString	m_in_emdtimc3;
	CString	m_in_emddist4;
	CString	m_in_emdtimc4;
	CString	m_in_edgethr;
	int		m_in_preproc;
	CString	m_in_sizeratio;
	CString	m_in_balanc;
	CString	m_in_cntramp;
	CString m_in_status_load;
	CString m_in_status_filt;
	CString m_in_status_mot;
	CString m_in_status_comb;
	int		m_direction;
	int		m_max_norm;
	int		m_start_x;
	int		m_start_y;
	int		m_end_x;
	int		m_end_y;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy2DMD_SIMLATEDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMy2DMD_SIMLATEDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLoadinput();
	virtual void OnCancel();
	afx_msg void OnChangeInFilename();
	afx_msg void OnChangeInFirstN();
	afx_msg void OnChangeInLastN();
	afx_msg void OnFilter();
	afx_msg void OnMotion();
	afx_msg void OnChangeInGainEMD();
	afx_msg void OnChangeInGainDOG();
	afx_msg void OnChangeInTimcEMD();
	afx_msg void OnChangeInDistEMD();
	afx_msg void OnChangeInPreproNL();
	afx_msg void OnChangeInSizeDOG();
	afx_msg void OnChangeInCentAmpDOG();
	afx_msg void OnChangeInBalanEMD();
	afx_msg void OnSimulate();
	afx_msg void OnCancelMode();
	afx_msg void OnDisplay();
	afx_msg void OnChangeInDirection();
	afx_msg void OnChangeInMaxNorm();
	afx_msg void OnChangeInStartX();
	afx_msg void OnChangeInStartY();
	afx_msg void OnChangeInEndX();
	afx_msg void OnChangeInEndY();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DMD_SIMLATEDLG_H__A71B675A_8C57_4EC5_AD0D_0FBFD54BDD17__INCLUDED_)
