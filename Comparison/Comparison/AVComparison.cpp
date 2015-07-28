
#include "stdafx.h"
#include "AVComparison.h"
#include "math.h"

extern int CheckPointNum;
extern CString gTempPath;

bool SingleImageComparebyPix(BYTE* RefImage,BYTE* TestImage, int xPos, int yPos, int width, int height, int xOffset, int yOffset, int Threshold);
int ImageComparebyPix(int i);
int ImageCompareByGrayscale(int i);
int AudioComparison(int i);


int ImageComparison(char CompareList[])
{
	int ret=RESULT_PASS;

	for(int i=0;i<CheckPointNum;i++)
	{
		if(CompareList[i]=='U')
		{
			ret=ImageComparebyPix(i);			
		}
		if(CompareList[i]=='V')
		{
			ret=ImageCompareByGrayscale(i);
		//	ret|=AudioComparison(i);
		}
		if(ret!=RESULT_PASS)
			return ret;
	}
	return RESULT_PASS;
}

int ImageComparebyPix(int i)
{
	CString strRef="";
	CString strTest="";
	int bufWidth=768;
	int bufHeight=576;
	int bufPix=bufWidth*bufHeight*2;

	int imgWidth=720;
	int imgHeight=576;
	int imgPix=imgWidth*imgHeight*2;

	BYTE* RefImageBuf=new BYTE[bufPix];
	BYTE* TestImageBuf=new BYTE[bufPix];

	BYTE* RefImage=new BYTE[imgPix];
	BYTE* TestImage=new BYTE[imgPix];

	strRef.Format("%s\\Ref_%d.yuv",gTempPath,i);
	strTest.Format("%s\\Test_%d.yuv",gTempPath,i);


	FILE *fp_ref,*fp_test;
	fp_ref=fopen(strRef,"rb");
	fp_test=fopen(strTest,"rb");

	if(fp_ref==NULL)
		return RESULT_CAPTRUE_REFVIDEO_NOT_FOUND;
	if(fp_test==NULL)
		return RESULT_CAPTRUE_TESTVIDEO_NOT_FOUND;

	while(fread(RefImageBuf,bufPix,1,fp_ref))
	{
		fseek(fp_test,bufPix,SEEK_SET);

		while(fread(TestImageBuf,bufPix,1,fp_test))
		{		
			bool singleRes_Y=false;
			bool singleRes_U=false;
			bool singleRes_V=false;

			int h,w;
			for(h=0;h<imgHeight;h++)
			{
				for(w=0;w<imgWidth;w++)
				{
					RefImage[h*imgWidth+w]=RefImageBuf[h*1536+w*2];					
					RefImage[h*(imgWidth/2)+(w/2)+imgWidth*imgHeight]=RefImageBuf[h*bufWidth*2+(w/2)*4+1];
					RefImage[h*(imgWidth/2)+(w/2)+imgWidth*imgHeight*3/2]=RefImageBuf[h*bufWidth*2+(w/2)*4+3];

					TestImage[h*imgWidth+w]=TestImageBuf[h*1536+w*2];					
					TestImage[h*(imgWidth/2)+(w/2)+imgWidth*imgHeight]=TestImageBuf[h*bufWidth*2+(w/2)*4+1];
					TestImage[h*(imgWidth/2)+(w/2)+imgWidth*imgHeight*3/2]=TestImageBuf[h*bufWidth*2+(w/2)*4+3];
				}
			}					

			for(int ii=-3;ii<=3;ii++)
			{
				if(singleRes_Y==true)
						break;
				for(int jj=-3;jj<=3;jj++)
				{
					
					singleRes_Y=SingleImageComparebyPix(RefImage,            TestImage,            64,32,imgWidth-64,   imgHeight-32,ii,jj,220);
					if(singleRes_Y==true)
						break;
				}
			}
			for(int ii=-3;ii<=3;ii++)
			{
				if(singleRes_U==true)
						break;
				for(int jj=-3;jj<=3;jj++)
				{
					singleRes_U=SingleImageComparebyPix(RefImage+imgPix/2,   TestImage+imgPix/2,   32,32,imgWidth/2-32, imgHeight-32,ii,jj,110);
					if(singleRes_U==true)
						break;
				}
			}
			for(int ii=-3;ii<=3;ii++)
			{
				if(singleRes_V==true)
					break;
				for(int jj=-3;jj<=3;jj++)
				{
					singleRes_V=SingleImageComparebyPix(RefImage+imgPix/4*3, TestImage+imgPix/4*3, 32,32,imgWidth/2-32, imgHeight-32,ii,jj,110);
					if(singleRes_V==true)
						break;
				}
			}

			if(singleRes_Y==true&&singleRes_U==true&&singleRes_V==true)
			{
				fclose(fp_ref);
				fclose(fp_test);
				delete []RefImage;
				delete []TestImage;
				delete []RefImageBuf;
				delete []TestImageBuf;

				return RESULT_PASS;
			}
		}
	}

	//}
	//}

	fclose(fp_ref);
	fclose(fp_test);

	delete []RefImage;
	delete []TestImage;
	delete []RefImageBuf;
	delete []TestImageBuf;
	return RESULT_FAIL;
}

bool SingleImageComparebyPix(BYTE* RefImage,BYTE* TestImage, int xPos, int yPos, int width, int height, int xOffset, int yOffset, int Threshold)
{ 

	bool res=false;

	int line,col;
	int diffPix=0;

	for(line=yPos;line<height;line++)
	{
		for(col=xPos;col<width;col++)
		{
			if( /*abs(RefImage[((line+xOffset)-2)*width+((col+yOffset)-2)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)-2)*width+((col+yOffset)-1)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)-2)*width+((col+yOffset))]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)-2)*width+((col+yOffset)+1)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)-2)*width+((col+yOffset)+2)]-TestImage[line*width+col])>20&&*/

				/*abs(RefImage[((line+xOffset)-1)*width+((col+yOffset)-2)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)-1)*width+((col+yOffset)-1)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)-1)*width+((col+yOffset))]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)-1)*width+((col+yOffset)+1)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)-1)*width+((col+yOffset)+2)]-TestImage[line*width+col])>20&&*/

				/*abs(RefImage[((line+xOffset))*width+((col+yOffset)-2)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset))*width+((col+yOffset)-1)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset))*width+((col+yOffset))]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset))*width+((col+yOffset)+1)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset))*width+((col+yOffset)+2)]-TestImage[line*width+col])>20&&*/

				/*abs(RefImage[((line+xOffset)+1)*width+((col+yOffset)-2)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)+1)*width+((col+yOffset)-1)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)+1)*width+((col+yOffset))]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)+1)*width+((col+yOffset)+1)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)+1)*width+((col+yOffset)+2)]-TestImage[line*width+col])>20&&*/

				/*abs(RefImage[((line+xOffset)+2)*width+((col+yOffset)-2)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)+2)*width+((col+yOffset)-1)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)+2)*width+((col+yOffset))]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)+2)*width+((col+yOffset)+1)]-TestImage[line*width+col])>20&&
				abs(RefImage[((line+xOffset)+2)*width+((col+yOffset)+2)]-TestImage[line*width+col])>20*/
				abs(RefImage[((line+yOffset)-1)*width+((col+xOffset)-1)]-TestImage[line*width+col])>32&&
				abs(RefImage[((line+yOffset)-1)*width+((col+xOffset))]-TestImage[line*width+col])>32&&
				abs(RefImage[((line+yOffset)-1)*width+((col+xOffset)+1)]-TestImage[line*width+col])>32&&
				abs(RefImage[((line+yOffset))*width+((col+xOffset)-1)]-TestImage[line*width+col])>32&&
				abs(RefImage[((line+yOffset))*width+((col+xOffset))]-TestImage[line*width+col])>32&&
				abs(RefImage[((line+yOffset))*width+((col+xOffset)+1)]-TestImage[line*width+col])>32&&
				abs(RefImage[((line+yOffset)+1)*width+((col+xOffset)-1)]-TestImage[line*width+col])>32&&
				abs(RefImage[((line+yOffset)+1)*width+((col+xOffset))]-TestImage[line*width+col])>32&&
				abs(RefImage[((line+yOffset)+1)*width+((col+xOffset)+1)]-TestImage[line*width+col])>32
				)
				diffPix++;
		}
	}



	CString tr;
	tr.Format("%d: diff:%d, %d ,%d ,%d\n",CheckPointNum,diffPix,width,xOffset,yOffset);
	if(diffPix>Threshold)
		res=false;
	else
		res=true;

	if(diffPix<200)
		TRACE(tr);
	return res;
}

int ImageCompareByGrayscale(int i)
{

	CString strRef="";
	CString strTest="";
	
	int bufWidth=768;
	int bufHeight=576;
	int bufPix=bufWidth*bufHeight*2;

	int imgWidth=720;
	int imgHeight=576;
	int imgPix=imgWidth*imgHeight*2;

	BYTE* RefImageBuf=new BYTE[bufPix];
	BYTE* TestImageBuf=new BYTE[bufPix];

	BYTE* RefImage=new BYTE[imgPix];
	BYTE* TestImage=new BYTE[imgPix];

	strRef.Format("%s\\Ref_%d.yuv",gTempPath,i);
	strTest.Format("%s\\Test_%d.yuv",gTempPath,i);

	FILE *fp_ref,*fp_test;
	fp_ref=fopen(strRef,"rb");
	fp_test=fopen(strTest,"rb");

	if(fp_ref==NULL)
		return RESULT_CAPTRUE_REFVIDEO_NOT_FOUND;
	if(fp_test==NULL)
		return RESULT_CAPTRUE_TESTVIDEO_NOT_FOUND;

	int n=fread(RefImageBuf,bufPix,1,fp_ref);
	while(fread(RefImageBuf,bufPix,1,fp_ref))
	{
		fseek(fp_test,bufPix,SEEK_SET);
		while(fread(TestImageBuf,bufPix,1,fp_test))
		{	
			double Corr_Y=0.0;
			double Corr_U=0.0;
			double Corr_V=0.0;

			double maxCorr_Y=0.0;
			double maxCorr_U=0.0;
			double maxCorr_V=0.0;

			int h,w;
			for(h=0;h<imgHeight;h++)
			{
				for(w=0;w<imgWidth;w++)
				{
					RefImage[h*imgWidth+w]=RefImageBuf[h*1536+w*2];					
					RefImage[h*(imgWidth/2)+(w/2)+imgWidth*imgHeight]=RefImageBuf[h*bufWidth*2+(w/2)*4+1];
					RefImage[h*(imgWidth/2)+(w/2)+imgWidth*imgHeight*3/2]=RefImageBuf[h*bufWidth*2+(w/2)*4+3];

					TestImage[h*imgWidth+w]=TestImageBuf[h*1536+w*2];					
					TestImage[h*(imgWidth/2)+(w/2)+imgWidth*imgHeight]=TestImageBuf[h*bufWidth*2+(w/2)*4+1];
					TestImage[h*(imgWidth/2)+(w/2)+imgWidth*imgHeight*3/2]=TestImageBuf[h*bufWidth*2+(w/2)*4+3];
				}
			}

			Corr_Y=GetImageCorr(RefImage,TestImage,imgWidth,imgHeight);
			Corr_U=GetImageCorr(RefImage+imgPix/2,TestImage+imgPix/2,imgWidth/2,imgHeight);
			Corr_V=GetImageCorr(RefImage+imgPix/4*3,TestImage+imgPix/4*3,imgWidth/2,imgHeight);

			if(maxCorr_Y<Corr_Y)
				maxCorr_Y=Corr_Y;
			if(maxCorr_U<Corr_U)
				maxCorr_U=Corr_U;
			if(maxCorr_V<Corr_V)
				maxCorr_V=Corr_V;
		
			if(maxCorr_Y>0.8||maxCorr_U>0.8||maxCorr_V>0.8)	
			{
				fclose(fp_ref);
				fclose(fp_test);
				delete []RefImage;
				delete []TestImage;
				delete []RefImageBuf;
				delete []TestImageBuf;

				return RESULT_PASS;
			}
		}
	}
	
	fclose(fp_ref);
	fclose(fp_test);

	delete []RefImage;
	delete []TestImage;
	delete []RefImageBuf;
	delete []TestImageBuf;

	return RESULT_FAIL;

}

double GetImageCorr(BYTE* RefImage,BYTE* TestImage,int width, int height)
{
	double corr=0;

	int line,col;

	int scaleRef[256];
	int scaleTest[256];

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
	return true;
}

double  GetCorr(int* Ref,int* Test,int length)
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

	return (Sxy-(Sx*Sy/N))/sqrt((Sxx-Sx*Sx/N)*(Syy-Sy*Sy/N));


}
/////////////////////////////////////////////
//AUDIO//////////////////////////////////////
/////////////////////////////////////////////

int AudioComparison(int i)
{
	int len=2048;
	int RefPos=0,TestPos=0;
	CString strRef="";
	CString strTest="";
	double maxCorr_l=0.0;
	double maxCorr_r=0.0;

	signed short* RefPCMBuf=new signed short[len*2];
	signed short* TestPCMBuf=new signed short[len*2];

	int* RefPCM_Left=new int[len];
	int* TestPCM_Left=new int[len];
	int* RefPCM_Right=new int[len];
	int* TestPCM_Right=new int[len];

	strRef.Format("%s\\Aud_%d_0.pcm",gTempPath,i);
	strTest.Format("%s\\Aud_%d_1.pcm",gTempPath,i);

	FILE *fp_ref,*fp_test;
	fp_ref=fopen(strRef,"rb");
	fp_test=fopen(strTest,"rb");

	if(fp_ref==NULL)
		return RESULT_CAPTRUE_REFAUDIO_NOT_FOUND;
	if(fp_test==NULL)
		return RESULT_CAPTRUE_TESTAUDIO_NOT_FOUND;

	FILE *fp=fopen("E:\\test.xls","wb");
	CString str;

	while(fread(RefPCMBuf,len*4,1,fp_ref))
	{
		fseek(fp_test,++++++++TestPos,SEEK_SET);
		while(fread(TestPCMBuf,len*4,1,fp_test))
		{
			for(int i=0;i<len;i++)
			{
				RefPCM_Left[i]=RefPCMBuf[i*4+1];//(RefPCMBuf[i*4+1]&128)?(~(RefPCMBuf[i*4+1]&127)+1)*(-1):RefPCMBuf[i*4+1];
				RefPCM_Right[i]=RefPCMBuf[i*4+3];//(RefPCMBuf[i*4+3]&128)?(~(RefPCMBuf[i*4+3]&127)+3)*(-1):RefPCMBuf[i*4+3];
				TestPCM_Left[i]=TestPCMBuf[i*4+1];//(TestPCMBuf[i*4+1]&128)?(~(TestPCMBuf[i*4+1]&127)+1)*(-1):TestPCMBuf[i*4+1];
				TestPCM_Right[i]=TestPCMBuf[i*4+3];//(TestPCMBuf[i*4+3]&128)?(~(TestPCMBuf[i*4+3]&127)+3)*(-1):TestPCMBuf[i*4+3];
				str.Format("%d\t%d\t%d\t%d\n",RefPCM_Left[i],RefPCM_Right[i],TestPCM_Left[i],TestPCM_Right[i]);
				fwrite(str,str.GetLength(),1,fp);
			}
			fclose(fp);
			double temp_l=GetCorr(RefPCM_Left,TestPCM_Left,len);
			double temp_r=GetCorr(RefPCM_Right,TestPCM_Right,len);
			if(temp_l>maxCorr_l)
				maxCorr_l=temp_l;
			if(temp_r>maxCorr_r)
				maxCorr_r=temp_r;
		}
	}


	delete []RefPCMBuf;
	delete []TestPCMBuf;
	delete []RefPCM_Left;
	delete []RefPCM_Right;
	delete []TestPCM_Left;
	delete []TestPCM_Right;

	if(maxCorr_l>0.8&&maxCorr_r>0.8)
		return RESULT_PASS;
	else
		return RESULT_FAIL;
}