#include "myPcaTools.h"

extern const int NUMBERFORDIVISIOM;
extern const int NUMBEROF_CENG;//logoҪ����-1 ��
extern double YUZHI_COS_MIN;
extern double YUZHI_COS_MAX;
extern double YUZHI_COS;
extern const double MINCOS_PCA;

void buildMycvSeqForeachLogoCeng(CvSeq ****SeqForeachLogoCeng,CvMemStorage*** *memstorage,
	int numberOfLogo)
{	//
	int i,j,k;

	//��ʼ��logo ��seq
	(*SeqForeachLogoCeng)= (CvSeq ***)malloc(sizeof(CvSeq **)*numberOfLogo);//ÿ��logo

	//��ʼ��mem
	(*memstorage)= (CvMemStorage***)malloc(sizeof(CvMemStorage**)*numberOfLogo);


	for (i=0; i<numberOfLogo ;i++ )
	{
		(*SeqForeachLogoCeng)[i]=(CvSeq **)malloc( sizeof(CvSeq *) * NUMBEROF_CENG );//ÿһ��
		(*memstorage)[i]=(CvMemStorage **)malloc( sizeof(CvMemStorage *) * NUMBEROF_CENG );

		for (j=0;j<NUMBEROF_CENG;j++)
		{
			(*memstorage)[i][j]=cvCreateMemStorage(0);//��ʼ���ڴ�

			(*SeqForeachLogoCeng)[i][j] =
				cvCreateSeq(0,sizeof(CvSeq),sizeof(MachedFeatPoint),(*memstorage)[i][j]);
		}	
	}
}

void reduceMachedPointscvSeqForeachLogoCeng(CvSeq ***SeqForeachLogoCeng,	int numberOfLogo)
{	//��ÿһ��seq ���� 1�Զ��ƥ��
	int i,j,k;


	for (i=0; i<numberOfLogo ;i++ )
	{
		for (j=0;j<NUMBEROF_CENG;j++)
		{	
			k= (SeqForeachLogoCeng)[i][j]->total;
			reduceMachedPoints((SeqForeachLogoCeng)[i][j],&k);
		}	
	}
}



int getOneMax(CvMat *resultPCAMatrix,int i,double *max1)
{//��ÿһ�е����ֵ����
	int j;
	int tempmi;
	double  tempmax;

	*max1= (double )CV_MAT_ELEM(*resultPCAMatrix,double,0,i);
	tempmi=0;

	for(j=1;j< resultPCAMatrix->rows ;j++)
	{//���һ����С����ֵ  cosӦ�������ֵ��
		tempmax=(double )	CV_MAT_ELEM(*resultPCAMatrix,double,j,i) ;
		if ( tempmax >= *max1)
		{

			*max1=tempmax;
			tempmi=j;//������С Ԫ�ص�������
		}
	}
	return tempmi;
}



struct LogoTagAndFeatTag*  myMachSmallClass(CvSeq *featseq_ForBigClass,int numberOfSmallClass,
	CvMat *PictureMatrix,int numberOfPictureFeat)
{//-1���� ��ʾû��ƥ�䵽
	/*
	��������seq�õ�k��Ԫ�أ� ��ȡ��cvmat ��tag����
	mymatchsmalllclasss����
	���Ĳ���
	ԭ�����cvmat128ά��ˣ�
	�õ�����������㣬 �����Ӧtag��ǵ�featseq�С�
	*/
	int i,j,k;
	CvMat *tempmat;
	struct LogoTagAndFeatTag *temptag;
	CvMat *colsMatrix;
	CvMat * resultMatrix;
	double max1,max2;

	int maxrow;
	int logotag;

	struct IndexStructOfSmallclass* tempstruct;

	tempstruct=(struct IndexStructOfSmallclass* )cvGetSeqElem(featseq_ForBigClass,numberOfSmallClass);
	tempmat=tempstruct->MatrixForClass;
	temptag=tempstruct->logoTagAndFeatTag;

	colsMatrix=cvCreateMatHeader(PictureMatrix->rows,1,PictureMatrix->type);

	colsMatrix=cvGetCol(PictureMatrix,colsMatrix,numberOfPictureFeat);//ȡ��һ�� 128ά

	resultMatrix=cvCreateMat(tempmat->rows,colsMatrix->cols,tempmat->type);

	cvMatMul(tempmat,colsMatrix,resultMatrix);
	maxrow=getTwoMax(resultMatrix,0,&max1,&max2);



	//���������ƥ�䵽
	if( (max1 > YUZHI_COS_MAX )  || 
		(  ( max1 > YUZHI_COS_MIN ) &&  ( ((1-max1) < (1- max2)* YUZHI_COS  )  ))
		)
	{//��ӵ�seq��

		return &temptag[maxrow];
	}

	return NULL;
}


int getTwoMax(CvMat *resultPCAMatrix,int i,double *max1,double *max2)
{//��ÿһ�е����ֵ����
	int j;
	int tempmi;
	double  tempmax;

	*max1= (double )	CV_MAT_ELEM(*resultPCAMatrix,double,0,i);
	*max2= (double )	CV_MAT_ELEM(*resultPCAMatrix,double,1,i);
	tempmi=0;
	if (max1 < max2)
	{
		tempmax=*max1;
		*max1=*max2;
		*max2=tempmax;
		tempmi=1;//������ֵ��index
	}	

	for(j=2;j< resultPCAMatrix->rows ;j++)
	{//���һ����С����ֵ  cosӦ�������ֵ��
		tempmax=(double )	CV_MAT_ELEM(*resultPCAMatrix,double,j,i) ;
		if ( tempmax >= *max1)
		{
			*max2=*max1;
			*max1=tempmax;
			tempmi=j;//������С Ԫ�ص�������
		}
		else if ( tempmax >* max2)
		{
			*max2=tempmax;
		}
	}
	return tempmi;
}

MachedFeatPoint *  featToMachedFeatPoint(struct feature* tempFeat1,struct feature* tempFeat2)
{//��featתMachedFeatPoint 
	MachedFeatPoint tempFeatPoint;
	CvPoint pt1, pt2;

	pt1	 = cvPoint( cvRound( tempFeat1->x ), cvRound( tempFeat1->y ) );
	pt2 = cvPoint( cvRound(tempFeat2->x ), cvRound(tempFeat2->y ) );

	tempFeatPoint.machedPointFeat1=pt1;
	tempFeatPoint.machedPointFeat2=pt2;

	return &tempFeatPoint;
}



void releaseMycvSeqForeachLogoCeng(CvSeq ****SeqForeachLogoCeng,CvMemStorage*** *memstorage,
	int numberOfLogo)
{	//�����ͷ�seq���ڴ�
	int i,j,k;

	for (i=0; i<numberOfLogo ;i++ )
	{
		for (j=0;j<NUMBEROF_CENG;j++)
		{
			cvReleaseMemStorage(&(*memstorage)[i][j]); //�ͷ���ײ��men
		}		
		free( (*SeqForeachLogoCeng)[i] );//ÿһ��
		free ( (*memstorage)[i] );
	}

	free( (*SeqForeachLogoCeng) );//ÿ��logo
	free((*memstorage) );

}