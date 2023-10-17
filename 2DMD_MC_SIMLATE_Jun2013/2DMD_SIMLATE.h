// 2DMD_SIMLATE.h : main header file for the 2DMD_SIMLATE application
//

#if !defined(AFX_2DMD_SIMLATE_H__2AADCE1F_773A_4DB0_894A_5BDA91235E44__INCLUDED_)
#define AFX_2DMD_SIMLATE_H__2AADCE1F_773A_4DB0_894A_5BDA91235E44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMy2DMD_SIMLATEApp:
// See 2DMD_SIMLATE.cpp for the implementation of this class
//

class CMy2DMD_SIMLATEApp : public CWinApp
{
public:
	CMy2DMD_SIMLATEApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy2DMD_SIMLATEApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMy2DMD_SIMLATEApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DMD_SIMLATE_H__2AADCE1F_773A_4DB0_894A_5BDA91235E44__INCLUDED_)
