
#ifndef __SAMPLEGRABBER_H_H_
#define __SAMPLEGRABBER_H_H_
#include "streams.h"
#include "qedit.h"



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
	bool AudCapReady;

public:
	virtual HRESULT CheckInputType(const CMediaType* mtIn); // PURE
	virtual HRESULT Transform(IMediaSample *pSample); // PURE
	FILE* fp;
	LONG Param;
	LONG Counting;
    HANDLE hFile;
	CFile *myFile;				
};

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

class CVidSampleGrabberRefCB : public ISampleGrabberCB 
{
	
public:
	int mGraphID;
	BYTE* pBuffer;
	FILE* fp;
	HANDLE hFile;
	CFile *myFile;
	int Counting;
	CCritSec    mSafeCapture;
	
    STDMETHODIMP_(ULONG) AddRef()  { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }
	
    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);	
    STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample );
    STDMETHODIMP BufferCB( double SampleTime, BYTE * pBuffer, long BufferLen );
	
};


class CVidSampleGrabberTestCB : public ISampleGrabberCB 
{
	
public:
	int mGraphID;
	BYTE* pBuffer;
	FILE* fp;
	HANDLE hFile;
	CFile *myFile;
	int Counting;
	CCritSec    mSafeCapture;
	
    STDMETHODIMP_(ULONG) AddRef()  { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }
	
    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);	
    STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample );
    STDMETHODIMP BufferCB( double SampleTime, BYTE * pBuffer, long BufferLen );
	
};










#endif // __H_CFilterImageGrabber__