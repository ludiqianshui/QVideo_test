
#ifndef __SAMPLEGRABBER_H_H_
#define __SAMPLEGRABBER_H_H_
#include "streams.h"
#include "qedit.h"


bool StartCheck=false;
bool RefImageReady=false;
bool TestImageReady=false;
int CheckPointNum=0;
#define  CHECKFRAMES 25;


class  CVidSampleGrabberRef : public CTransInPlaceFilter
{
public:
	CVidSampleGrabberRef(LPUNKNOWN punk, 	HRESULT *phr, LONG inParam);
	~CVidSampleGrabberRef();
	CCritSec    mSafeCapture;

public:
	virtual HRESULT CheckInputType(const CMediaType* mtIn); // PURE
	virtual HRESULT Transform(IMediaSample *pSample); // PURE
	FILE* fp;
	LONG Param;
	LONG Counting;
    HANDLE hFile;
	CFile *myFile;				
};

class  CVidSampleGrabberTest : public CTransInPlaceFilter
{
public:
	CVidSampleGrabberTest(LPUNKNOWN punk, 	HRESULT *phr, LONG inParam);
	~CVidSampleGrabberTest();
	CCritSec    mSafeCapture;

public:
	virtual HRESULT CheckInputType(const CMediaType* mtIn); // PURE
	virtual HRESULT Transform(IMediaSample *pSample); // PURE
	FILE* fp;
	LONG Param;
	LONG Counting;
    HANDLE hFile;
	CFile *myFile;				
};



class  CAudSampleGrabber : public CTransInPlaceFilter
{
public:
	CAudSampleGrabber(LPUNKNOWN punk, 	HRESULT *phr, LONG inParam);
	~CAudSampleGrabber();
	CCritSec    mSafeCapture;

public:
	virtual HRESULT CheckInputType(const CMediaType* mtIn); // PURE
	virtual HRESULT Transform(IMediaSample *pSample); // PURE
	FILE* fp;
	LONG Param;
	LONG Counting;
    HANDLE hFile;
	CFile *myFile;				
};
#endif // __H_CFilterImageGrabber__