
#include "Stdafx.h"
#include "VidCompare.h"


BYTE VidReferenceBuf[829440];
BYTE VidTestBuf[829440];
BYTE VidOutputBuf[829440];
BOOL bVidGetReferenceBuf=FALSE;
BOOL bVidGetTestBuf=FALSE;
FILE* Vidfp=fopen("D:\\test1\\out.yuv","w");
FILE* VidfpRef=fopen("D:\\test1\\ref.yuv","w");
FILE* VidfpTest=fopen("D:\\test1\\test.yuv","w");
HANDLE VidhFile;
CFile* myVidFileOut;
CFile* myVidFileRef;
CFile* myVidFileTest;

BYTE AudReferenceBuf[81920];
BYTE AudTestBuf[81920];
BYTE AudOutputBuf[81920];
BOOL bAudGetReferenceBuf=FALSE;
BOOL bAudGetTestBuf=FALSE;
FILE* Audfp=fopen("D:\\test1\\out.pcm","w");
FILE* AudfpRef=fopen("D:\\test1\\ref.pcm","w");
FILE* AudfpTest=fopen("D:\\test1\\test.pcm","w");
HANDLE AudhFile;
CFile* myAudFileOut;
CFile* myAudFileRef;
CFile* myAudFileTest;


static int iiVid=0;
static int iiAud=0;

int  VidCompare(BYTE* ReferenceBuf, BYTE* TestBuf)
{
if(iiVid==0)
{
 VidhFile= CreateFile(L"D:\\test1\\out.yuv",GENERIC_WRITE, FILE_SHARE_WRITE,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 VidhFile= CreateFile(L"D:\\test1\\out.yuv",GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

 myVidFileOut= new CFile(VidhFile);

 VidhFile= CreateFile(L"D:\\test1\\ref.yuv",GENERIC_WRITE, FILE_SHARE_WRITE,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 VidhFile= CreateFile(L"D:\\test1\\ref.yuv",GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 myVidFileRef = new CFile(VidhFile);

 VidhFile= CreateFile(L"D:\\test1\\test.yuv",GENERIC_WRITE, FILE_SHARE_WRITE,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 VidhFile= CreateFile(L"D:\\test1\\test.yuv",GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 myVidFileTest = new CFile(VidhFile);
 iiVid++;
}
	int i,j;
	int xOffset=0;
	int yOffset=0;
	BYTE diff;
	for(i=10;i<854;i++)
		for(j=20;j<700;j++)
		{
			diff=abs(VidReferenceBuf[(i+xOffset)*720+j+yOffset]-VidTestBuf[i*720+j]);
			VidOutputBuf[i*720+j]=diff>10?diff+128:128;
		}
	myVidFileOut->Write(VidOutputBuf,622080);	
	myVidFileRef->Write(VidReferenceBuf,622080);		
	myVidFileTest->Write(VidTestBuf,622080);
	
	return 0;
}



extern "C" _declspec(dllexport)  int VidCompareBuf(BYTE* Buf, int Param)
{
	if(Param==0)
	{
		memcpy(VidReferenceBuf,Buf,622080);
		bVidGetReferenceBuf=TRUE;		
	}

	else
	{
		memcpy(VidTestBuf,Buf,622080);
		bVidGetTestBuf=TRUE;
	}

	if(bVidGetReferenceBuf&&bVidGetTestBuf)
	{
		VidCompare(VidReferenceBuf,VidTestBuf);
		bVidGetReferenceBuf=FALSE;
		bVidGetTestBuf=FALSE;
	}

	return 0;

}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


int  AudCompare(BYTE* ReferenceBuf, BYTE* TestBuf, int Lenth, int Param)
{
if(iiAud==0)
{
 AudhFile= CreateFile(L"D:\\test1\\out.pcm",GENERIC_WRITE, FILE_SHARE_WRITE,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 AudhFile= CreateFile(L"D:\\test1\\out.pcm",GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

 myAudFileOut= new CFile(AudhFile);

 AudhFile= CreateFile(L"D:\\test1\\ref.pcm",GENERIC_WRITE, FILE_SHARE_WRITE,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 AudhFile= CreateFile(L"D:\\test1\\ref.pcm",GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 myAudFileRef = new CFile(AudhFile);

 AudhFile= CreateFile(L"D:\\test1\\test.pcm",GENERIC_WRITE, FILE_SHARE_WRITE,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 AudhFile= CreateFile(L"D:\\test1\\test.pcm",GENERIC_WRITE, FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 myAudFileTest = new CFile(AudhFile);
 iiAud++;
}
	int i,j;
	int xOffset=0;
	int yOffset=0;
	BYTE diff;

//	myAudFileOut->Write(AudOutputBuf,Lenth);	
	if(Param==0)
	{
		memcpy(AudReferenceBuf,ReferenceBuf,Lenth);
		myAudFileRef->Write(AudReferenceBuf,Lenth);
	}
	else
	{
		memcpy(AudTestBuf,TestBuf,Lenth);
		myAudFileTest->Write(AudTestBuf,Lenth);
	}
	
	return 0;
}

extern "C" _declspec(dllexport)  int AudCompareBuf(BYTE* Buf, int Lenth, int Param)
{

	if(Param==0)
	{
//		memcpy(AudReferenceBuf,Buf,Lenth);
		bAudGetReferenceBuf=TRUE;	
		AudCompare(Buf,0,Lenth,Param);
	}

	else
	{
//		memcpy(AudTestBuf,Buf,Lenth);
		bAudGetTestBuf=TRUE;
		AudCompare(0,Buf,Lenth,Param);
	}

	//if(bAudGetReferenceBuf&&bAudGetTestBuf)
	//{
	//	AudCompare(AudReferenceBuf,AudTestBuf,Lenth);
	//	bAudGetReferenceBuf=FALSE;
	//	bAudGetTestBuf=FALSE;
	//}

	return 0;

}

extern "C" _declspec(dllexport)  int Dlltest(int a, int b)
{
	return a+b;
}