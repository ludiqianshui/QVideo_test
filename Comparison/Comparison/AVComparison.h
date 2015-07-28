


#ifndef __H_AVCOMPARISON__
#define __H_AVCOMPARISON__
#define RESULT_OK 0
#define RESULT_PASS 0
#define RESULT_FAIL 1
#define RESULT_SCRIPT_FORMAT_ERROR 2
#define RESULT_SCRIPT_NOT_FOUND 4
#define RESULT_CAPTRUE_REFVIDEO_NOT_FOUND 8
#define RESULT_CAPTRUE_TESTVIDEO_NOT_FOUND 16
#define RESULT_CAPTRUE_REFAUDIO_NOT_FOUND 32
#define RESULT_CAPTRUE_TESTAUDIO_NOT_FOUND 64
#define RESULT_DEPENDED_CASE_IS_NOT_PASS 128
#define RESULT_INTERRUPT_BY_USER 256

double  GetCorr(int* Ref,int* Test,int length);
double GetImageCorr(BYTE* RefImage,BYTE* TestImage,int width, int height);
int ImageCompareByGrayscale(void);
int ImageComaparebyPix(BYTE* RefImage,BYTE* TestImage, int width, int height);
int AudioComparison(int i);
int ImageComparison(char CompareList[]);

#endif