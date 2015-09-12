#ifndef MYPICTURE_TOOLS_H
#define MYPICTURE_TOOLS_H
#include "myBaseTools.h"


typedef struct logoFeatStruct{
	struct feature * feat1;
	int numberOffeat;
}LogoFeatStruct;




typedef struct machedFeatPoint{
	CvPoint machedPointFeat1;
	CvPoint machedPointFeat2;
}MachedFeatPoint;



void myDrawRectToImage(IplImage *img,CvRect rect);



void GetMachedWindowPoint(MachedFeatPoint* featTemp,
	CvPoint * p1,CvPoint * p2,CvSize img1size,CvSize img1size2 );

int myDivedeImg(IplImage** *imgForPicture,IplImage* img2);




void drawTagForWindow(CvMat * tagMat,int x,int y,CvSize size);

void myDrawPoint(CvSeq *featseq,int numberOfMached,IplImage* stacked,int height);

void reduceMachedPoints(CvSeq *featseq,int * numberOfMached );

int compareIsEqualFeat2(MachedFeatPoint * machedstruct,
	MachedFeatPoint * machedstructTemp);

int compareIsEqual(MachedFeatPoint * machedstruct,
	MachedFeatPoint * machedstructTemp);


int compareIsInWindow(MachedFeatPoint * machedstruct,
	CvPoint pointWindow1, CvPoint pointWindow2);

void printMyImag(IplImage* img);

IplImage* caculateSmallSizeImg( IplImage* img ,
	double nForSize,int ceng);

IplImage* smallSizeImg( IplImage* img ,double nForSize);

int Img1BigImg2(IplImage*img1,IplImage*img2);
int isTooSmall(IplImage*img1);



int  find_Drawcontours (IplImage *src,IplImage *dst);





#endif