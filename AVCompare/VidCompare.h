
#include "Stdafx.h"
//#include "cv.h"
int  VidCompare(BYTE* ReferenceBuf, BYTE* TestBuf);
int  AudCompare(BYTE* ReferenceBuf, BYTE* TestBuf, int Lenth, int Param);

extern "C" _declspec(dllexport)  int VidCompareBuf(BYTE* Buf, int Param);
extern "C" _declspec(dllexport)  int AudCompareBuf(BYTE* Buf, int Lenth, int Param);