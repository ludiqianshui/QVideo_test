//------------------------------------------------------------------------------
// File: SampGrabCB.cpp
//
// Desc: DirectShow sample code - C++ console application demonstrating
//       use of the IMediaDet interface to create a graph that contains a
//       sample grabber filter.  It shows how to use the sample grabber 
//       and a COM object callback to display information about media
//       samples in a running video file.  
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "SampleGrabber.h"
#include "qedit.h"
#include "math.h"


bool StartCheck=false;
bool RefImageReady=false;
bool TestImageReady=false;

int CheckPointNum=0;
#define  CHECKFRAMES 25

extern "C" _declspec(dllimport)  int VidCompareBuf(BYTE* Buf, int Param);
extern "C" _declspec(dllimport)  int AudCompareBuf(BYTE* Buf, int Lenth, int Param);

double  GetCorr(int* Ref,int* Test,int length);
double GetImageCorr(BYTE* RefImage,BYTE* TestImage,int width, int height);
bool ImageCompareByGrayscale(void);
bool ImageComaparebyPix(BYTE* RefImage,BYTE* TestImage, int width, int height);
extern CString gTempPath;
//CFile* myFileRef=NULL;
//CFile* myFileTest=NULL;

	
int TestRef=0,TestTest=0;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//video///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CVidSampleGrabberRef::CVidSampleGrabberRef(LPUNKNOWN punk,  HRESULT *phr, LONG inParam) : 
CTransInPlaceFilter(NAME("VIDEO SAMPLE GRABBER"), punk, CLSID_NULL, phr)
{
	Param=inParam;
	Counting=0;
	CheckPointNum=0;
	myFile=NULL;
}
CVidSampleGrabberRef::~CVidSampleGrabberRef()
{
	if(myFile)
		delete myFile;

}
HRESULT CVidSampleGrabberRef::CheckInputType(const CMediaType* mtIn)
{
	// Dynamic changing media type is not allowed!
	return NOERROR;
}


HRESULT CVidSampleGrabberRef::Transform(IMediaSample *pSample)
{
	HRESULT hr;
	
	if(StartCheck&&!RefImageReady)	
	{

		LONGLONG time,time1,start,end;
		int size, i,j;
		BYTE* buf;
		pSample->GetPointer(&buf);
		size=pSample->GetSize();	
		CAutoLock lck(&mSafeCapture); 		
		{
		if(Counting==0)//first inital
		{	
			CString str;	
			str.Format("%s\\Ref_%d.yuv",gTempPath,CheckPointNum);
			hFile= CreateFile(str,  GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			////hFile= CreateFile(str,  GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			myFile = new CFile(hFile);
			//fp=fopen(str,"wb");
			start=true;
		}
		if(Counting%3==0)
		{
			if(myFile)
				{myFile->Write(buf,size);myFile->Flush();}
			//::Sleep(1);
			//fwrite(buf,size,1,fp);
			//fflush(fp);

		}
		Counting++;

		TestRef=Counting;
		if(Counting>=CHECKFRAMES)
		{			
			if(myFile) {myFile->Close();delete myFile;myFile=NULL;}
			//fclose(fp);
			Counting=0;
			RefImageReady=true;
			
			if(RefImageReady&&TestImageReady)
			{
				StartCheck=false;
				RefImageReady=false;
				TestImageReady=false;
				CheckPointNum++;
				if(myFile) {myFile->Close();delete myFile;myFile=NULL;}
				//if(myFileTest) {myFileTest->Close();myFileTest=NULL;}
				//CString str;
				//str.Format("Finished in Ref\nRef:%d Test:%d",TestRef,TestTest);
				//AfxMessageBox(str);
			}			
		}
		}
	}
		
	return NOERROR;
}

////////////////////////////////////////////

CVidSampleGrabberTest::CVidSampleGrabberTest(LPUNKNOWN punk,  HRESULT *phr, LONG inParam) : 
CTransInPlaceFilter(NAME("VIDEO SAMPLE GRABBER"), punk, CLSID_NULL, phr)
{
	Param=inParam;
	Counting=0;
	CheckPointNum=0;
	myFile=NULL;
}
CVidSampleGrabberTest::~CVidSampleGrabberTest()
{
	if(myFile)
		delete myFile;
}
HRESULT CVidSampleGrabberTest::CheckInputType(const CMediaType* mtIn)
{
	// Dynamic changing media type is not allowed!
	return NOERROR;
}


HRESULT CVidSampleGrabberTest::Transform(IMediaSample *pSample)
{
	HRESULT hr;
	
	if(StartCheck&&!RefImageReady)
	{
		LONGLONG time,time1,start,end;
		int size, i,j;
		BYTE* buf;
		pSample->GetPointer(&buf);		
		AM_MEDIA_TYPE *pmt;
		size=pSample->GetSize();
		CAutoLock lck(&mSafeCapture); 	
		{
			if(Counting==0)//first inital
			{	
				CString str;	
				str.Format("%s\\Test_%d.yuv",gTempPath,CheckPointNum);
				//str=str;
				hFile= CreateFile(str,	GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				//hFile= CreateFile(str,GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				myFile = new CFile(hFile);
				//fp=fopen(str,"wb");
				start=true;
			}

			if(Counting%4==0)
			{
				if(myFile) {myFile->Write(buf,size);myFile->Flush();}
				//fwrite(buf,size,1,fp);
				//fflush(fp);
			}
			Counting++;

			TestTest=Counting;
			if(Counting>=CHECKFRAMES)
			{			
				if(myFile) {myFile->Close();delete myFile;myFile=NULL;}
				//fclose(fp);
				Counting=0;
				TestImageReady=true;

				if(RefImageReady&&TestImageReady)
				{
					StartCheck=false;
					RefImageReady=false;
					TestImageReady=false;
					CheckPointNum++;
					if(myFile) {myFile->Close();delete myFile;myFile=NULL;}
					//if(myFileTest) {myFileTest->Close();myFileTest=NULL;}
					//CString str;
					//str.Format("Finished in Test\nRef:%d Test:%d",TestRef,TestTest);
					//AfxMessageBox(str);
				}			
			}
		}
	}

	return NOERROR;
}

//////////////////////////////////////////////////////
//Audio///////////////////////////////////////////////
//////////////////////////////////////////////////////
CAudSampleGrabber::CAudSampleGrabber(LPUNKNOWN punk,  HRESULT *phr, LONG inParam) : 
CTransInPlaceFilter(NAME("SAMPLE GRABBER"), punk, CLSID_NULL, phr)
{
	Param=inParam;
	
	//this->m_bModifiesData=FALSE;
//	CString PCMFile;
//	PCMFile.Format("%s\\aud%d.pcm",GetTempPath(),Param);
//	fp=fopen("D:\\test1\\aud.xls","w");
	//hFile= CreateFile(PCMFile,	GENERIC_WRITE, FILE_SHARE_WRITE,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//hFile= CreateFile(PCMFile,GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	myFile = NULL;
	AudCapReady=false;
	Counting=0;
}
CAudSampleGrabber::~CAudSampleGrabber()
{
	//if(hFile)
	//	CloseHandle(hFile);
	if(myFile) {myFile->Close();delete myFile;myFile=NULL;}		
	//if(myFile)
	//	delete myFile;
}
HRESULT CAudSampleGrabber::CheckInputType(const CMediaType* mtIn)
{
	// Dynamic changing media type is not allowed!

	return NOERROR;
}

HRESULT CAudSampleGrabber::Transform(IMediaSample *pSample)
{

	HRESULT hr;
	if(StartCheck==false)
		return NOERROR;
	if(AudCapReady==false)
		AudCapReady=StartCheck;

	if(AudCapReady)	
	{
		int size, i,j;
		BYTE* buf;
		pSample->GetPointer(&buf);
		size=pSample->GetActualDataLength();	
		CAutoLock lck(&mSafeCapture); 		
		{
			if(Counting==0)//first inital
			{	
				CString str;	
				str.Format("%s\\Aud_%d_%d.pcm",gTempPath,CheckPointNum,Param);
				hFile= CreateFile(str,  GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				myFile = new CFile(hFile);
				//myFile=new CFile(str, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);
			}

			if(myFile)
				{myFile->Write(buf,size);myFile->Flush();}

			Counting++;	

			if(Counting>=3)
			{			
				if(myFile) {myFile->Close();delete myFile;myFile=NULL;}			
				Counting=0;		
				AudCapReady=false;
			}
		}
	}
	return NOERROR;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


STDMETHODIMP CVidSampleGrabberRefCB:: QueryInterface(REFIID riid, void ** ppv)
{
	//        CheckPointer(ppv, E_POINTER);

	if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) 
	{
		*ppv = (void *) static_cast<ISampleGrabberCB *>(this);
		return NOERROR;
	}    
	return E_NOINTERFACE;
}

STDMETHODIMP CVidSampleGrabberRefCB:: SampleCB( double SampleTime, IMediaSample * pSample )
{

	return 0;
}

STDMETHODIMP CVidSampleGrabberRefCB::BufferCB( double SampleTime, BYTE * pBuffer, long BufferLen )
{
		HRESULT hr;
	
	if(StartCheck&&!RefImageReady)
	{	
		CAutoLock lck(&mSafeCapture); 	
		{
			if(Counting==0)//first inital ref
			{	
				CString str;	
				str.Format("%s\\Ref_%d.yuv",gTempPath,CheckPointNum);;
				hFile= CreateFile(str,	GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				//hFile= CreateFile(str,GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				myFile = new CFile(hFile);
				
			}

			if(Counting%2==0)
			{
				if(myFile) {myFile->Write(pBuffer,BufferLen);myFile->Flush();}
			}
			Counting++;

			if(Counting>=CHECKFRAMES)
			{			
				if(myFile) {myFile->Close();delete myFile;myFile=NULL;}
				Counting=0;
				RefImageReady=true;

				if(RefImageReady&&TestImageReady)
				{
					StartCheck=false;
					RefImageReady=false;
					TestImageReady=false;
					CheckPointNum++;
					if(myFile) {myFile->Close();delete myFile;myFile=NULL;}
				}			
			}
		}
	}

	return NOERROR;

}

///////////////////////////////////////////

STDMETHODIMP CVidSampleGrabberTestCB:: QueryInterface(REFIID riid, void ** ppv)
{
	//        CheckPointer(ppv, E_POINTER);

	if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) 
	{
		*ppv = (void *) static_cast<ISampleGrabberCB *>(this);
		return NOERROR;
	}    
	return E_NOINTERFACE;
}

STDMETHODIMP CVidSampleGrabberTestCB:: SampleCB( double SampleTime, IMediaSample * pSample )
{

	return 0;
}

STDMETHODIMP CVidSampleGrabberTestCB::BufferCB( double SampleTime, BYTE * pBuffer, long BufferLen )
{
		HRESULT hr;
	
	if(StartCheck&&!TestImageReady)
	{
		CAutoLock lck(&mSafeCapture); 	
		{
			if(Counting==0)//first inital test
			{	
				CString str;	
				str.Format("%s\\Test_%d.yuv",gTempPath,CheckPointNum);
				hFile= CreateFile(str,	GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				//hFile= CreateFile(str,GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				myFile = new CFile(hFile);
			
			}

			if(Counting%3==0)
			{
				if(myFile) {myFile->Write(pBuffer,BufferLen);myFile->Flush();}
			}
			Counting++;

			if(Counting>=CHECKFRAMES)
			{			
				if(myFile) {myFile->Close();delete myFile;myFile=NULL;}
				Counting=0;
				TestImageReady=true;

				if(RefImageReady&&TestImageReady)
				{
					StartCheck=false;
					RefImageReady=false;
					TestImageReady=false;
					CheckPointNum++;
					if(myFile) {myFile->Close();delete myFile;myFile=NULL;}
				}			
			}
		}
	}

	return NOERROR;
}