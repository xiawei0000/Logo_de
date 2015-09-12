#ifndef  _MYPCATOOLS_H
#define  _MYPCATOOLS_H


#include "tools.h"
#include "pca.h"
void buildMycvSeqForeachLogoCeng(CvSeq ****SeqForeachLogoCeng,CvMemStorage*** *memstorage,
	int numberOfLogo);

void reduceMachedPointscvSeqForeachLogoCeng(CvSeq ***SeqForeachLogoCeng,	int numberOfLogo);


int getOneMax(CvMat *resultPCAMatrix,int i,double *max1);


struct LogoTagAndFeatTag*  myMachSmallClass(CvSeq *featseq_ForBigClass,int numberOfSmallClass,
	CvMat *PictureMatrix,int numberOfPictureFeat);

MachedFeatPoint *  featToMachedFeatPoint(struct feature* tempFeat1,struct feature* tempFeat2);

void releaseMycvSeqForeachLogoCeng(CvSeq ****SeqForeachLogoCeng,CvMemStorage*** *memstorage,
	int numberOfLogo);

int getTwoMax(CvMat *resultPCAMatrix,int i,double *max1,double *max2);

#endif