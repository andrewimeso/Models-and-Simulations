// 2DMD_GENPAT.h : main header file for the 2DMD_GENPAT application
//

#if !defined(AFX_2DMD_GENPAT_H__43785963_1430_4A88_B1DF_152D0FB7C88A__INCLUDED_)
#define AFX_2DMD_GENPAT_H__43785963_1430_4A88_B1DF_152D0FB7C88A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMy2DMD_GENPATApp:
// See 2DMD_GENPAT.cpp for the implementation of this class
//

class CMy2DMD_GENPATApp : public CWinApp
{
public:
	CMy2DMD_GENPATApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy2DMD_GENPATApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMy2DMD_GENPATApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DMD_GENPAT_H__43785963_1430_4A88_B1DF_152D0FB7C88A__INCLUDED_)
