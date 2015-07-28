// DSGraph.cpp: implementation of the CDSGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "Comparison.h"

//#include "SampleGrabber.h"
#include "Stdafx.h"
#include "ComparisonDlg.h"
#include "UDsUtils.h"
#include "UFilterUtils.h"
#include <streams.h>
#include "crossbar.h"
#include "CVideoWindow.h"
#include "SampleGrabber.h"


#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDSGraph::CDSGraph()
{
	pGraphBuilder=NULL;
	pCaptureFilter=NULL;
	pCaptureFilterAud=NULL;
	pVidRenderFilter=NULL;
	pAudRenderFilter=NULL;
	pNullRender=NULL;
	pVidGrabberFilter=NULL;
	pAudGrabberFilter=NULL;
	pIVidSampleGrabber=NULL;
	pIAudSampleGrabber=NULL;
	pGraph=NULL;
	pVideoDecoder=NULL;
	pMediaControl=NULL;
	pVideoWindow=NULL;
	pClock = NULL;
	pBasicVideo=NULL;
	pBasicAudio=NULL;
	pVidCBRef=NULL;
	pVidCBTest=NULL;
	//pAudCB=NULL;
	pAudSampleGrabberFilter=NULL;
	pVidSampleGrabberFilterRef=NULL;
	pVidSampleGrabberFilterTest=NULL;
	FailedInfo=_T("");
	pMediaType=NULL;
}

CDSGraph::~CDSGraph()
{


}

BOOL CDSGraph::CreateGraph(char* pDeviceDisplayName,char* pDeviceDisplayNameAudio,HWND inHWnd,BYTE* pBuffer ,int Param)
{	
	HRESULT hr;
		
	WCHAR CapFilterName[256];
	WCHAR CapFilterNameAud[256];
	WCHAR VidRenderName[256];
	WCHAR AudRenderName[256];
	WCHAR VidGrabberName[256];
	
	if(Param==0)
	{
		MultiByteToWideChar(CP_ACP, 0, "Video Capture0", -1, CapFilterName, 256);
		MultiByteToWideChar(CP_ACP, 0, "Audio Capture0", -1, CapFilterNameAud, 256);
		MultiByteToWideChar(CP_ACP, 0, "Video Render0", -1, VidRenderName, 256);
		MultiByteToWideChar(CP_ACP, 0, "Audio Render0", -1, AudRenderName, 256);
		MultiByteToWideChar(CP_ACP, 0, "Grabber Filter0", -1, VidGrabberName, 256);	
	}
	else
	{
		MultiByteToWideChar(CP_ACP, 0, "Video Capture1", -1, CapFilterName, 256);
		MultiByteToWideChar(CP_ACP, 0, "Audio Capture1", -1, CapFilterNameAud, 256);
		MultiByteToWideChar(CP_ACP, 0, "Video Render1", -1, VidRenderName, 256);
		MultiByteToWideChar(CP_ACP, 0, "Audio Render1", -1, AudRenderName, 256);
		MultiByteToWideChar(CP_ACP, 0, "Grabber Filter1", -1, VidGrabberName, 256);	
	}
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,IID_IGraphBuilder, (void **)&pGraph);	
	pCaptureFilter=UDsUtils::CreateHardwareFilter(AM_KSCATEGORY_CAPTURE,pDeviceDisplayName,pDeviceFriendlyName);//CLSID_VideoInputDeviceCategory
	hr = pGraph->AddFilter(pCaptureFilter,CapFilterName);	
	if(!SUCCEEDED(hr))
	{
		FailedInfo="Failed to create Capture Filter!";
	//	return FALSE;
	}
		
	hr = CoCreateInstance((REFCLSID)CLSID_CaptureGraphBuilder2,0,CLSCTX_INPROC, (REFIID)IID_ICaptureGraphBuilder2, (void **)&pGraphBuilder);	
	hr = pGraphBuilder->SetFiltergraph(pGraph);
	if(!SUCCEEDED(hr))
	{
		FailedInfo="Failed to create CaptureGraphBuilder2!";
		return FALSE;
	}

	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter, (void **)&pNullRender);
	hr = pGraph->AddFilter(pNullRender,L"Null Render");	
	if(!SUCCEEDED(hr))
	{
		FailedInfo="Failed to create Null Renderer!";
		return FALSE;
	}

	GUID CLSID_VMR9 = {	0x51B4ABF3, 0x748F, 0x4E3B, 0xA2, 0x76, 0xC8, 0x28, 0x33, 0x0E, 0x92, 0x6A};			
	hr = CoCreateInstance(CLSID_VMR9, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter, (void **)&pVidRenderFilter);
	hr = pGraph->AddFilter(pVidRenderFilter,VidRenderName);	
	if(!SUCCEEDED(hr))
	{
		FailedInfo="Failed to create Video Renderer!";
		return FALSE;
	}
				
	if(Param==0)
	{
		hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter, reinterpret_cast<void**>(&pVidGrabberFilter));
		hr = pGraph->AddFilter(pVidGrabberFilter, L"Video SampleGrabber");
		pVidGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&pIVidSampleGrabber);
		pVidCBRef = new CVidSampleGrabberRefCB;
		pVidCBRef->mGraphID=Param;
		pVidCBRef->Counting=0;	
		hr = pIVidSampleGrabber->SetBufferSamples( TRUE );
		hr = pIVidSampleGrabber->SetOneShot( FALSE );
		hr = pIVidSampleGrabber->SetCallback( pVidCBRef, 1 );
		hr = pIVidSampleGrabber->SetMediaType(NULL);
		hr = pVidGrabberFilter->SetSyncSource(NULL);
	}
	else
	{   hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter, reinterpret_cast<void**>(&pVidGrabberFilter));
		hr = pGraph->AddFilter(pVidGrabberFilter, L"Video SampleGrabber");
		pVidGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&pIVidSampleGrabber);
		pVidCBTest = new CVidSampleGrabberTestCB;
		pVidCBTest->mGraphID=Param;
		pVidCBTest->Counting=0;	
		hr = pIVidSampleGrabber->SetBufferSamples( TRUE );
		hr = pIVidSampleGrabber->SetOneShot( FALSE );
		hr = pIVidSampleGrabber->SetCallback( pVidCBTest, 1 );
		hr = pIVidSampleGrabber->SetMediaType(NULL);
		hr = pVidGrabberFilter->SetSyncSource(NULL);
	}

	/*
	if(Param==0)
	{
		hr=S_OK;
		pVidSampleGrabberFilterRef=new CVidSampleGrabberRef(NULL,&hr,Param);
		pVidSampleGrabberFilterRef->AddRef();
		pVidSampleGrabberFilterRef->QueryInterface(IID_IBaseFilter,(void**)&pVidGrabberFilter);
		pVidGrabberFilter->Release();
		pGraph->AddFilter(pVidGrabberFilter,L"Video SampleGrabber");
	}
	else
	{
		hr=S_OK;
		pVidSampleGrabberFilterTest=new CVidSampleGrabberTest(NULL,&hr,Param);
		pVidSampleGrabberFilterTest->AddRef();
		pVidSampleGrabberFilterTest->QueryInterface(IID_IBaseFilter,(void**)&pVidGrabberFilter);
		pVidGrabberFilter->Release();
		pGraph->AddFilter(pVidGrabberFilter,L"Video SampleGrabber");
	}

	*/
	if(!SUCCEEDED(hr))
	{
		FailedInfo="Failed to create Video Sample Grabber!";
		return FALSE;
	}
						
	hr = pGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,	&MEDIATYPE_Video,pCaptureFilter, pVidGrabberFilter, pVidRenderFilter);	
	//hr = pGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,	&MEDIATYPE_Video,pCaptureFilter, pVidGrabberFilter, pVidRenderFilter);	
	
	if(!SUCCEEDED(hr))
	{
		hr = pGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,	&MEDIATYPE_Video,pCaptureFilter, pVidGrabberFilter, pVidRenderFilter);	
		if(!SUCCEEDED(hr))
		{
			FailedInfo="Failed to render video stream!";
			return FALSE;
		}
	}

	hr = pCaptureFilter->QueryInterface(IID_IAMAnalogVideoDecoder, (void**)&pVideoDecoder);
	pVideoDecoder->put_TVFormat(AnalogVideo_PAL_B);	

	if(!SUCCEEDED(hr))
	{
		FailedInfo="Failed to setup video format!";
		return FALSE;
	}

//Audio////////////////////////////

	if(pDeviceDisplayNameAudio==""||pDeviceDisplayNameAudio==NULL)
	{
		pCaptureFilterAud=pCaptureFilter;
	}
	else
	{
		pCaptureFilterAud=UDsUtils::CreateHardwareFilter(CLSID_AudioInputDeviceCategory,pDeviceDisplayNameAudio,pDeviceFriendlyName);//CLSID_VideoInputDeviceCategory
		hr = pGraph->AddFilter(pCaptureFilterAud,CapFilterNameAud);			
	}

	hr=S_OK;
	pAudSampleGrabberFilter=new CAudSampleGrabber(NULL,&hr,Param);
	pAudSampleGrabberFilter->AddRef();
	pAudSampleGrabberFilter->QueryInterface(IID_IBaseFilter,(void**)&pAudGrabberFilter);
	pAudGrabberFilter->Release();
	hr = pGraph->AddFilter(pAudGrabberFilter,L"Audio SampleGrabber");		
	pAudSampleGrabberFilter;

	//hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter, reinterpret_cast<void**>(&pAudGrabberFilter));
	//hr = pGraph->AddFilter(pAudGrabberFilter, L"Audio SampleGrabber");
	//pAudGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&pIAudSampleGrabber);
	//pAudCB = new CAudSampleGrabberCB;
	//pAudCB->mGraphID=Param;
	//pAudCB->InitGrabberCB();	
	//hr = pIAudSampleGrabber->SetBufferSamples( TRUE );
	//hr = pIAudSampleGrabber->SetOneShot( FALSE );
	//hr = pIAudSampleGrabber->SetCallback( pAudCB, 1 );
	//hr = pIAudSampleGrabber->SetMediaType(NULL);
	//hr = pAudGrabberFilter->SetSyncSource(NULL);

	//CLSID_DSoundRender

//	pAudGrabberFilter=NULL;

	if(Param==0)
		hr = CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter, (void **)&pAudRenderFilter);
	else
		hr = CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter, (void **)&pAudRenderFilter);
	hr = pGraph->AddFilter(pAudRenderFilter,AudRenderName);
	
	if(Param==0)
		hr = pGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio,pCaptureFilterAud,pAudGrabberFilter,pAudRenderFilter);// pAudGrabberFilter, pAudRenderFilter);	
	else
		hr = pGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio,pCaptureFilterAud,pAudGrabberFilter,pAudRenderFilter);// pAudGrabberFilter, pAudRenderFilter);	

	if(hr!=S_OK)
	{		
		FailedInfo="Failed to render audio stream!";				
		return FALSE;
	}	
		
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pMediaControl);
	hr = pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVideoWindow);

	hr = pVideoWindow->put_Owner((OAHWND)inHWnd);	
	hr = pVideoWindow->put_Left(0);
	hr = pVideoWindow->put_Top(0);
	hr = pVideoWindow->put_Width(360);
	hr = pVideoWindow->put_Height(288);
	hr = pVideoWindow->put_WindowStyle(WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
	hr = pVideoWindow->put_Visible(OATRUE);	
	hr = pVideoWindow->put_MessageDrain((OAHWND) inHWnd);
	if(!SUCCEEDED(hr))
	{
		FailedInfo="Failed to create video window!";
		return FALSE;
	}
	hr = pGraph->QueryInterface(IID_IBasicVideo, (void **)&pBasicVideo);
	if(!SUCCEEDED(hr))
	{
		FailedInfo="Failed to create basic video!";
		return FALSE;
	}
	hr = pGraph->QueryInterface(IID_IBasicAudio, (void **)&pBasicAudio);
	pBasicAudio->put_Volume(0);
	if(!SUCCEEDED(hr))
	{
		FailedInfo="Failed to create basic audio!";
		return FALSE;
	}

		//VIDEO_STREAM_CONFIG_CAPS  pSCC;
		//IAMStreamConfig * pStreamConfig;
		//hr = pGraphBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,pCaptureFilter,IID_IAMStreamConfig, (void **)&pStreamConfig);
		//pStreamConfig->Release();
		//hr = pStreamConfig->GetStreamCaps(0, &pMediaType, (BYTE *)&pSCC);
		//	LONGLONG avgTimePerFrame = 400000;  // 25fps

		//if (pMediaType->formattype == FORMAT_VideoInfo) 
		//{
		//	long width, height;
		//	((VIDEOINFOHEADER *)(pMediaType->pbFormat))->AvgTimePerFrame       = avgTimePerFrame;
		//	((VIDEOINFOHEADER *)(pMediaType->pbFormat))->bmiHeader.biWidth     = 720;//width;
		//	((VIDEOINFOHEADER *)(pMediaType->pbFormat))->bmiHeader.biHeight    = 576;//height;
		//	((VIDEOINFOHEADER *)(pMediaType->pbFormat))->bmiHeader.biSizeImage = 720 * 576 * ((VIDEOINFOHEADER *)(pMediaType->pbFormat))->bmiHeader.biBitCount / 8;
		//	
		//	IAMStreamConfig * pStreamConfig;
		//hr = pGraphBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,pCaptureFilter,IID_IAMStreamConfig, (void **)&pStreamConfig);
		//pStreamConfig->Release();

		//	hr = pStreamConfig->SetFormat(pMediaType);
		//}
	//	DeleteMediaType(pmt); //  Delete by ZHOU Wei

  //IReferenceClock *pClock = 0;
  //hr = CoCreateInstance( CLSID_SystemClock,0,CLSCTX_INPROC_SERVER,IID_IReferenceClock,reinterpret_cast<void**>(&pClock) );
  //if( hr == S_OK )
  //{
  // IMediaFilter*   pMediaFilter=0;   
  // hr   =   pGraph->QueryInterface(IID_IMediaFilter,(void**)&pMediaFilter);   
  // if(SUCCEEDED(hr))   
  // {   
  //  if(S_OK==pMediaFilter->SetSyncSource(pClock))
  //  {

  //  }
  //  SAFE_RELEASE(pMediaFilter);   
  // }
  //}



	//hr = pGraph->SetDefaultSyncSource();  
	//hr = pAudRenderFilter->QueryInterface(IID_IReferenceClock,(void**)&pClock);
	//hr = pVidRenderFilter->SetSyncSource(pClock);
	//hr = pAudRenderFilter->SetSyncSource(pClock);

	//hr = pVidGrabberFilter->SetSyncSource(pClock);
	//hr = pAudGrabberFilter->SetSyncSource(pClock); 
	//hr = pCaptureFilter->SetSyncSource(pClock);

	//IMediaFilter *pMediaFilter;
	//hr = pGraph->QueryInterface(IID_IMediaFilter, (void**)&pMediaFilter);
	//hr = pMediaFilter->SetSyncSource(pClock);
	//pClock->Release();
	//pMediaFilter->Release();

	return TRUE;

}

BOOL CDSGraph::DeleteGraph(int Param)
{
	HRESULT hr;
	if(pMediaControl)
		hr = pMediaControl->Stop();
	if(pVideoWindow)
	{
		hr = pVideoWindow->put_Visible(OAFALSE);
		hr = pVideoWindow->put_Owner(OAHWND(0));
	}
	if(pGraph)
	{
		hr = pGraph->RemoveFilter(pCaptureFilter);
		hr = pGraph->RemoveFilter(pVidGrabberFilter);
		hr = pGraph->RemoveFilter(pVidRenderFilter);
		hr = pGraph->RemoveFilter(pAudGrabberFilter);
		hr = pGraph->RemoveFilter(pAudRenderFilter);
		hr = pGraph->RemoveFilter(pAudSampleGrabberFilter);
	}
//	hr = pCaptureFilter->Release();//
	SAFE_RELEASE(pVidGrabberFilter);
	SAFE_RELEASE(pVidGrabberFilter);
	SAFE_RELEASE(pVidRenderFilter);
	SAFE_RELEASE(pAudGrabberFilter);
	SAFE_RELEASE(pAudRenderFilter);
	SAFE_RELEASE(pGraphBuilder);
//	SAFE_RELEASE(pAudSampleGrabberFilter);
//	if(pAudSampleGrabberFilter)
//		delete pAudSampleGrabberFilter;
	if(pVidCBRef)
		delete pVidCBRef;
	if(pVidCBTest)
		delete pVidCBTest;
//	if(pAudSampleGrabberFilter)
//		delete pAudSampleGrabberFilter;
//	
//		delete pAudSampleGrabberFilter;

//	if(pVidCB)
//		pVidCB->UnInitGrabberCB();
//	SAFE_DELETE(pVidCB);
//	if(pAudCB)
//		pAudCB->UnInitGrabberCB();
//	SAFE_DELETE(pAudCB);

	return TRUE;

}


BOOL CDSGraph::GetCurrentImage(const char * inFile)
{
	if (!pBasicVideo)
	{
		return FALSE;
	}
	
	long bitmapSize = 0;
	if (SUCCEEDED(pBasicVideo->GetCurrentImage(&bitmapSize, 0)))
	{
		BOOL pass = FALSE;
		BYTE * buffer = new BYTE[bitmapSize];
		if (SUCCEEDED(pBasicVideo->GetCurrentImage(&bitmapSize, (long *)buffer)))
		{
			BITMAPFILEHEADER	hdr;
			LPBITMAPINFOHEADER	lpbi;
			
			lpbi = (LPBITMAPINFOHEADER)buffer;
			
    		hdr.bfType		= ((WORD) ('M' << 8) | 'B'); // always is "BM"
			hdr.bfSize		= bitmapSize + sizeof( hdr );
			hdr.bfReserved1 = 0;
			hdr.bfReserved2 = 0;
			hdr.bfOffBits	 = (DWORD) (sizeof(BITMAPFILEHEADER) + lpbi->biSize);
			
			CFile bmpFile(inFile, CFile::modeReadWrite | CFile::modeCreate | CFile::typeBinary);
			bmpFile.Write(&hdr, sizeof(BITMAPFILEHEADER));
			bmpFile.Write(buffer, bitmapSize);
			bmpFile.Close();
			pass = TRUE;
		}
		delete [] buffer;
		return pass;
	}

	return TRUE;
}



BOOL CDSGraph::ChangeTVFormat(LONG SetFormat)
{
	HRESULT hr;
//	hr = pMediaControl->Stop();
	hr = pVideoDecoder->put_TVFormat(SetFormat);
//	hr = pMediaControl->Run();
	if(hr!=S_OK)
	{
		AfxMessageBox("Failed to change TV format!");
		return FALSE;
	}
	return TRUE;
}



BOOL CDSGraph::ChangeVolume()
{
	LONG Volume;
	pBasicAudio->get_Volume(&Volume);
	//if(Volume==0)
	//{
	//	pBasicAudio->put_Volume(-10000);
	//	return FALSE;
	//}
	//else
	//{
	//	pBasicAudio->put_Volume(0);
		return TRUE;
	//}
}

BOOL CDSGraph::ChangeBalance(LONG Balance)
{
	if(pBasicAudio)
		pBasicAudio->put_Balance(Balance);
	return TRUE;
}

BOOL CDSGraph::CaptureSetting()
{
	ISpecifyPropertyPages * pSpecify;
	HRESULT hr = pCaptureFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify);
	if (SUCCEEDED(hr))
	{
		CAUUID   caGUID;
		pSpecify->GetPages(&caGUID);
		pSpecify->Release();
		
		OleCreatePropertyFrame(
			::GetActiveWindow(),        // Parent window
			0,                          // x (Reserved)
			0,                          // y (Reserved)
			NULL,                       // Caption for the dialog box
			1,                          // Number of filters
			(IUnknown **) &pCaptureFilter,    // Pointer to the filter
			caGUID.cElems,              // Number of property pages
			caGUID.pElems,              // Pointer of property page CLSIDs
			0,                          // Locale identifier
			0,                          // Reserved
			NULL                        // Reserved
			);
		CoTaskMemFree(caGUID.pElems);
		return TRUE;
	}
	return FALSE;
}

BOOL CDSGraph::StartGraph()
{
	HRESULT hr;
	hr = pMediaControl->Run();
	if(SUCCEEDED(hr))
		return TRUE;
	else
		return FALSE;
}

