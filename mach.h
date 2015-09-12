/*
  Detects SIFT features in two images and finds matches between them.

  Copyright (C) 2006-2012  Rob Hess <rob@iqengines.com>

  @version 1.1.2-20100521
*/
#include "mysql_main.h"
#include <windows.h>  
#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"
#include "xform.h"
#include "tools.h"
#include "pca.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <math.h>

#include <stdio.h>
#include<string.h>

#include<string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>  
#include <process.h>  
#include <windows.h>  
#include "myPcaTools.h"


/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.35//0.49



struct tempStructForThread_LogoCaculate
{//用于传参数
	int numberOfLogo;
	srcIndexStruct *logo_srcIndex;
	LogoFeatStruct *** logofeat;
	int NUMBEROF_CENG;
	int threadNumber;
	CvSize **logoSizeIndex;
};
struct tempStructForThread_EachPicture{


		srcIndexStruct *Picture_srcIndex;
			srcIndexStruct *logo_srcIndex;
			int numberOfPicture;
			int numberOfLogo;
			LogoFeatStruct ** logofeat;
			//CvMat  ***LogoMatrix,
			CvMat *AvgVector;
			CvMat *EigenVector;
			CvMat *BigIndex;
			CvSeq *featseq_ForBigClass;
			CvSize **logoSizeIndex;
			int threadNumber;
};




void myreleaseFeatStruct(LogoFeatStruct *** logofeat,
	int numberOfLogo,int NUMBEROF_CENG );

void init_tempForThread_LogoCaculate(
struct tempStructForThread_LogoCaculate *tempstruct,
	srcIndexStruct *logo_srcIndex,LogoFeatStruct *** logofeat
	,int threadNumber,int NUMBEROF_CENG,int numberOfLogo,
	CvSize **logoSizeIndex);

void  CaculateLogoFeatFirst(
	srcIndexStruct *logo_srcIndex,
	LogoFeatStruct *** logofeat,
	int numberOfLogo,int NUMBEROF_CENG,
	CvSize ***logoSizeIndex);

unsigned int __stdcall MyThreadFun_LogoCaculate(PVOID pM)  ;

void init_tempStructForThread_EachPicture(
struct tempStructForThread_EachPicture *pM,
	srcIndexStruct *Picture_srcIndex,
	srcIndexStruct *logo_srcIndex,
	int numberOfPicture,
	int numberOfLogo,
	LogoFeatStruct ** logofeat,
	//CvMat  ***LogoMatrix,
	CvMat *AvgVector,
	CvMat *EigenVector,
	CvMat *BigIndex,
	CvSeq *featseq_ForBigClass,
	CvSize **logoSizeIndex,
	int threadNumber);

 

unsigned int __stdcall MyThreadFun_EachPicture(PVOID pM)  ;


int  myMachALLL(CvMat *resultPCAMatrix,CvSeq *featseq_ForBigClass,
	LogoFeatStruct ** logofeat , int numberOfLogo,
	struct feature  * feat2,CvMat *PictureMatrix,
	IplImage *img2,
	srcIndexStruct *logo_srcIndex,CvSize **logoSizeIndex,
	CvRect *resultRect,
	int *numberOfMachedLogo/*用于结果处理,匹配到了哪个logo*/
	);


int judgeFormPiont(struct feature* feat1,struct feature * feat2,CvSeq * featseq,int n1,int n2,int n3,
	IplImage*img1,double liangduImg1,IplImage * img2,IplImage*stacked ,
	CvPoint pointWindow1,CvPoint  pointWindow2);

int myLastMached_seqFeat_Fea2_Feat1( srcIndexStruct *logo_srcIndex,
	int lei,int ceng,//到时候可以不使用
	CvSize logoSize,IplImage* img2, 
	struct feature*feat1,struct feature* feat2, 
	CvSeq *featseq, 
	int n1,int n2,int n3
	,CvRect *resultRect);

int judgMachedFromImgaToImage(IplImage*img1,IplImage*img2,IplImage*stacked,
struct feature* feat1,struct feature * feat2,CvSeq *featseq,
	int n1,int n2,int n3,
	CvRect *resultRect);


int compareFeartureFirstTime( struct feature* feat1,CvMat * tempMatrixForLogo,
struct feature* feat2,CvMat *PictureMatrix,
int n1,int n2,
IplImage* img1,IplImage* stacked,	CvSeq *featseq) ;

int compareFearture( struct feature* feat1, struct feature* feat2, 
	int n1,int n2,IplImage* img1,IplImage* stacked,CvPoint pointWindow1,CvPoint pointWindow2);

int simOfFearture(int n1,int n2,int m,double huiduImag1,double  huiduImag2 );





