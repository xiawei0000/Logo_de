#include "myPcaTools.h"

extern const int NUMBERFORDIVISIOM;
extern const int NUMBEROF_CENG;//logo要多少-1 层
extern double YUZHI_COS_MIN;
extern double YUZHI_COS_MAX;
extern double YUZHI_COS;
extern const double MINCOS_PCA;

void buildMycvSeqForeachLogoCeng(CvSeq ****SeqForeachLogoCeng,CvMemStorage*** *memstorage,
	int numberOfLogo)
{	//
	int i,j,k;

	//初始化logo 的seq
	(*SeqForeachLogoCeng)= (CvSeq ***)malloc(sizeof(CvSeq **)*numberOfLogo);//每个logo

	//初始化mem
	(*memstorage)= (CvMemStorage***)malloc(sizeof(CvMemStorage**)*numberOfLogo);


	for (i=0; i<numberOfLogo ;i++ )
	{
		(*SeqForeachLogoCeng)[i]=(CvSeq **)malloc( sizeof(CvSeq *) * NUMBEROF_CENG );//每一层
		(*memstorage)[i]=(CvMemStorage **)malloc( sizeof(CvMemStorage *) * NUMBEROF_CENG );

		for (j=0;j<NUMBEROF_CENG;j++)
		{
			(*memstorage)[i][j]=cvCreateMemStorage(0);//初始化内存

			(*SeqForeachLogoCeng)[i][j] =
				cvCreateSeq(0,sizeof(CvSeq),sizeof(MachedFeatPoint),(*memstorage)[i][j]);
		}	
	}
}

void reduceMachedPointscvSeqForeachLogoCeng(CvSeq ***SeqForeachLogoCeng,	int numberOfLogo)
{	//对每一个seq 缩减 1对多的匹配
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
{//找每一列的最大值两个
	int j;
	int tempmi;
	double  tempmax;

	*max1= (double )CV_MAT_ELEM(*resultPCAMatrix,double,0,i);
	tempmi=0;

	for(j=1;j< resultPCAMatrix->rows ;j++)
	{//求得一列最小两个值  cos应该是最大值啊
		tempmax=(double )	CV_MAT_ELEM(*resultPCAMatrix,double,j,i) ;
		if ( tempmax >= *max1)
		{

			*max1=tempmax;
			tempmi=j;//返回最小 元素的列坐标
		}
	}
	return tempmi;
}



struct LogoTagAndFeatTag*  myMachSmallClass(CvSeq *featseq_ForBigClass,int numberOfSmallClass,
	CvMat *PictureMatrix,int numberOfPictureFeat)
{//-1返回 表示没有匹配到
	/*
	第三步：seq得到k个元素， 获取，cvmat 和tag数组
	mymatchsmalllclasss（）
	第四部：
	原矩阵和cvmat128维相乘，
	得到最近的两个点， 加入对应tag标记的featseq中。
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

	colsMatrix=cvGetCol(PictureMatrix,colsMatrix,numberOfPictureFeat);//取得一列 128维

	resultMatrix=cvCreateMat(tempmat->rows,colsMatrix->cols,tempmat->type);

	cvMatMul(tempmat,colsMatrix,resultMatrix);
	maxrow=getTwoMax(resultMatrix,0,&max1,&max2);



	//如果两个点匹配到
	if( (max1 > YUZHI_COS_MAX )  || 
		(  ( max1 > YUZHI_COS_MIN ) &&  ( ((1-max1) < (1- max2)* YUZHI_COS  )  ))
		)
	{//添加到seq中

		return &temptag[maxrow];
	}

	return NULL;
}


int getTwoMax(CvMat *resultPCAMatrix,int i,double *max1,double *max2)
{//找每一列的最大值两个
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
		tempmi=1;//标记最大值的index
	}	

	for(j=2;j< resultPCAMatrix->rows ;j++)
	{//求得一列最小两个值  cos应该是最大值啊
		tempmax=(double )	CV_MAT_ELEM(*resultPCAMatrix,double,j,i) ;
		if ( tempmax >= *max1)
		{
			*max2=*max1;
			*max1=tempmax;
			tempmi=j;//返回最小 元素的列坐标
		}
		else if ( tempmax >* max2)
		{
			*max2=tempmax;
		}
	}
	return tempmi;
}

MachedFeatPoint *  featToMachedFeatPoint(struct feature* tempFeat1,struct feature* tempFeat2)
{//有feat转MachedFeatPoint 
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
{	//按层释放seq的内存
	int i,j,k;

	for (i=0; i<numberOfLogo ;i++ )
	{
		for (j=0;j<NUMBEROF_CENG;j++)
		{
			cvReleaseMemStorage(&(*memstorage)[i][j]); //释放最底层的men
		}		
		free( (*SeqForeachLogoCeng)[i] );//每一层
		free ( (*memstorage)[i] );
	}

	free( (*SeqForeachLogoCeng) );//每个logo
	free((*memstorage) );

}