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


extern "C" _declspec(dllimport)  int VidCompareBuf(BYTE* Buf, int Param);
extern "C" _declspec(dllimport)  int AudCompareBuf(BYTE* Buf, int Lenth, int Param);

BYTE* pRefImage[CHECKFRAMES];
BYTE* pTestImage[CHECKFRAMES];
	

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//video///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CVidSampleGrabberRef::CVidSampleGrabberRef(LPUNKNOWN punk,  HRESULT *phr, LONG inParam) : 
CTransInPlaceFilter(NAME("VIDEO SAMPLE GRABBER"), punk, CLSID_NULL, phr)
{
	Param=inParam;
	
	for(int i=0;i<CHECKFRAMES;i++)
	{
		pReferImage[i]=new BYTE[829440];
		pTestImage[i]=new BYTE[829440];
	}
	
	Counting=0;
	mFrameCounter=CHECKFRAMES;
	CheckPointNum=0;
}
CVidSampleGrabberRef::~CVidSampleGrabberRef()
{

		
	for(int i=0;i<CHECKFRAMES;i++)
	{
		delete pRefImage[i];
		delete pTestImage[i];
	}
}
HRESULT CVidSampleGrabberRef::CheckInputType(const CMediaType* mtIn)
{
	// Dynamic changing media type is not allowed!
	return NOERROR;
}


HRESULT CVidSampleGrabberRef::Transform(IMediaSample *pSample)
{
	HRESULT hr;

	if(StartCheck&&!RefImageReady)	{

		LONGLONG time,time1,start,end;
		int size, i,j;
		BYTE* buf;
		//BYTE bufYUV[829440];
		pSample->GetPointer(&buf);
		
		AM_MEDIA_TYPE *pmt;
		pSample->GetMediaType(&pmt);
		size=pSample->GetSize();	
		//memset(bufYUV,128,829440);

		CAutoLock lck(&mSafeCapture); 		

		/*for(i=0;i<576;i++)
		{
			for(j=0;j<720;j++)
			{
	    		bufYUV[i*720+j]=buf[i*2048+j*2+1];					
				bufYUV[(i/2)*360+j/2+414720]=buf[(i/2)*4096+(j/2)*4];
				bufYUV[(i/2)*360+j/2+518400]=buf[(i/2)*4096+(j/2)*4+2];
			}
		}*/

		if(Counting==0)//first inital
		{	
			CString str;	
			str.Format("Ref_%d.yuv",CheckPointNum);
			hFile= CreateFile(PCMFile,	GENERIC_WRITE, FILE_SHARE_WRITE,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			hFile= CreateFile(PCMFile,GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			myFile = new CFile(hFile);
			start=true;
		}
		myFile->Write(buf,size);
		Counting++;
		if(Counting==CHECKFRAMES)
		{
			RefImageReady=true;
			myFile->Close();	
			Counting=0;
			if(hFile)
				CloseHandle(hFile);
			if(myFile)
				delete myFile;
			
			if(RefImageReady&&TestImageReady)
			{
				StartCheck=false;
				ImageCompareByGrayscale();
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
	
	for(int i=0;i<CHECKFRAMES;i++)
	{
		pReferImage[i]=new BYTE[829440];
		pTestImage[i]=new BYTE[829440];
	}
	
	Counting=0;
	mFrameCounter=CHECKFRAMES;
	CheckPointNum=0;
}
CVidSampleGrabberTest::~CVidSampleGrabberTest()
{
	if(hFile)
		CloseHandle(hFile);
	if(myFile)
		delete myFile;
		
	for(int i=0;i<CHECKFRAMES;i++)
	{
		delete pRefImage[i];
		delete pTestImage[i];
	}
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
		BYTE bufYUV[829440];
		pSample->GetTime(&time,&time1);
		pSample->GetPointer(&buf);
		
		AM_MEDIA_TYPE *pmt;
		pSample->GetMediaType(&pmt);
		size=pSample->GetSize();	
		//memset(bufYUV,128,829440);

		CAutoLock lck(&mSafeCapture); 	
		

		/*for(i=0;i<576;i++)
		{
			for(j=0;j<720;j++)
			{
	    		bufYUV[i*720+j]=buf[i*2048+j*2+1];					
				bufYUV[(i/2)*360+j/2+414720]=buf[(i/2)*4096+(j/2)*4];
				bufYUV[(i/2)*360+j/2+518400]=buf[(i/2)*4096+(j/2)*4+2];
			}
		}*/
	
		if(Counting==0)//first inital
		{	
			CString str;	
			str.Format("Test_%d.yuv",CheckPointNum);
			hFile= CreateFile(PCMFile,	GENERIC_WRITE, FILE_SHARE_WRITE,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			hFile= CreateFile(PCMFile,GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			myFile = new CFile(hFile);
			start=true;
		}
		myFile->Write(buf,size);
		Counting++;
		if(Counting==CHECKFRAMES)
		{
			TestImageReady=true;
			myFile->Close();	
			Counting=0;

			if(hFile)
				CloseHandle(hFile);
			if(myFile)
				delete myFile;
			
			if(RefImageReady&&TestImageReady)
			{
				StartCheck=false;
				//ImageCompareByGrayscale();
			}			
		}
	}

	return NOERROR;
}

bool ImageComaparebyPix(BYTE* RefImage,BYTE* TestImage, int width, int height)
{
	bool res=false;

	int line,col;
	int diffPix=0;
	int diffPos[1024][2];

	for(line=32;line<height-32;line++)
	{
		for(col=64;col<width-64;col++)
		{
			if( abs(RefImage[(line-1)*width+(col-1)]-TestImage[line*width+col])>20&&
				abs(RefImage[(line-1)*width+col]-TestImage[line*width+col])>20&&
				abs(RefImage[(line-1)*width+(col+1)]-TestImage[line*width+col])>20&&
				abs(RefImage[line*width+(col-1)]-TestImage[line*width+col])>20&&
				abs(RefImage[line*width+col]-TestImage[line*width+col])>20&&
				abs(RefImage[line*width+(col+1)]-TestImage[line*width+col])>20&&
				abs(RefImage[(line+1)*width+(col-1)]-TestImage[line*width+col])>20&&
				abs(RefImage[(line+1)*width+col]-TestImage[line*width+col])>20&&
				abs(RefImage[(line+1)*width+(col+1)]-TestImage[line*width+col])>20)
				diffPix++;
		}
	}

	if(diffPix>100)
		res=false;
	else
		res=true;
	
	return res;
}

bool ImageCompareByGrayscale(BYTE* RefImage,BYTE* TestImage, int width, int height)
{

	CString strRef,sreTest;
	strRef.Format("Ref_%d",CheckPointNum);
	strTest.Format("Test_%d",CheckPointNum);
	CheckPointNum++;

	CFile fileRef(strRef,CFile::modeRead);
	CFile fileTest(strTest,CFile::modeRead);

	for(int i=0;i<CHECKFRAMES;i++)
	{
		fileRef.Read(RefImage[i],622080);
		fileTest.Read(TestImage[i],622080);
	}

	fileRef.Close();
	fileTest.Close();
	double tempCorr=0;
	for(int r=0;r<CHECKFRAMES;r++)
	{
		for(int t=0;t<CHECKFRAMES;t++)
		{
			tempCorr=GetImageCorr(RefImage[r],TestImage[t],720,576);
			
			if(maxCorr<tempCorr)
					maxCorr=tempCorr;
		}
	}

	if(maxCorr>0.8)
		return true;
	else
		return false;
}

double GetImageCorr(BYTE* RefImage,BYTE* TestImage,int width, int height)
{
	double corr=0;

	int line,col;

	int scaleRef[256];
	int scaleTest[256];
	int diffPos[256][2];
	
	int t=sizeof(scaleRef);
	int tt=sizeof(diffPos);

	memset(scaleRef,0,sizeof(scaleRef));
	memset(scaleTest,0,sizeof(scaleTest));
	
	for(line=32;line<height-32;line++)
	{
		for(col=64;col<width-64;col++)
		{
			scaleRef[RefImage[line*width+col]]++;
			scaleTest[TestImage[line*width+col]]++;
		}
	}

	corr=GetCorr(scaleRef,scaleTest,256);
	return corr;
}

bool GetAudioCorr(BYTE* RefAudio, BYTE* TestAudio, int length)
{

}

double  GetCorr(BYTE* Ref,BYTE* Test,int length)
{
		
	double Sx=0,Sy=0,Sxy=0,Sxx=0,Syy=0;
	int N=256;
	int i;
	for(i=0;i<N;i++)
	{
		Sxy += Ref[i]*Test[i];
		Sx  += Ref[i];
		Sy  += Test[i];
		Sxx += Ref[i]*Ref[i];
		Syy += Test[i]*Test[i];
	}

	corr = (Sxy-(Sx*Sy/N))/sqrt((Sxx-Sx*Sx/N)*(Syy-Sy*Sy/N));
	return corr;

}

