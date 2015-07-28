// DSGraph.h: interface for the CDSGraph class.
//
//////////////////////////////////////////////////////////////////////
#include <streams.h>
#include <qedit.h>
#include "ComparisonDlg.h"
#include "SampleGrabber.h"
#include "UDsUtils.h"
//#include "FilterImageGrabber\IImageGrabber.h"

#if !defined(AFX_DSGRAPH_H__9E8BE770_3F99_4EFA_BEA6_EF769FC65063__INCLUDED_)
#define AFX_DSGRAPH_H__9E8BE770_3F99_4EFA_BEA6_EF769FC65063__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//GUID 	CLSID_VMR9 = {0x51B4ABF3, 0x748F, 0x4E3B, 0xA2, 0x76, 0xC8, 0x28, 0x33, 0x0E, 0x92, 0x6A};


	
class CDSGraph  
{
public:
	CDSGraph();
	virtual ~CDSGraph();
	static void GetDeviceInfo();

private:
		
	IBaseFilter* pVidRenderFilter;
	IBaseFilter* pAudRenderFilter;
	IBaseFilter* pVidGrabberFilter;
	IBaseFilter* pAudGrabberFilter;
	IBaseFilter* pNullRender;
	IBasicVideo * pBasicVideo;
	IBasicAudio * pBasicAudio;
	CVidSampleGrabberRefCB *pVidCBRef;	
	CVidSampleGrabberTestCB *pVidCBTest;	
	//CAudSampleGrabberCB *pAudCB;
	CAudSampleGrabber* pAudSampleGrabberFilter;
	CVidSampleGrabberRef* pVidSampleGrabberFilterRef;
	CVidSampleGrabberTest* pVidSampleGrabberFilterTest;

	ISampleGrabber* pIVidSampleGrabber;
	ISampleGrabber* pIAudSampleGrabber;
	IGraphBuilder *     pGraph;
	IAMAnalogVideoDecoder *	pVideoDecoder;
	IVideoWindow* pVideoWindow;
	ICaptureGraphBuilder2* pGraphBuilder;
	IBaseFilter* pCaptureFilter;
	IBaseFilter* pCaptureFilterAud;
	IMediaControl *		pMediaControl;
	IReferenceClock *pClock;
	AM_MEDIA_TYPE* pMediaType;

public:
	char pDeviceFriendlyName[256];
	CString FailedInfo;
	
		
	
	BOOL CreateGraph(char* pDeviceDisplayName,char* pDeviceDisplayNameAudio,HWND inHWnd,BYTE* pBuffer,int Param);
	BOOL DeleteGraph(int Param);
	BOOL GetCurrentImage(const char * inFile);
	BOOL ChangeTVFormat(LONG SetFormat);
	BOOL ChangeVolume();
	BOOL ChangeBalance(LONG Balance);
	BOOL CaptureSetting();
	BOOL StartGraph();
		
};

#endif // !defined(AFX_DSGRAPH_H__9E8BE770_3F99_4EFA_BEA6_EF769FC65063__INCLUDED_)
