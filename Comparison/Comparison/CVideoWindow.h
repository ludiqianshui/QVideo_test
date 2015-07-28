//
// CVideoWindow.h
//

#ifndef __H_CVideoWindow__
#define __H_CVideoWindow__

#include "streams.h"
//class CDXGraph;
class CVideoWindow : public CWnd
{
protected:
//	CDXGraph *		mGraph;

public:
//	void SetGraph(IGraphBuilder * inGraph);

// Construction
public:
	CVideoWindow();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVideoWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVideoWindow)
	afx_msg LRESULT OnGraphNotify(WPARAM inWParam, LPARAM inLParam);
	virtual void PreSubclassWindow(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __H_CVideoWindow__
