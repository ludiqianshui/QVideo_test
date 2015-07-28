// ComparisonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Comparison.h"
#include "ComparisonDlg.h"
#include "DSGraph.h"
#include "ProjectSettingDlg.h"
#include "AVComparison.h"
//#include "SampleGrabber.h"
#include "math.h"

#import <MSXML6.dll>
using namespace MSXML2;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#import "C:\Program Files\Common Files\Microsoft Shared\OFFICE11\mso.dll" rename("RGB", "MSRGB")
#import "C:\Program Files\Common Files\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB" raw_interfaces_only, rename("Reference", "ignorethis"), rename("VBE", "JOEVBE")
#import "C:\Program Files\Microsoft Office\OFFICE11\excel.exe" exclude("IFont", "IPicture") rename("RGB", "ignorethis"), rename("DialogBox", "ignorethis"), rename("VBE", "JOEVBE"),rename("ReplaceText", "JOEReplaceText"), rename("CopyFile","JOECopyFile"),rename("FindText", "JOEFindText"), rename("NoPrompt", "JOENoPrompt")

//#import "mso.dll" rename("RGB", "MSRGB")
//#import "VBE6EXT.OLB" raw_interfaces_only, rename("Reference", "ignorethis"), rename("VBE", "JOEVBE")
//#import "excel.exe" exclude("IFont", "IPicture") rename("RGB", "ignorethis"), rename("DialogBox", "ignorethis"), rename("VBE", "JOEVBE"),rename("ReplaceText", "JOEReplaceText"), rename("CopyFile","JOECopyFile"),rename("FindText", "JOEFindText"), rename("NoPrompt", "JOENoPrompt")

using namespace Office;
using namespace VBIDE;
using namespace Excel;


extern "C" int __declspec(dllimport) DllStartRecording(int left, int top, int right, int bottom);
extern "C" int __declspec(dllimport) DllStopRecord(char* savePath, int Param);
BOOL TestSingleCase(CString ScriptPath,CComparisonDlg* pDlg);
//extern bool ImageCompareByGrayscale(void);
//extern bool ImageComaparebyPix(void);
//extern int ImageComparison(char ComapreList[]);
extern int CheckPointNum;

 CString gTempPath="C:\\WINDOWS\\Temp";
 
/////////////////////////////////////////////////////////////////////////////
// CComparisonDlg dialog


CComparisonDlg::CComparisonDlg(CWnd* pParent /*=NULL*/)
: CDialog(CComparisonDlg::IDD, pParent)
, mCurrentCaseID(_T(""))
{
	CoInitialize(NULL);
	//{{AFX_DATA_INIT(CComparisonDlg)
	//}}AFX_DATA_INIT
	pGraph1=new CDSGraph();
	pGraph2=new CDSGraph();
	pThread=NULL;
	bStopTest=false;

	//RedratInitial();
	hCom_Power=NULL;
	RefSTRPath="";
	TestSTRPath="";
	char currDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,currDir);
	CurrentDirectory=currDir;
	bEnableTrace=false;

	NewProjectName="";
	SignalDBPath="SignalDB.xml";
	ProjectName="";
	Version="";
	ProjectFolder="";
	TempFolder="C:\\WINDOWS\\Temp";
	VideoFolder=".";
	LogFolder=".";
	signalDB=NULL;
	rr3=NULL;
	rrLookup=NULL;
}

CComparisonDlg::~CComparisonDlg()

{
	//if(pGraph1)
	//	delete pGraph1;
	//if(pGraph2)
	//	delete pGraph2;

	
	RedratUninitial();
	CoUninitialize();
}


//////////////////////////////////////////////
//Redrat//////////////////////////////////////
//////////////////////////////////////////////

void CComparisonDlg::RedratInitial()
{
	rr3=NULL;
	rrLookup=NULL;
	signalDB=NULL;
	Is_DoubleSignal=false;
	SAFEARRAY* rr3s;

	HRESULT hr;
	hr = CoCreateInstance(CLSID_RedRatLookup,NULL,CLSCTX_SERVER,IID_IRedRatLookup,reinterpret_cast<void**>(&rrLookup));
	if (FAILED(hr)) 
		MessageBox("CoCreateInstance Failed!\nPlease check the connection of Redrat3");


	//hr = rrLookup->FindRedRat3s(&rr3s);
	//BSTR* rrName = (BSTR*)rr3s->pvData;
	BSTR rrName = SysAllocString(OLESTR("RedRatII-0"));
	hr = rrLookup->GetRedRat3(rrName,&rr3);
	if (FAILED(hr)) 
	{
		MessageBox("GetRedRat3 Failed!\nPlease check the connection of Redrat3");
		return;
	}
	SysFreeString(rrName);


	//SAFEARRAY* rr3s;
	hr = rrLookup->FindRedRat3s(&rr3s);
	if (FAILED(hr)) AfxMessageBox("FindRedRat3s Failed!\nPlease check the connection of Redrat3");	



	long noRR3s = rr3s->rgsabound[0].cElements;
	if (noRR3s > 0)
	{
		BSTR* rr3Name = (BSTR*)rr3s->pvData;
		rrLookup->GetRedRat3(*rr3Name,&rr3);
		// Iterate over the RedRat3 names and list them.
		CString str;
		for (int i=0; i<noRR3s; i++)
		{
			str.Format("%c%c%c%c%c%c%c%c%c%c%c%c",rr3Name[0],rr3Name[1],rr3Name[2],rr3Name[3],rr3Name[4],rr3Name[5]);
			//			MessageBox(str);
		}
	}
	else
	{
		MessageBox("Have not been able to find any RedRat3s.\n");
		//			return 0;
	}

		
	CString dbName=SignalDBPath;
//	CString dvName="";
		
	hr = CoCreateInstance(CLSID_CCWSignalDB,NULL,CLSCTX_SERVER,IID_ISignalDB,reinterpret_cast<void**>(&signalDB));
	if (FAILED(hr)) 
		MessageBox("CoCreateInstance for signalDB Failed!");		

	//	BSTR dbFileName = SysAllocString(_com_util::ConvertStringToBSTR("Demo.xml"));
	hr = signalDB->AddDeviceDB(dbName.AllocSysString());
	if (FAILED(hr)) 
		MessageBox("Failed to load SignalDB!");	

	//SysFreeString(dbFileName);	
	hr = rr3->SetSignalDB(signalDB);
	if (FAILED(hr)) 
		MessageBox("Failed to set SignalDB!");

}

CString CComparisonDlg::GetDeviceName(CString SigalDBPath)
{
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(!SUCCEEDED(hr))
	{
		MessageBox("Failed to create DOMDocument30!");
		return "";
	}
	pDoc->load(SigalDBPath.GetString());
	MSXML2::IXMLDOMElementPtr childNode;
	MSXML2::IXMLDOMNodeListPtr pList;
	_variant_t vt;
	childNode = (MSXML2::IXMLDOMElementPtr)(pDoc->selectSingleNode("//Name"));
	childNode->get_nodeTypedValue(&vt);
	rrDeviceName=vt.bstrVal;

	childNode = (MSXML2::IXMLDOMElementPtr)(pDoc->selectSingleNode("//DoubleSignals"));
	childNode->get_nodeTypedValue(&vt);
	CString str=(CString)vt.bstrVal;
	if(str=="false")
		Is_DoubleSignal=false;
	else
		Is_DoubleSignal=true;
	//pDoc->Release();
	
	return "";

}

void CComparisonDlg::RedratUninitial()
{
	if(signalDB) 
		signalDB->Release();

	if(rr3) 
		rr3->Release();

	if(rrLookup) 
		rrLookup->Release();
}

void CComparisonDlg::RedratSendSignal( CString devName, CString sigName)
{
	HRESULT hr;
	CString Interval;
	if(Is_DoubleSignal)
	{
		CCWDoubleModulatedSignal *dualSig=new CCWDoubleModulatedSignal();
		signalDB->GetDoubleModSignal(devName.AllocSysString(),sigName.AllocSysString(),dualSig);

		hr = rr3->OutputModulatedSignal(&(dualSig->signal1));
		delete(dualSig);
	}
	else
	{					
		hr = rr3->OutputSignal(devName.AllocSysString(),sigName.AllocSysString());
	}		
	if(FAILED(hr))	
		MessageBox("Failed to output signal");
}

void CComparisonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComparisonDlg)
	DDX_Control(pDX, IDC_VIDEOWINDOW2, mVideoWindow2);
	DDX_Control(pDX, IDC_VIDEOWINDOW1, mVideoWindow1);
	//}}AFX_DATA_MAP
	//	DDX_Control(pDX, IDC_BTN_VOLUMESETTING1, mBtnVolumeSetting1);
	//	DDX_Control(pDX, IDC_BTN_VOLUMESETTING2, mBtnVolumeSetting2);
	//	DDX_Control(pDX, IDC_SLD_LRCONTROL1, mSLDLRControl1);
	//	DDX_Control(pDX, IDC_SLD_LRCONTROL2, mSLDLRControl2);
	DDX_Control(pDX, IDC_SCRIPTLIST, mScriptList);
	DDX_Text(pDX, IDC_EDIT_TRACEWND1, mTraceWnd1);
	DDX_Text(pDX, IDC_EDIT_TRACEWND2, mTraceWnd2);
	DDX_Text(pDX, IDC_EDIT_CURRENTCASE, mCurrentCaseID);
}


BEGIN_MESSAGE_MAP(CComparisonDlg, CDialog)
	//{{AFX_MSG_MAP(CComparisonDlg)
	ON_BN_CLICKED(IDC_BTN_CAPSETTING1, OnBtnCapsetting1)
	ON_BN_CLICKED(IDC_BTN_CAPSETTING2, OnBtnCapsetting2)
	ON_BN_CLICKED(IDC_BTN_SNAPSHOT1, OnBtnSnapshot1)
	ON_BN_CLICKED(IDC_BTN_SNAPSHOT2, OnBtnSnapshot2)
	//}}AFX_MSG_MAP
	//	ON_BN_CLICKED(IDC_BTN_VOLUMESETTING1, &CComparisonDlg::OnBnClickedBtnVolumesetting1)
	//	ON_BN_CLICKED(IDC_BTN_VOLUMESETTING2, &CComparisonDlg::OnBnClickedBtnVolumesetting2)
	ON_BN_CLICKED(IDC_BTN_STARTTEST, &CComparisonDlg::OnBnClickedStarttest)
	//	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLD_LRCONTROL1, &CComparisonDlg::OnNMCustomdrawSldLrcontrol1)
	//	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLD_LRCONTROL2, &CComparisonDlg::OnNMCustomdrawSldLrcontrol2)
	ON_BN_CLICKED(IDC_BTN_PROJECTSETTING, &CComparisonDlg::OnBnClickedBtnProjectsetting)
	ON_BN_CLICKED(IDC_BTN_PAUSETEST, &CComparisonDlg::OnBnClickedBtnPausetest)
	ON_BN_CLICKED(IDCANCEL, &CComparisonDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_STOPTEST, &CComparisonDlg::OnBnClickedBtnStoptest)
	ON_BN_CLICKED(IDOK, &CComparisonDlg::OnBnClickedOk)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComparisonDlg message handlers
BOOL CComparisonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	bool ret;
	char VidDevice[2][1024];
	char AudDevice[2][1024];
	char pFriendlyName1V[256]="Osprey-200 Video Device 1";
	//UDsUtils::QueryDeviceCategory(AM_KSCATEGORY_CAPTURE,ppDeviceDisplayName,pFriendlyName);
	UDsUtils::QueryDeviceCategory(CLSID_VideoInputDeviceCategory,VidDevice,pFriendlyName1V);
	char pFriendlyName1A[256]="Osprey-200 Audio Device 1";
	UDsUtils::QueryDeviceCategory(CLSID_AudioInputDeviceCategory,AudDevice,pFriendlyName1A);
	memcpy(&ppDeviceDisplayName[0],&VidDevice[0],1024);
	memcpy(&ppDeviceDisplayName[1],&AudDevice[0],1024);
	ret=pGraph1->CreateGraph(ppDeviceDisplayName[0],ppDeviceDisplayName[1],mVideoWindow1.m_hWnd,NULL,0);

	if(ret==FALSE)
	{
		MessageBox(pGraph1->FailedInfo+" 1");
		return FALSE;
	}


	char pFriendlyName2V[256]="Osprey-200 Video Device 2";
	//UDsUtils::QueryDeviceCategory(AM_KSCATEGORY_CAPTURE,ppDeviceDisplayName,pFriendlyName2V);
	UDsUtils::QueryDeviceCategory(CLSID_VideoInputDeviceCategory,VidDevice,pFriendlyName2V);
	char pFriendlyName2A[256]="Osprey-200 Audio Device 2";
	UDsUtils::QueryDeviceCategory(CLSID_AudioInputDeviceCategory,AudDevice,pFriendlyName2A);
	memcpy(&ppDeviceDisplayName[0],&VidDevice[0],1024);
	memcpy(&ppDeviceDisplayName[1],&AudDevice[0],1024);
	ret=pGraph2->CreateGraph(ppDeviceDisplayName[0],ppDeviceDisplayName[1],mVideoWindow2.m_hWnd,NULL,1);

	//if(ret==FALSE)
	//{
	//	MessageBox(pGraph1->FailedInfo + " 2");
	//	return FALSE;
	//}

	pGraph1->StartGraph();
	pGraph2->StartGraph();

	hCom_Power=CreateFile("COM1",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,	0, NULL);
	if(hCom_Power==(HANDLE)-1)
	{
		MessageBox("打开COM失败!");
		return FALSE;
	}

	SetupComm(hCom_Power,100,100); //输入缓冲区和输出缓冲区的大小都是100

	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout=MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier=0;
	TimeOuts.ReadTotalTimeoutConstant=0;

	TimeOuts.WriteTotalTimeoutMultiplier=100;
	TimeOuts.WriteTotalTimeoutConstant=500;
	SetCommTimeouts(hCom_Power,&TimeOuts); //设置超时

	DCB dcb;
	GetCommState(hCom_Power,&dcb);
	dcb.BaudRate=9600; //波特率为9600
	dcb.ByteSize=8; //每个字节有8位
	dcb.Parity=NOPARITY; //无奇偶校验位
	dcb.StopBits=TWOSTOPBITS; //两个停止位
	SetCommState(hCom_Power,&dcb);

	PurgeComm(hCom_Power,PURGE_TXCLEAR|PURGE_RXCLEAR);


	//if(pGraph1->CreateGraph(ppDeviceDisplayName[0],mVideoWindow1.m_hWnd,NULL,0)&&
	//	pGraph2->CreateGraph(ppDeviceDisplayName[1],mVideoWindow2.m_hWnd,NULL,1))
	//{
	//	pGraph2->StartGraph();
	//	pGraph1->StartGraph();
	//	
	//}
	//else
	//{
	//	MessageBox(pGraph1->FailedInfo);
	//	MessageBox(pGraph2->FailedInfo);
	//}


	//	mSLDLRControl1.SetRange(-10000,10000,1);
	//mSLDLRControl1.SetPos(0);
	//mSLDLRControl2.SetRange(-10000,10000,1);
	//mSLDLRControl2.SetPos(0);
	return TRUE;
}

BOOL CComparisonDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	// Get the bounding rectangle for the movie screen
	mVideoWindow1.GetWindowRect(&rc);
	ScreenToClient(&rc);
	// Exclude the clipping region occupied by our movie screen
	pDC->ExcludeClipRect(&rc);

	// Erase the remainder of the dialog as usual
	return CDialog::OnEraseBkgnd(pDC);
}


void CComparisonDlg::OnBtnCapsetting1() 
{
	// TODO: Add your control notification handler code here
	pGraph1->CaptureSetting();
}

void CComparisonDlg::OnBtnCapsetting2() 
{
	// TODO: Add your control notification handler code here
	pGraph2->CaptureSetting();
}

void CComparisonDlg::OnBtnSnapshot1() 
{
	// TODO: Add your control notification handler code here	
	//SYSTEMTIME sysTime;
	//GetSystemTime(&sysTime);
	CString fileName;
	char  szFilter[] =	"BMP Files (*.bmp)|*.bmp||";
	CFileDialog savaDlg(FALSE,"*.bmp","*.bmp",OFN_LONGNAMES,szFilter,0);	
	if(savaDlg.DoModal() == IDOK)
	{
		fileName=savaDlg.GetPathName();			
	}
	else 
	{
		return;
	}
	//fileName.Format("D:\\Snapshot1_%02d%02d_%02d%02d%02d%03d.bmp",sysTime.wMonth,sysTime.wDay,sysTime.wHour+8,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
	pGraph1->GetCurrentImage(fileName);


}


void CComparisonDlg::OnBtnSnapshot2() 
{
	// TODO: Add your control notification handler code here
	//SYSTEMTIME sysTime;
	//GetSystemTime(&sysTime);
	CString fileName;
	char  szFilter[] =	"BMP Files (*.bmp)|*.bmp||";
	CFileDialog savaDlg(FALSE,"*.bmp","*.bmp",OFN_LONGNAMES,szFilter,0);	
	if(savaDlg.DoModal() == IDOK)
	{
		fileName=savaDlg.GetPathName();		
	}
	else 
	{
		return;
	}
	//fileName.Format("D:\\Snapshot2_%02d%02d_%02d%02d%02d%03d.bmp",sysTime.wMonth,sysTime.wDay,sysTime.wHour+8,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
	pGraph2->GetCurrentImage(fileName);

}



void CComparisonDlg::OnOK()
{
	if(pThread)
	{
		MessageBox("The test is running, please stop it first!");
		return;
	}

	pGraph1->DeleteGraph(0);
	pGraph2->DeleteGraph(1);

	delete pGraph1;
	delete pGraph2;

	pGraph1=NULL;
	pGraph2=NULL;
	CDialog::OnOK();

}



void CComparisonDlg::OnBnClickedBtnVolumesetting1()
{
	// TODO: Add your control notification handler code here
	if(pGraph1->ChangeVolume())
		mBtnVolumeSetting1.SetWindowText("((( )))");
	else
		mBtnVolumeSetting1.SetWindowText("X");

}

void CComparisonDlg::OnBnClickedBtnVolumesetting2()
{
	// TODO: Add your control notification handler code here
	if(pGraph2->ChangeVolume())
		mBtnVolumeSetting2.SetWindowText("((( )))");
	else
		mBtnVolumeSetting2.SetWindowText("X");

}

void CComparisonDlg::PowerSwitch(bool PowerON)
{
	char lpOutBuffer[3];
	memset(lpOutBuffer,'\0',3);
	if(PowerON)
	{
		lpOutBuffer[0]=0xE2;  
		lpOutBuffer[1]=0x05;  
		lpOutBuffer[2]=0xE7; 		
	}
	else
	{
		lpOutBuffer[0]=0xE1;  
		lpOutBuffer[1]=0x05;  
		lpOutBuffer[2]=0xE6; 
	}

	DWORD dwBytesWrite=3;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	ClearCommError(hCom_Power,&dwErrorFlags,&ComStat);

	bWriteStat=WriteFile(hCom_Power,lpOutBuffer,dwBytesWrite,& dwBytesWrite,NULL);::Sleep(50);
	bWriteStat=WriteFile(hCom_Power,lpOutBuffer,dwBytesWrite,& dwBytesWrite,NULL);::Sleep(100);
	bWriteStat=WriteFile(hCom_Power,lpOutBuffer,dwBytesWrite,& dwBytesWrite,NULL);::Sleep(150);
	bWriteStat=WriteFile(hCom_Power,lpOutBuffer,dwBytesWrite,& dwBytesWrite,NULL);::Sleep(800);
	bWriteStat=WriteFile(hCom_Power,lpOutBuffer,dwBytesWrite,& dwBytesWrite,NULL);

	if(!bWriteStat)
	{
		MessageBox("Failed of COM!");
	}
}

//
//void CComparisonDlg::OnNMCustomdrawSldLrcontrol1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	pGraph1->ChangeBalance(mSLDLRControl1.GetPos());
//	*pResult = 0;
//}
//
//void CComparisonDlg::OnNMCustomdrawSldLrcontrol2(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	pGraph2->ChangeBalance(mSLDLRControl2.GetPos());
//	*pResult = 0;
//}

extern bool StartCheck;

void CComparisonDlg::OnBnClickedStarttest() 
{
	// TODO: Add your control notification handler code here
	if(TestSTRPath=="")
	{
		MessageBox("Please set the test event first!");
		return;
	}
	else
	{
    	pThread = AfxBeginThread(RunScriptThread, this);		
		GetDlgItem(IDC_BTN_STARTTEST)->EnableWindow(false);
		GetDlgItem(IDC_BTN_PROJECTSETTING)->EnableWindow(false);
	}
	
}

int ExecuteScript(CString fileName,CComparisonDlg* pDlg);
char CompareList[1024];
int complist=0;

bool GetResultofDependCase(_WorksheetPtr pThisWorksheet,char* szCaseID)
{
	
	if(pThisWorksheet==NULL)
		return false;

	if(strcmp(szCaseID,"")==0||szCaseID==NULL)
		return true;

	CString cellPos;
	RangePtr pThisRange = NULL ;
	_variant_t vt ;
	char* str=NULL;//=new char[128];
	int j=25;

	while(j<256)
	{
		cellPos.Format("A%d",j);
		pThisRange = pThisWorksheet->GetRange(COleVariant(cellPos),COleVariant(cellPos)); 
		vt=pThisRange->GetValue2(); 
		if(vt.vt)
			str = _com_util::ConvertBSTRToString(_bstr_t(vt));
		else
		{
			j++;
			str=NULL;
			continue;
		}

		if(strcmp(str,szCaseID)==0)
		{
			
			delete []str;str=NULL;
			cellPos.Format("H%d",j);
			pThisRange = pThisWorksheet->GetRange(COleVariant(cellPos),COleVariant(cellPos)); 
			vt=pThisRange->GetValue2();   
			if(vt.vt)
				str = _com_util::ConvertBSTRToString(_bstr_t(vt));
			else
				return false;
				
			if(strcmp(str,"Pass")==0)
			{
				delete []str;str=NULL;				
				return true;
			}
			else
			{
				delete []str;str=NULL;
				return false;
			}
		}
		delete []str;str=NULL;
		j++;
	}
	//delete []str;
	return false;
}

UINT RunScriptThread(LPVOID pParam)
{

	CComparisonDlg* pDlg=(CComparisonDlg*)pParam;		
	RECT rc;
	

	_ApplicationPtr pApplication = NULL ;
	_WorkbookPtr pThisWorkbook = NULL ;
	_WorksheetPtr pThisWorksheet = NULL ;
	SheetsPtr pThisSheets = NULL ;
	RangePtr pThisRange = NULL ;
	_variant_t vt ;

	CString cellPos;

	Excel::XlFileFormat vFileFormat ;
	Excel::XlSaveAsAccessMode vSaveAsAccessMode ;
	Excel::XlSaveConflictResolution vSaveConflictResolution ;
	HRESULT hr=pApplication.CreateInstance("Excel.Application");
	pApplication->PutVisible (0,VARIANT_FALSE);

	if(pDlg->TestSTRPath=="")
	{
		AfxMessageBox("Please set the project first");
		return false;
	}

	pThisWorkbook = pApplication->Workbooks->Open(_bstr_t(pDlg->TestSTRPath)); 
	pThisSheets = pThisWorkbook->GetWorksheets() ;  
	pThisWorksheet = pThisSheets->GetItem(_bstr_t("测试结果"));

	char* str=NULL ;//= new char[128];
	char TestSheet[128][128];
	int nTestFeature=-1;

	int j=19;
	while(j<128)
	{
		cellPos.Format("B%d",j);
		pThisRange = pThisWorksheet->GetRange(COleVariant(cellPos),COleVariant(cellPos)); 
		vt=pThisRange->GetValue2();  
		if(vt.vt)
			str = _com_util::ConvertBSTRToString(_bstr_t(vt));
		else
			str=NULL;

		if(str&&strcmp(str,"TOTAL")==0)
		{
			delete []str;str=NULL;
			break;
		}
		if(nTestFeature>=0)
		{
			strcpy(TestSheet[nTestFeature],str);
			delete []str;str=NULL;
			nTestFeature++;
		}
		if(str&&strcmp(str,"Family")==0)
		{
			delete []str;str=NULL;
			nTestFeature=0;
		}
//		delete []str;str=NULL;
		j++;		
	}

	for(int f=0;f<nTestFeature;f++)
	{
		if(pDlg->bStopTest==true)
			break;
	//	delete []str;str=NULL;
		pThisWorksheet.Release();
		pThisWorksheet = pThisSheets->GetItem(_bstr_t(TestSheet[f]));
		if(pThisWorksheet==NULL)
			continue;

		int i=25;
		while(i<256)
		{			
			if(pDlg->bStopTest==true)
				break;

			CString Result="";
//			delete []str;str=NULL;
			cellPos.Format("E%d",i);
			pThisRange = pThisWorksheet->GetRange(COleVariant(cellPos),COleVariant(cellPos)); 
			vt=pThisRange->GetValue2(); 
			if(vt.vt)
				str = _com_util::ConvertBSTRToString(_bstr_t(vt));
			else
			{
				i++;
				str=NULL;
				continue;
			}
				//str = _com_util::ConvertBSTRToString(_bstr_t(vt));
			delete []str;str=NULL;//Not selected to perform auto test	
			
			cellPos.Format("C%d",i);//result
			pThisRange = pThisWorksheet->GetRange(COleVariant(cellPos),COleVariant(cellPos)); 
			vt=pThisRange->GetValue2();   
			if(vt.vt)
				str = _com_util::ConvertBSTRToString(_bstr_t(vt));
			else
			{
				i++;
				str=NULL;
				continue;
			}

			if(str&&strcmp(str,"PASS")!=0)
			{
				i++;
				delete []str;str=NULL;
				continue;
			}
			delete []str;str=NULL;


			cellPos.Format("F%d",i);//script
			pThisRange = pThisWorksheet->GetRange(COleVariant(cellPos),COleVariant(cellPos)); 
			vt=pThisRange->GetValue2();   
			if(vt.vt)
				str = _com_util::ConvertBSTRToString(_bstr_t(vt));
			else
			{
				i++;
				str=NULL;
				continue;
			}

			delete []str;str=NULL;
			

			cellPos.Format("G%d",i);//depend case
			pThisRange = pThisWorksheet->GetRange(COleVariant(cellPos),COleVariant(cellPos)); 
			vt=pThisRange->GetValue2();   
			if(vt.vt)
				str = _com_util::ConvertBSTRToString(_bstr_t(vt));
			else
			{
				;
			}

			CString dependCase;
			dependCase=str;
			delete []str;str=NULL;
			if(GetResultofDependCase(pThisWorksheet,dependCase.GetBuffer())==false)
			{
				cellPos.Format("H%d:H%d",i,i);
				pThisRange = pThisWorksheet->GetRange(_variant_t(cellPos)) ;	
				pThisRange->GetInterior()->ColorIndex = 3 ;
				pThisRange->PutItem(1, 1, _variant_t("Depended Case is not Pass")) ;
				i++;
				continue;
			}
	


			pDlg->GetWindowRect(&rc);		
			DllStartRecording(rc.left,rc.top,rc.right,rc.bottom);

			cellPos.Format("A%d",i);
			pThisRange = pThisWorksheet->GetRange(COleVariant(cellPos),COleVariant(cellPos)); 
			vt=pThisRange->GetValue2();   
			if(vt.vt)
				str = _com_util::ConvertBSTRToString(_bstr_t(vt));
			else
				str="UnKnownCaseID";

	//		pDlg->mCurrentCaseID=str;
			pDlg->GetDlgItem(IDC_STATIC_CURRENCTCASE)->SetWindowTextA(str);
	//		pDlg->UpdateData(FALSE);
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			pDlg->bEnableTrace=true;
			::Sleep(1000);
			pDlg->TraceFilePath1.Format("%s\\%s_Ref.txt",pDlg->LogFolder,str);
			pDlg->TraceFilePath2.Format("%s\\%s_Test.txt",pDlg->LogFolder,str);
			delete []str;str=NULL;
			pDlg->StartCaptureTrace1();
			pDlg->StartCaptureTrace2();

			cellPos.Format("F%d",i);//script
			pThisRange = pThisWorksheet->GetRange(COleVariant(cellPos),COleVariant(cellPos)); 
			vt=pThisRange->GetValue2();  
			if(vt.vt)
				str = _com_util::ConvertBSTRToString(_bstr_t(vt));
			else
				str=NULL;

			
			//bool res = TestSingleCase(str,pDlg);
			int res=RESULT_FAIL;
			int validscript=RESULT_OK;
			CheckPointNum=0;

			validscript=ExecuteScript(str,pDlg);//////////////////////////////////////////////////////////////////////////////////
			delete []str;str=NULL;

			::Sleep(1000);
			pDlg->bEnableTrace=false;

			if(validscript==RESULT_OK)
			{
				//res=true;
				::Sleep(5000);
				res = ImageComparison(CompareList);
			}
			///////////////////////////////////////////////////////////////////////////////////////////////////////

			//delete []str;str=NULL;
		

			cellPos.Format("A%d",i);
			pThisRange = pThisWorksheet->GetRange(COleVariant(cellPos),COleVariant(cellPos)); 
			vt=pThisRange->GetValue2();   
			if(vt.vt)
				str = _com_util::ConvertBSTRToString(_bstr_t(vt));
			else
				str="UnKnownCaseID";
			CString aviFile;
			aviFile.Format("%s\\%s.avi",pDlg->VideoFolder,str);
			DllStopRecord(aviFile.GetBuffer(),0);
			delete []str;str=NULL;

			cellPos.Format("H%d:H%d",i,i);	//result	

			if(res==RESULT_PASS)
			{	
				pThisRange = pThisWorksheet->GetRange(_variant_t(cellPos)) ;	
				pThisRange->GetInterior()->ColorIndex = 4 ;
				pThisRange->PutItem(1, 1, _variant_t("Pass")) ;
				
			}
			else 
			{
				if(validscript!=RESULT_OK) //script problem
				{

					pThisRange = pThisWorksheet->GetRange(_variant_t(cellPos)) ;	
					pThisRange->GetInterior()->ColorIndex = 3 ;
					if(validscript==RESULT_SCRIPT_NOT_FOUND)
						pThisRange->PutItem(1, 1, _variant_t("Script not found"));
					if(validscript==RESULT_SCRIPT_FORMAT_ERROR)
						pThisRange->PutItem(1, 1, _variant_t("Script format error"));
					if(validscript==RESULT_INTERRUPT_BY_USER)
						pThisRange->PutItem(1, 1, _variant_t("Interrput by user"));
				}
						

				else //test problem
				{			
					CString aviFile;

					aviFile.Format("%s","Fail");
					pThisRange = pThisWorksheet->GetRange(_variant_t(cellPos)) ;
					pThisRange->GetInterior()->ColorIndex = 3 ;
					pThisRange->PutItem(1, 1, _variant_t(aviFile));

				}	
			}
			i++;	
		}

	}

	vSaveAsAccessMode = xlNoChange ;
	vFileFormat = xlWorkbookNormal ;
	vSaveConflictResolution = xlLocalSessionChanges ;

	pThisWorkbook->Save();
	pThisWorkbook->Close();
	pApplication->Quit();

	pThisWorksheet.Release();
	pThisSheets.Release();
	pThisWorkbook.Release();
	pApplication.Release();

	pDlg->bStopTest=false;
//	AfxEndThread(0);
	pDlg->GetDlgItem(IDC_BTN_STOPTEST)->EnableWindow(true);
	pDlg->GetDlgItem(IDC_BTN_PAUSETEST)->EnableWindow(true);
	pDlg->GetDlgItem(IDC_BTN_STARTTEST)->EnableWindow(true);
	pDlg->GetDlgItem(IDC_BTN_PROJECTSETTING)->EnableWindow(true);

	pDlg->pThread=NULL;
	AfxMessageBox("Test Finished!");
	return 0;
}

void CComparisonDlg::ShowScriptList(CString fileName)
{
	
	while(mScriptList.GetCount())
		mScriptList.DeleteString(0);
	//	UpdateData(false);
	//	mScriptList.UpdateData(false);
	FILE *fp;
	fp=fopen(fileName,"r");
	if(fp==NULL)
		return;
	char line[256];
	char keyName[256];
	char szWaitingTime[256];
	int waitingTime;
	int currentCmd=0;

	GetDlgItem(IDC_EDIT_CURRENTCASE)->SetWindowTextA(fileName.Right(fileName.GetLength()-fileName.ReverseFind('\\')-1));

	//while(true)
	for(;;)
	{
		//if(fgets(line,254,fp)==NULL) break;
		//sscanf(line,"%s",szWaitingTime);
		if(fgets(line,254,fp)==NULL) break;
		sscanf(line,"%s",keyName);
		//keyName.Trim();
		//mScriptList.InsertString(mScriptList.GetCount(),szWaitingTime);
		mScriptList.InsertString(mScriptList.GetCount(),keyName);
		if(strcmp(keyName,"END")==0)
			break;		
	}	

	fclose(fp);
	//UpdateData(false);
	mScriptList.RedrawWindow();
}


int ExecuteScript(CString fileName,CComparisonDlg* pDlg)
{
	//mScriptList.ResetContent();
	

	FILE *fp;
	fp=fopen(fileName,"r");
		
	//pDlg->GetDlgItem(IDC_EDIT_CURRENTCASE)->SetWindowTextA(fileName);

	if(fp==NULL)
	{
		fileName=pDlg->ProjectFolder+"\\Script\\"+fileName;
		fp=fp=fopen(fileName,"r");
		if(fp==NULL)
			return RESULT_SCRIPT_NOT_FOUND;
	}

	if(pDlg->CheckScript(fileName.GetBuffer())!=0)
	{
		return RESULT_SCRIPT_FORMAT_ERROR;
	}

	pDlg->ShowScriptList(fileName);	

	char line[256];
	char keyName[256];
	char szWaitingTime[256];

	int waitingTime;
	int currentCmd=0; 

	int loop1=0,loop2=0,loop3=0;
	long pos1,pos2,pos3;
	int loopStartIndex=0;

	do
	{	 
		if(pDlg->bStopTest==true)
			return RESULT_INTERRUPT_BY_USER;
		pDlg->mScriptList.SetCurSel(currentCmd++);
		if(fgets(line,254,fp)==NULL) break;
		if(strstr(line,"//"))  continue;
		sscanf(line,"%d",&waitingTime);		

		if(strstr(line,"//")) waitingTime=0;
		::Sleep(waitingTime);

		pDlg->mScriptList.SetCurSel(currentCmd++);
		if(fgets(line,254,fp)==NULL) break;
		sscanf(line,"%s",keyName);		    

		if(strstr(strlwr(keyName),"loop_start_"))
		{
			loopStartIndex=currentCmd;
			loop1=atoi(keyName+11);
			pos1 = ftell(fp);

			for(int i=0;i<loop1;i++)
			{
				currentCmd=loopStartIndex;
				fseek(fp,pos1,SEEK_SET);  
				do
				{		
					if(pDlg->bStopTest==true)
						return RESULT_INTERRUPT_BY_USER;

					pDlg->mScriptList.SetCurSel(currentCmd++);
					if(fgets(line,254,fp)==NULL) break;
					if(strstr(line,"//")) continue;

					sscanf(line,"%d",&waitingTime);					
					if(strstr(line,"//")) waitingTime=0;
					::Sleep(waitingTime);

					pDlg->mScriptList.SetCurSel(currentCmd++);
					if(fgets(line,254,fp)==NULL) break;
					sscanf(line,"%s",keyName);
					
					if(strstr(strlwr(keyName),".txt"))
					{
						ExecuteScript(keyName,pDlg);
						//pDlg->GetDlgItem(IDC_EDIT_CURRENTCASE)->SetWindowTextA(fileName);
						pDlg->ShowScriptList(fileName);
					}
					else if(strstr(strlwr(keyName),"//")!=NULL)
					{
						continue;
					}
					else if(!strcmp(strlwr(keyName),"check_av"))
					{
						CompareList[complist++]='V';
						StartCheck=true;
						continue;
					}			
					else if(!strcmp(strlwr(keyName),"check_ui"))
					{
						CompareList[complist++]='U';
						StartCheck=true;
						continue;
					}
					else if(!strcmp(strlwr(keyName),"power_on"))
					{
						pDlg->PowerSwitch(true);
					}
					else if(!strcmp(strlwr(keyName),"power_off"))
					{
						pDlg->PowerSwitch(false);
					}
					else  if(strstr(strlwr(keyName),"end")==NULL)
					{
						pDlg->RedratSendSignal(pDlg->rrDeviceName,keyName);
					}
				}
				while(strstr(strlwr(keyName),"loop_end")==NULL);
			}//loop over
		}
		else if(strstr(strlwr(keyName),".txt")!=NULL)
		{			
			ExecuteScript(keyName,pDlg);
			//pDlg->GetDlgItem(IDC_EDIT_CURRENTCASE)->SetWindowTextA(fileName);
			pDlg->ShowScriptList(fileName);
		}
		else if(strstr(strlwr(keyName),"//")!=NULL)
	    {
		    continue;
	    }
		else if(!strcmp(strlwr(keyName),"check_av"))
		{
			CompareList[complist++]='V';
			StartCheck=true;
			continue;
		}			
		else if(!strcmp(strlwr(keyName),"check_ui"))
		{
			CompareList[complist++]='U';
			StartCheck=true;
			continue;
		}
		else if(!strcmp(strlwr(keyName),"power_on"))
		{
			pDlg->PowerSwitch(true);
		}
		else if(!strcmp(strlwr(keyName),"power_off"))
		{
			pDlg->PowerSwitch(false);
		}	
		else if(strstr(strlwr(keyName),"end")==NULL)
		{
			pDlg->RedratSendSignal(pDlg->rrDeviceName,keyName);
		}
	}
	while(strcmp(strlwr(keyName),"end"));
	pDlg->mScriptList.SetCurSel(currentCmd++);
	pDlg->mScriptList.SetCurSel(currentCmd++);

	fclose(fp);

	return RESULT_OK;

}

UINT CaptureTraceThread1(LPVOID pParam);
UINT CaptureTraceThread2(LPVOID pParam);


bool CComparisonDlg::StartCaptureTrace1()
{
	CString COMPort;
	int Baudrate;
	GetPrivateProfileString("Config","TraceCom1","COM3",COMPort.GetBuffer(MAX_PATH),MAX_PATH,CurrentDirectory+"\\Config.ini");
	Baudrate=GetPrivateProfileInt("Config","Baudrate1",115200,CurrentDirectory+"\\Config.ini");
	hCom1=CreateFile(COMPort,GENERIC_READ|GENERIC_WRITE, 0, NULL,	OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL);
	if(hCom1==(HANDLE)-1)
	{
		//MessageBox("Failed to open Trace COM 1");
		return FALSE;
	}
	SetupComm(hCom1,300,300); //输入缓冲区和输出缓冲区的大小都是100

	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout=MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier=0;
	TimeOuts.ReadTotalTimeoutConstant=0;

	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier=100;
	TimeOuts.WriteTotalTimeoutConstant=500;
	SetCommTimeouts(hCom1,&TimeOuts); //设置超时

	DCB dcb;
	GetCommState(hCom1,&dcb);
	dcb.BaudRate=Baudrate; //波特率为9600
	dcb.ByteSize=8; //每个字节有8位
	dcb.Parity=NOPARITY; //无奇偶校验位
	dcb.StopBits=ONESTOPBIT; //两个停止位
	SetCommState(hCom1,&dcb);
	PurgeComm(hCom1,PURGE_TXCLEAR|PURGE_RXCLEAR);

	CWinThread * pThread = AfxBeginThread(CaptureTraceThread1, this);
}


bool CComparisonDlg::StartCaptureTrace2()
{
	CString COMPort;
	int Baudrate;
	GetPrivateProfileString("Config","TraceCom2","COM5",COMPort.GetBuffer(MAX_PATH),MAX_PATH,CurrentDirectory+"\\Config.ini");
	Baudrate=GetPrivateProfileInt("Config","Baudrate2",115200,CurrentDirectory+"\\Config.ini");
	hCom2=CreateFile(COMPort,GENERIC_READ|GENERIC_WRITE, 0, NULL,	OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL);
	if(hCom2==(HANDLE)-1)
	{
		//MessageBox("Failed to open Trace COM 2");
		return FALSE;
	}
	SetupComm(hCom2,300,300); //输入缓冲区和输出缓冲区的大小都是100

	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout=MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier=0;
	TimeOuts.ReadTotalTimeoutConstant=0;

	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier=100;
	TimeOuts.WriteTotalTimeoutConstant=500;
	SetCommTimeouts(hCom2,&TimeOuts); //设置超时

	DCB dcb;
	GetCommState(hCom2,&dcb);
	dcb.BaudRate=Baudrate; //波特率为9600
	dcb.ByteSize=8; //每个字节有8位
	dcb.Parity=NOPARITY; //无奇偶校验位
	dcb.StopBits=ONESTOPBIT; //两个停止位
	SetCommState(hCom2,&dcb);
	PurgeComm(hCom2,PURGE_TXCLEAR|PURGE_RXCLEAR);

	CWinThread * pThread = AfxBeginThread(CaptureTraceThread2, this);
}


UINT CaptureTraceThread1(LPVOID pParam)
{
	CComparisonDlg* pDlg=(CComparisonDlg*)pParam;
	HANDLE hCom=pDlg->hCom1;

	CString dispStr="";
	char str[1024];
	int buflen=1024;

	while(pDlg->bEnableTrace)
	{	
		::Sleep(100);
		//pDlg->StartCaptureTrace();
		
		OVERLAPPED m_osRead;
		memset(&m_osRead,0,sizeof(OVERLAPPED));
		m_osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

		COMSTAT ComStat;
		DWORD dwErrorFlags;		
		
		memset(str,'\0',buflen);
		DWORD dwBytesRead=buflen;//读取的字节数
		BOOL bReadStat;
		
		ClearCommError(hCom,&dwErrorFlags,&ComStat);
		dwBytesRead=min(dwBytesRead, (DWORD)ComStat.cbInQue);
		bReadStat=ReadFile(hCom,str,dwBytesRead,&dwBytesRead,&m_osRead);
		if(!bReadStat)
		{
			if(GetLastError()==ERROR_IO_PENDING)
			//GetLastError()函数返回ERROR_IO_PENDING,表明串口正在进行读操作
			{
				WaitForSingleObject(m_osRead.hEvent,2000);
				//使用WaitForSingleObject函数等待，直到读操作完成或延时已达到2秒钟
				//当串口读操作进行完毕后，m_osRead的hEvent事件会变为有信号
			}
		}

		PurgeComm(hCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		if(strlen(str)==0)
			continue;

		dispStr.Append(str);
		//strcat(dispStr.GetBuffer(),str);
		int n=dispStr.GetLength();
		dispStr.Trim();
		dispStr.Replace("\n","\r\n");
		pDlg->mTraceWnd1=dispStr;//.SetWindowTextA(dispStr);
		if(dispStr.GetLength()>3000)
		{
			CFile traceFile(pDlg->TraceFilePath1, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);
			traceFile.SeekToEnd();
			traceFile.Write(dispStr,dispStr.GetLength());
			dispStr="";
		}
				
		pDlg->GetDlgItem(IDC_EDIT_TRACEWND1)->SetWindowTextA(dispStr);
		//pDlg->GetDlgItem(IDC_EDIT_TRACEWND)->ScrollWindow(10,10);
	}
	CFile traceFile(pDlg->TraceFilePath1, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);
	traceFile.SeekToEnd();
	traceFile.Write(dispStr,dispStr.GetLength());
	traceFile.Close();
	dispStr="";
	if(hCom)
	{
		CloseHandle(hCom);
		hCom=NULL;
	}
	return 0;
}


UINT CaptureTraceThread2(LPVOID pParam)
{
	CComparisonDlg* pDlg=(CComparisonDlg*)pParam;
	HANDLE hCom=pDlg->hCom2;

	CString dispStr="";
	char str[1024];
	int buflen=1024;

	while(pDlg->bEnableTrace)
	{	
		::Sleep(100);
		//pDlg->StartCaptureTrace();
		
		OVERLAPPED m_osRead;
		memset(&m_osRead,0,sizeof(OVERLAPPED));
		m_osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

		COMSTAT ComStat;
		DWORD dwErrorFlags;		
		
		memset(str,'\0',buflen);
		DWORD dwBytesRead=buflen;//读取的字节数
		BOOL bReadStat;
		
		ClearCommError(hCom,&dwErrorFlags,&ComStat);
		dwBytesRead=min(dwBytesRead, (DWORD)ComStat.cbInQue);
		bReadStat=ReadFile(hCom,str,dwBytesRead,&dwBytesRead,&m_osRead);
		if(!bReadStat)
		{
			if(GetLastError()==ERROR_IO_PENDING)
			//GetLastError()函数返回ERROR_IO_PENDING,表明串口正在进行读操作
			{
				WaitForSingleObject(m_osRead.hEvent,2000);
				//使用WaitForSingleObject函数等待，直到读操作完成或延时已达到2秒钟
				//当串口读操作进行完毕后，m_osRead的hEvent事件会变为有信号
			}
		}

		PurgeComm(hCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		if(strlen(str)==0)
			continue;

		dispStr.Append(str);
		//strcat(dispStr.GetBuffer(),str);
		int n=dispStr.GetLength();
		dispStr.Trim();
		dispStr.Replace("\n","\r\n");
		pDlg->mTraceWnd2=dispStr;//.SetWindowTextA(dispStr);
		if(dispStr.GetLength()>3000)
		{
			CFile traceFile(pDlg->TraceFilePath2, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);
			traceFile.SeekToEnd();
			traceFile.Write(dispStr,dispStr.GetLength());
			dispStr="";
		}
				
		pDlg->GetDlgItem(IDC_EDIT_TRACEWND2)->SetWindowTextA(dispStr);
		//pDlg->GetDlgItem(IDC_EDIT_TRACEWND)->ScrollWindow(10,10);
	}
	CFile traceFile(pDlg->TraceFilePath2, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);
	traceFile.SeekToEnd();
	traceFile.Write(dispStr,dispStr.GetLength());
	traceFile.Close();
	dispStr="";
	if(hCom)
	{
		CloseHandle(hCom);
		hCom=NULL;
	}
	return 0;
}
void CComparisonDlg::OnBnClickedBtnProjectsetting()
{
	// TODO: Add your control notification handler code here
	CProjectSettingDlg Dlg;
	if(Dlg.DoModal()==IDOK)
	{
		RefSTRPath=Dlg.mReferenceSTRPath;	
		SignalDBPath=Dlg.mSignalDBPath;
		ProjectName=Dlg.mProjectName;
		Version=Dlg.mVersion;
		NewProjectName=Dlg.mNewProjectFolder;
		RefSTRPath=Dlg.mReferenceSTRPath;
		ProjectFolder=Dlg.mProjectFolder;

		CreateDirectory(ProjectFolder+'\\'+ProjectName,NULL);
		CreateDirectory(ProjectFolder+'\\'+ProjectName+"\\Video",NULL);
		CreateDirectory(ProjectFolder+'\\'+ProjectName+"\\Log",NULL);
		CreateDirectory(ProjectFolder+'\\'+ProjectName+"\\Temp",NULL);

		TempFolder=ProjectFolder+'\\'+ProjectName+"\\Temp";
		gTempPath=TempFolder;
		VideoFolder=ProjectFolder+'\\'+ProjectName+"\\Video";
		LogFolder=ProjectFolder+'\\'+ProjectName+"\\Log";

		CString   strDate; 
		CTime   ttime   =   CTime::GetCurrentTime(); 
		strDate.Format( "%d%02d%02d ",ttime.GetYear(),ttime.GetMonth(),ttime.GetDay()); 

		TestSTRPath=ProjectFolder+'\\'+ProjectName+'\\'+ProjectName+'_'+Version+'_'+strDate+".xls";
		CopyFile(RefSTRPath,TestSTRPath,false);
		CopyFile(RefSTRPath,ProjectFolder+'\\'+ProjectName+"\\"+"Ref_to_"+RefSTRPath.Right(RefSTRPath.GetLength()-RefSTRPath.ReverseFind('\\')),false);
		
		GetDeviceName(SignalDBPath);
		RedratInitial();

		GetDlgItem(IDC_STATIC_TESTSTATUS)->SetWindowTextA("Project Folder:"+ProjectFolder+";  Event:"+ProjectName+";  Version;"+Version+";  STR:"+TestSTRPath+"; ");
	}

}

enum scriptError{noError, normalError, loopError, endError, fileError};

int CComparisonDlg::CheckScript(char* filename)
{
//Check waiting time
	char waitingTime[256];
	char cmd[256];
	char fname[256];

	memset(waitingTime,0,256);
	memset(cmd,0,256);
	memset(fname,0,256);
	//char *projectFolder=ProjectFolder.GetBuffer();

	sscanf(filename,"%s",fname);

	char *pRead1=NULL,*pRead2=NULL;
	int loopFlag=0;
	int endFlag=0;	
	
	FILE *fp=NULL;
	fp=fopen(fname,"r");
	if(fp==NULL)
	{   
		char fname1[256];
		memset(fname1,0,256);
		sprintf(fname1,"%s\\Script\\%s",ProjectFolder.GetBuffer() ,fname);
		//fname=ProjectFolder+'\\'+fname;
		//fname=strcat(strcat(projectFolder,"\\Script\\"),fname);
		fp=fopen(fname1,"r");
		if(fp==NULL)
			return fileError;
	}

	do
	{
		pRead1=fgets(waitingTime,255,fp);
		if(strstr(waitingTime,"//")) continue;
		pRead2=fgets(cmd,255,fp);

		if(pRead1==NULL&&pRead2==NULL)
			break;

		if(pRead1&&pRead2==NULL)
		{
			fclose(fp);
			return normalError;
		}

		if(strstr(cmd,".txt"))
		{	
			int ret=CheckScript(cmd);
			if(ret!=noError)
			{
				fclose(fp);
				return	ret;
			}
		}

		if(strstr(waitingTime,"//"))
			continue;

		else if(atoi(waitingTime)==0)
		{
			fclose(fp);
			return normalError;
		}

		if(strstr(strlwr(cmd),"end")&&!strstr(strlwr(cmd),"loop_end"))
			endFlag++;


		if(strstr(strlwr(cmd),"loop_start"))
		{
			loopFlag++;
			if(loopFlag>1)
			{
				fclose(fp);
				return loopError;
			}
		}

		if(strstr(strlwr(cmd),"loop_end"))
		{
			loopFlag--;
			if(loopFlag<0)
			{
				fclose(fp);
				return loopError;
			}
		}
	}
	while(true);

	if(loopFlag>0||loopFlag<0)
	{
		fclose(fp);
		return loopError;
	}

	if(endFlag!=1)
	{
		fclose(fp);
		return endError;
	}

	fclose(fp);	

	return noError;
}
void CComparisonDlg::OnBnClickedBtnPausetest()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItem(IDC_BTN_PAUSETEST)->GetWindowTextA(str);

	if(pThread)
	{
		if(str=="Pause")
		{
			GetDlgItem(IDC_BTN_PAUSETEST)->SetWindowTextA("Resume");
			GetDlgItem(IDC_BTN_STOPTEST)->EnableWindow(false);
			pThread->SuspendThread();
		}	
		if(str=="Resume")
		{
			GetDlgItem(IDC_BTN_PAUSETEST)->SetWindowTextA("Pause");
			GetDlgItem(IDC_BTN_STOPTEST)->EnableWindow(true);
			pThread->ResumeThread();
		}
	}
}

void CComparisonDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CComparisonDlg::OnBnClickedBtnStoptest()
{
	// TODO: Add your control notification handler code here
	bStopTest=true;
	GetDlgItem(IDC_BTN_STOPTEST)->EnableWindow(false);
	GetDlgItem(IDC_BTN_PAUSETEST)->EnableWindow(false);
	
}

void CComparisonDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CComparisonDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	OnOK();
	//CDialog::OnClose();
}
