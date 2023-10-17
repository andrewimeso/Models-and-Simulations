// 2DMD_DISPLAY.h : main header file for the 2DMD_DISPLAY application
//

#if !defined(AFX_2DMD_DISPLAY_H__ABF27830_C379_4B4E_A331_14547F7159A1__INCLUDED_)
#define AFX_2DMD_DISPLAY_H__ABF27830_C379_4B4E_A331_14547F7159A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMy2DMD_DISPLAYApp:
// See 2DMD_DISPLAY.cpp for the implementation of this class
//

class CMy2DMD_DISPLAYApp : public CWinApp
{
public:
	CMy2DMD_DISPLAYApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy2DMD_DISPLAYApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMy2DMD_DISPLAYApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DMD_DISPLAY_H__ABF27830_C379_4B4E_A331_14547F7159A1__INCLUDED_)
