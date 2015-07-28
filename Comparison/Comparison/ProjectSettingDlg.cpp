// ProjectSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Comparison.h"
#include "ProjectSettingDlg.h"


// CProjectSettingDlg dialog

IMPLEMENT_DYNAMIC(CProjectSettingDlg, CDialog)

CProjectSettingDlg::CProjectSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectSettingDlg::IDD, pParent)
	, mProjectName(_T(""))
	, mVersion(_T(""))
	, mReferenceSTRPath(_T(""))
	, mSignalDBPath(_T(""))
	, mNewProjectFolder(_T(""))
	, mProjectFolder(_T(""))
{


}

CProjectSettingDlg::~CProjectSettingDlg()
{
}

void CProjectSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PROJECTNAME, mProjectName);
	DDX_Text(pDX, IDC_EDIT_VERSION, mVersion);
	DDX_Text(pDX, IDC_EDIT_REFERENCESTR, mReferenceSTRPath);
	DDX_Text(pDX, IDC_EDIT_SIGNALDB, mSignalDBPath);
	DDX_Text(pDX, IDC_EDIT_NEWPROJECTFOLDER, mNewProjectFolder);
	DDX_Text(pDX, IDC_EDITPROJECTFOLDER, mProjectFolder);
}


BEGIN_MESSAGE_MAP(CProjectSettingDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CProjectSettingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CProjectSettingDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CProjectSettingDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BTN_NEWPROJECT, &CProjectSettingDlg::OnBnClickedBtnNewproject)
	ON_BN_CLICKED(IDC_BTN_PROJECTFOLDER, &CProjectSettingDlg::OnBnClickedBtnProjectfolder)
END_MESSAGE_MAP()


// CProjectSettingDlg message handlers

void CProjectSettingDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	UpdateData(FALSE);
	OnOK();
}

void CProjectSettingDlg::OnBnClickedButton1()//get reference STR path
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	char  szFilter[] =	"XLS Files (*.xls)|*.xls||";
	CFileDialog Dlg(FALSE,"*.xls","*.xls",OFN_LONGNAMES,szFilter,0);	
	if(Dlg.DoModal() == IDOK)
	{
		mReferenceSTRPath=Dlg.GetPathName();
		//
		UpdateData(FALSE);
	}
	else 
	{
		return;
	}

}

void CProjectSettingDlg::OnBnClickedButton2()//get signal DB path
{
	// TODO: Add your control notification handler code here	
	UpdateData(TRUE);
	char  szFilter[] =	"XML Files (*.xml)|*.xml||";
	CFileDialog Dlg(FALSE,"*.xml","*.xml",OFN_LONGNAMES,szFilter,0);	
	if(Dlg.DoModal() == IDOK)
	{
		mSignalDBPath=Dlg.GetPathName();
		//
		UpdateData(FALSE);
	}
	else 
	{
		return;
	}
}

void CProjectSettingDlg::OnBnClickedBtnNewproject()
{
	// TODO: Add your control notification handler code here
	char   szDir[MAX_PATH]; 
	BROWSEINFO   bi; 
	ITEMIDLIST   *pidl; 

	bi.hwndOwner   =   this-> m_hWnd; 
	bi.pidlRoot   =   NULL; 
	bi.pszDisplayName   =   szDir; 
	bi.lpszTitle   =   "请选择目录 "; 
	bi.ulFlags   =   BIF_RETURNONLYFSDIRS|BIF_USENEWUI; 
	bi.lpfn   =   NULL; 
	bi.lParam   =   0; 
	bi.iImage   =   0; 

	pidl   =   SHBrowseForFolder(&bi); 
	if(pidl   ==   NULL) 
		return; 
	if(!SHGetPathFromIDList(pidl,   szDir))  
		return;
	mNewProjectFolder=szDir;
	UpdateData(FALSE);
}

void CProjectSettingDlg::OnBnClickedBtnProjectfolder()
{
	// TODO: Add your control notification handler code here	
	char   szDir[MAX_PATH]; 
	BROWSEINFO   bi; 
	ITEMIDLIST   *pidl; 

	bi.hwndOwner   =   this-> m_hWnd; 
	bi.pidlRoot   =   NULL; 
	bi.pszDisplayName   =   szDir; 
	bi.lpszTitle   =   "请选择目录 "; 
	bi.ulFlags   =   BIF_RETURNONLYFSDIRS; 
	bi.lpfn   =   NULL; 
	bi.lParam   =   0; 
	bi.iImage   =   0; 

	pidl   =   SHBrowseForFolder(&bi); 
	if(pidl   ==   NULL) 
		return; 
	if(!SHGetPathFromIDList(pidl,   szDir))  
		return;
	mProjectFolder=szDir;
	UpdateData(FALSE);
}
