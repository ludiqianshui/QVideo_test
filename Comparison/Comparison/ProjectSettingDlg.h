#pragma once


// CProjectSettingDlg dialog

class CProjectSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CProjectSettingDlg)

public:
	CProjectSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProjectSettingDlg();
// Dialog Data
	enum { IDD = IDD_PROJECTSETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CString mProjectName;
	CString mVersion;
	CString mReferenceSTRPath;
	CString mSignalDBPath;
	afx_msg void OnBnClickedBtnNewproject();
	CString mNewProjectFolder;
	CString mProjectFolder;
	afx_msg void OnBnClickedBtnProjectfolder();
};
