// StressMark.h : main header file for the STRESSMARK application
//

#if !defined(AFX_STRESSMARK_H__45FDDE77_1FE5_47DE_A509_18DEBB5C4607__INCLUDED_)
#define AFX_STRESSMARK_H__45FDDE77_1FE5_47DE_A509_18DEBB5C4607__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CStressMarkApp:
// See StressMark.cpp for the implementation of this class
//

class CStressMarkApp : public CWinApp
{
public:
	CStressMarkApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStressMarkApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CStressMarkApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRESSMARK_H__45FDDE77_1FE5_47DE_A509_18DEBB5C4607__INCLUDED_)
