//
// CVideoWindow.cpp
//

/*-----------------------------------------------------*\
			HQ Tech, Make Technology Easy!       
 More information, please go to http://hqtech.nease.net.
/*-----------------------------------------------------*/

#include "stdafx.h"
#include "CVideoWindow.h"
#include <streams.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CVideoWindow::CVideoWindow()
{
//	mGraph = 0;
}

CVideoWindow::~CVideoWindow()
{
//	mGraph = 0;
}

/*
void CVideoWindow::SetGraph(IGraphBuilder * inGraph)
{
	if (mGraph != inGraph)
	{
		if (mGraph)
		{
			mGraph->SetDisplayWindow(0);
			mGraph->SetNotifyWindow(0);
		}
		mGraph = inGraph;
		if (mGraph)
		{
			mGraph->SetDisplayWindow(m_hWnd);
			mGraph->SetNotifyWindow(m_hWnd);
		}
	}
}
*/
BEGIN_MESSAGE_MAP(CVideoWindow, CWnd)
	//{{AFX_MSG_MAP(CVideoWindow)
//	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVideoWindow message handlers
LRESULT CVideoWindow::OnGraphNotify(WPARAM inWParam, LPARAM inLParam)
{
//	if (mGraph)
//	{
//		mGraph->HandleEvent(inWParam, inLParam);
//	}
	return 0;
}

// Modify the video window style. Furthermore, remember to override
// the Main dialog's WM_ERASEBKGND message procedure, as the following:
/*
BOOL CMainDlg::OnEraseBkgnd(CDC* pDC) // ON_WM_ERASEBKGND()
{
	// Intercept background erasing for the movie window, since the
    // video renderer will keep the screen painted.  Without this code,
    // your video window might get painted over with gray (the default
    // background brush) when it is obscured by another window and redrawn.
    CRect rc;
    // Get the bounding rectangle for the movie screen
    mVideoWnd.GetWindowRect(&rc);
    ScreenToClient(&rc);
    // Exclude the clipping region occupied by our movie screen
    pDC->ExcludeClipRect(&rc);

	// Erase the remainder of the dialog as usual
    return CDialog::OnEraseBkgnd(pDC);
}
*/
void CVideoWindow::PreSubclassWindow(void)
{
	// Since we're embedding video in a child window of a dialog,
    // we must set the WS_CLIPCHILDREN style to prevent the bounding
    // rectangle from drawing over our video frames.
	ModifyStyle(0, WS_CLIPCHILDREN);

	CWnd::PreSubclassWindow();
}