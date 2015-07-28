#if !defined(AFX_COMPARISONDLG_H__A1EB3165_5223_45A0_8651_EDF9391F8894__INCLUDED_)
#define AFX_COMPARISONDLG_H__A1EB3165_5223_45A0_8651_EDF9391F8894__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ComparisonDlg.h : header file
//
//GUID CLSID_VMR9 = {0x51B4ABF3, 0x748F, 0x4E3B, 0xA2, 0x76, 0xC8, 0x28, 0x33, 0x0E, 0x92, 0x6A};

#include "resource.h"
#include "CVideoWindow.h"
#include "DSGraph.h"
#include "afxwin.h"
#include "afxcmn.h"
/////////////////////////////////////////////////////////////////////////////
// CComparisonDlg dialog

// For XP & Vista
#import "C:\\Windows\\Microsoft.NET\\Framework\\v2.0.50727\\mscorlib.tlb" no_namespace named_guids raw_interfaces_only
// Win2K - #import "C:\\WINNT\\Microsoft.NET\\Framework\\v2.0.50727\\mscorlib.tlb" no_namespace named_guids raw_interfaces_only
#import "C:\\Program Files\\RedRat\\RedRat SDK\\RedRat.tlb" no_namespace named_guids raw_interfaces_only

class CComparisonDlg : public CDialog
{
// Construction
public:
	CComparisonDlg(CWnd* pParent = NULL);   // standard constructor
	~CComparisonDlg();

	
//	CVideoWindow mVideoWindow1;
// Dialog Data
	//{{AFX_DATA(CComparisonDlg)
	enum { IDD = IDD_COMPARISONDLG };
	CVideoWindow	mVideoWindow2;
	CVideoWindow	mVideoWindow1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComparisonDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CComparisonDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBtnCapsetting1();
	afx_msg void OnBtnCapsetting2();
	afx_msg void OnBtnSnapshot1();
	afx_msg void OnBtnSnapshot2();	
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CDSGraph* pGraph1;
	CDSGraph* pGraph2;
	
	char ppDeviceDisplayName[2][1024];//="@device:pnp:\\\\?\\usb#vid_6000&pid_0001#2004090820040908#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\{16bf6d30-22ce-4b70-b2a2-e54b13d2f474}";
	//="@device:pnp:\\\\?\\usb#vid_6000&pid_0001#6&33ecd847&0&4#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\{16bf6d30-22ce-4b70-b2a2-e54b13d2f474}";

public:
	afx_msg void OnBnClickedBtnVolumesetting1();
	afx_msg void OnBnClickedBtnVolumesetting2();
	afx_msg void OnBnClickedStarttest();
	CButton mBtnVolumeSetting1;
	CButton mBtnVolumeSetting2;
	void ShowScriptList(CString fileName);
	void PowerSwitch(bool PowerON);
	HANDLE hCom_Power;
	CString RefSTRPath;
	CString TestSTRPath;
	CString CurrentDirectory;
	CString rrDeviceName;
	CString NewProjectName;
	CString SignalDBPath;
	CString ProjectName;
	CString Version;
	CString ProjectFolder;
	CString TempFolder;
	CString VideoFolder;
	CString LogFolder;
	bool bStopTest;

	bool StartCaptureTrace1();
	bool StartCaptureTrace2();
	HANDLE hCom1;
	HANDLE hCom2;
	bool bEnableTrace;
	CString TraceFilePath1;
	CString TraceFilePath2;

	CWinThread * pThread;

	//CSliderCtrl mSLDLRControl1;
	//afx_msg void OnNMCustomdrawSldLrcontrol1(NMHDR *pNMHDR, LRESULT *pResult);
	//CSliderCtrl mSLDLRControl2;
	//afx_msg void OnNMCustomdrawSldLrcontrol2(NMHDR *pNMHDR, LRESULT *pResult);
	

public:
	void RedratInitial();
	void RedratUninitial();
	void RedratSendSignal(CString devName, CString sigName);
	CString GetDeviceName(CString SigalDBPath);

	ICCWRedRat3*   rr3;
	IRedRatLookup* rrLookup;
	ISignalDB* signalDB;
	bool Is_DoubleSignal;
	long LastTimeStamp;
	CListBox mScriptList;
	CString mTraceWnd1;
	CString mTraceWnd2;
	afx_msg void OnBnClickedBtnProjectsetting();
	CString mCurrentCaseID;
	int CheckScript(char* filename);
	afx_msg void OnBnClickedBtnPausetest();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnStoptest();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
};

UINT RunScriptThread(LPVOID pParam);
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPARISONDLG_H__A1EB3165_5223_45A0_8651_EDF9391F8894__INCLUDED_)
