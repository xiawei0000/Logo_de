/*
ԭ�棺 logo���С�� ����ÿ����������
ͼƬ����һ����С�������㣬
Ȼ��һһƥ��
������ÿ��logo�Ĵ�С����
����ȡ1.3Ч��ԶԶ����2

���ʱ��ļ�¼

����sigma

ͬһ��ͼ����ͬ��ɫҲ���У����Ҷ�ֵ��ͬ��

*/


#include "mach.h"

//extern  int find_Drawcontours (IplImage *src,IplImage *dst);

extern const int NUMBEROF_CENG;//logoҪ����-1 ��
extern const double NUMBERFOR_RESIZE;//ÿ����С����
extern const  int  SLIPER;//һ�� �������� =logo��10��֮һ
extern const int MIN_SLIPSIZE;  //��С����
extern const int SMALLIMAG_SIZE;//��Сlogo�Ĵ�С
extern const double SIGOLIKELY;//���ƶȵķ�ֵ
extern const double BIG_SIGOLIKELY;
extern  const int MINNUMBER_MACHING;
extern const int MINNUMBER_FORFEAT;
extern const int  chipNumber;
extern 	HANDLE  mutex;
extern  int numberForMachedPicture;
extern const char nameForPictureChange[50];
extern const int NUMBERFORDIVISIOM;

extern double YUZHI_COS_MIN;
extern double YUZHI_COS_MAX;
extern double YUZHI_COS;
extern const double MINCOS_PCA;

void myreleaseFeatStruct(LogoFeatStruct *** logofeat,
	int numberOfLogo,int NUMBEROF_CENG )
{//�ͷ�feat
	int i,j;
	for (j=0;j<numberOfLogo;j++)
	{

		for (i=0;i<NUMBEROF_CENG   ;i++)
		{
			if ((*logofeat)[j][i].feat1 != NULL )
			{			
				free( (*logofeat)[j][i].feat1) ;
			}
		}
		free((*logofeat)[j]);
	}
	free(*logofeat);
	logofeat=NULL;

}
void init_tempForThread_LogoCaculate(
struct tempStructForThread_LogoCaculate *tempstruct,
	srcIndexStruct *logo_srcIndex,LogoFeatStruct *** logofeat
	,int threadNumber,int NUMBEROF_CENG,int numberOfLogo,
	CvSize **logoSizeIndex)
{//�����̲߳���
	tempstruct->logofeat=logofeat;
	tempstruct->threadNumber=threadNumber;
	tempstruct->NUMBEROF_CENG=NUMBEROF_CENG;
	tempstruct->logo_srcIndex=logo_srcIndex;
	tempstruct->numberOfLogo=numberOfLogo;
	tempstruct->logoSizeIndex=logoSizeIndex;
}

void  CaculateLogoFeatFirst(
	srcIndexStruct *logo_srcIndex,
	LogoFeatStruct *** logofeat,
	int numberOfLogo,int NUMBEROF_CENG,
	CvSize ***logoSizeIndex)
	//,CvMat * ***LogoMatrix )
{//ֱ�Ӽ������е�logo ��feat

	/* �����̣߳�����feat�Ŀռ�
	�ṹ�崫����
	*/
	
	int i;
	int k,m;

	clock_t start, finish;
	double duration;

	int j,numberforsrc;
	HANDLE * handle;  
	struct tempStructForThread_LogoCaculate *tempForThread_men;

	struct feature *feat1;
	int realchipNumber;
		
	//*LogoMatrix=(CvMat * **)malloc( sizeof(CvMat **) * numberOfLogo );

	start = clock();//��ʼ��ʱ
	
	realchipNumber =chipNumber< numberOfLogo? chipNumber: numberOfLogo;

	tempForThread_men=(	struct tempStructForThread_LogoCaculate *)malloc(
		sizeof(	struct tempStructForThread_LogoCaculate )*realchipNumber);
	handle=(HANDLE *) malloc(sizeof(HANDLE)*realchipNumber);

	*logofeat=(LogoFeatStruct **)malloc(sizeof(LogoFeatStruct *) *numberOfLogo );
	
	//Ϊsize����ռ�
	*logoSizeIndex=(CvSize **)malloc(sizeof(CvSize *)*numberOfLogo);
	for (i=0;i<numberOfLogo;i ++ )
	{
		(*logoSizeIndex)[i]=(CvSize *)malloc(sizeof(CvSize )*NUMBEROF_CENG);
	}

	for(j=0;j<realchipNumber;j++)
	{

		init_tempForThread_LogoCaculate(&tempForThread_men[j],logo_srcIndex, logofeat, j, NUMBEROF_CENG,numberOfLogo,*logoSizeIndex);
		handle[j] = (HANDLE)_beginthreadex(NULL, 0, MyThreadFun_LogoCaculate,& tempForThread_men[j], 0, NULL);  

	}
	WaitForMultipleObjects(realchipNumber, handle, TRUE, INFINITE);  

	/*
	//��featתΪ����
	for (i=0; i<numberOfLogo ;i++ )
	{
		(*LogoMatrix)[i]=(CvMat **)malloc( sizeof(CvMat *) * NUMBEROF_CENG );
		for (j=0;j<NUMBEROF_CENG;j++)
		{
			if ((*logofeat)[i][j].numberOffeat !=0)
			{			
				(*LogoMatrix)[i][j]=cvCreateMat( (*logofeat)[i][j].numberOffeat ,NUMBERFORDIVISIOM,CV_32FC1);
				LogoFeatStructToMatrix(  (*LogoMatrix)[i][j],(*logofeat)[i][j]);
			}
			else{
				(*LogoMatrix) [i][j]=NULL;
			}
		}
		
	}

	*/
	free(handle);
	free(tempForThread_men);

	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("��ʱ��%.2lf ��",duration);
//	system("pause");
	return;
}

unsigned int __stdcall MyThreadFun_LogoCaculate(PVOID pM)  
{  //���̺߳���
	char srcLogoPicturePath[_MAX_PATH];

	IplImage* img1 ;
	IplImage** tempForLogoImg;

	struct feature* feat1 ;

	int n1, k, i, m = 0;
	int endCeng=0;

	int endi=0;
	int iForPicture;
	int logoNumber;//�ڼ���logo
	int starti;

	//������
	int threadNumber;
	int numberOfLogo;
	srcIndexStruct *logo_srcIndex;
	LogoFeatStruct *** logofeat;
	int NUMBEROF_CENG;
	CvSize **logoSizeIndex;

	numberOfLogo=((struct tempStructForThread_LogoCaculate*)pM )->numberOfLogo;//logo����
	threadNumber=((struct tempStructForThread_LogoCaculate*)pM )->threadNumber;//�ڼ����߳�
	logo_srcIndex=((struct tempStructForThread_LogoCaculate*)pM )->logo_srcIndex;//logo��ַ����
	logofeat=((struct tempStructForThread_LogoCaculate*)pM )->logofeat;//���������� �͸���������
	NUMBEROF_CENG=((struct tempStructForThread_LogoCaculate*)pM )->NUMBEROF_CENG;
	logoSizeIndex=((struct tempStructForThread_LogoCaculate*)pM )->logoSizeIndex;
	if (numberOfLogo /chipNumber==0)
	{//chip �� logo��Ŀ
		starti=threadNumber;//ֱ�����߳����Ϊͼ���
		endi=threadNumber+1;
	}
	else
	{
		starti=numberOfLogo /chipNumber *threadNumber;
		//��ʾ�ڼ����߳�
		if (threadNumber==chipNumber-1)//���һ���߳�
		{
			endi=numberOfLogo;
		}
		else
		{
			endi=numberOfLogo /chipNumber *(threadNumber+1);
		}
	}


	for(iForPicture = starti; iForPicture< endi ;iForPicture++)
	{//�̵߳�ÿһ��ͼ

		logoNumber=iForPicture;//logo��� ��0��ʼ��
		strcpy(srcLogoPicturePath,logo_srcIndex[logoNumber].srcIndex);	

		(*logofeat)[logoNumber]=(LogoFeatStruct *)malloc(sizeof(LogoFeatStruct ) *NUMBEROF_CENG );

		img1 = cvLoadImage( srcLogoPicturePath, 1 );	
		if (img1 ==NULL)
		{
			printf("û�ж�ӦͼƬ %s",srcLogoPicturePath);
			continue ;//����û��ͼƬ
		}

		endCeng=0;


		tempForLogoImg=(IplImage**)malloc(sizeof(IplImage*)*NUMBEROF_CENG);
		//��ԭͼ  �ȶ����С������feat
		for (i=0;i<NUMBEROF_CENG   ;i++)
		{//����
			if (i ==0)
			{   
				tempForLogoImg[i]=cvCloneImage(img1);
			}  
			if(i!=0)
				tempForLogoImg[i]= smallSizeImg( tempForLogoImg[i-1],NUMBERFOR_RESIZE);

			endCeng=i;
			
			//���size�ľ���
			(logoSizeIndex)[iForPicture][i].width=tempForLogoImg[i]->width;
			(logoSizeIndex)[iForPicture][i].height=tempForLogoImg[i]->height;

			if (isTooSmall( tempForLogoImg[i]))
			{//й¶��

				for (;i< NUMBEROF_CENG ;i++)
				{
					(*logofeat)[logoNumber][i].feat1=NULL;
					(*logofeat)[logoNumber][i].numberOffeat=0;
					
					//���size�ľ���
					(logoSizeIndex)[iForPicture][i].width=0;
					(logoSizeIndex)[iForPicture][i].height=0;
				}
				break;//logo ̫С��
			}


			//��������
			fprintf( stderr, "Finding features in %s ----%d ...\n", srcLogoPicturePath,i );
			n1 = sift_features(  tempForLogoImg[i], &feat1 );
			fprintf( stderr, " features %d\n", n1 );
			//(*logofeat)[j][i]= (LogoFeatStruct *)malloc(sizeof(LogoFeatStruct));
			(*logofeat)[logoNumber][i].feat1=feat1;
			(*logofeat)[logoNumber][i].numberOffeat=n1;
		}//ÿһ��

		for (i=0;i<=endCeng ;i++)
		{
			cvReleaseImage(&tempForLogoImg[i]);
		}


		cvReleaseImage(&img1);
	}//ÿһ��ͼ
		return 0;
}


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
	int threadNumber)
{
	( ( struct tempStructForThread_EachPicture*)pM )-> 	Picture_srcIndex=Picture_srcIndex;
	( ( struct tempStructForThread_EachPicture*)pM )->  logo_srcIndex=logo_srcIndex;
	( ( struct tempStructForThread_EachPicture*)pM )->  numberOfPicture=numberOfPicture;
	( ( struct tempStructForThread_EachPicture*)pM )->   numberOfLogo=numberOfLogo;
	( ( struct tempStructForThread_EachPicture*)pM )->  logofeat=logofeat;

	( ( struct tempStructForThread_EachPicture*)pM )->  AvgVector=AvgVector;
	( ( struct tempStructForThread_EachPicture*)pM )->  EigenVector=EigenVector;
	( ( struct tempStructForThread_EachPicture*)pM )->  BigIndex=BigIndex;
	( ( struct tempStructForThread_EachPicture*)pM )->  featseq_ForBigClass=featseq_ForBigClass;
	( ( struct tempStructForThread_EachPicture*)pM )->  logoSizeIndex=logoSizeIndex;

	( ( struct tempStructForThread_EachPicture*)pM )-> threadNumber=threadNumber;

}

 

unsigned int __stdcall MyThreadFun_EachPicture(PVOID pM)  
{	
	//�̲߳���
	srcIndexStruct *Picture_srcIndex;
		srcIndexStruct *logo_srcIndex;
		int numberOfPicture;
		int numberOfLogo;
		LogoFeatStruct ** logofeat;
		//CvMat  ***LogoMatrix;
		CvMat *AvgVector;
		CvMat *EigenVector;
		CvMat *BigIndex;
		CvSeq *featseq_ForBigClass;
		CvSize **logoSizeIndex;
		int threadNumber;


//	CvMat **tempMatrix;

	CvMat *PictureMatrix;
	CvMat *PictureMatrix_PCA;
	CvMat *PictureMatrix_PCA_MO_T;
	CvMat *resultPCAMatrix;


	char srcTestPicturePath[_MAX_PATH];//����ȡ�ĵ�ַ
	char srcLogoPicturePath[_MAX_PATH];
	IplImage* img2;
	struct feature  * feat2;
	int n2;

	char  tempNameForPictureChange[_MAX_PATH];
	char *nameForPictureChange="_Mach_";
	char *nameForPictureSub="��ͼ_";
	char tempnumber[10];
	char dstTestPicturePath[_MAX_PATH];

	int tagForMatched=0;
	int tagForMatchedLogo;
	IplImage **imgForPicture;
	int numberOfpices;
	int k;
	int numberOfMachedLogo;

	int tempPictureNumber;
	int endi=0;
	int iForPicture;
	int stari;

	CvRect resultRect;//ƥ������򷵻ؽ��

	//�̲߳�������
	Picture_srcIndex=( ( struct tempStructForThread_EachPicture*)pM )-> 	Picture_srcIndex;
	logo_srcIndex=( ( struct tempStructForThread_EachPicture*)pM )->  logo_srcIndex;
	numberOfPicture=( ( struct tempStructForThread_EachPicture*)pM )->  numberOfPicture;
	numberOfLogo=( ( struct tempStructForThread_EachPicture*)pM )->   numberOfLogo;
	logofeat=( ( struct tempStructForThread_EachPicture*)pM )->  logofeat;
	

	//��������
	//LogoMatrix=( ( struct tempStructForThread_EachPicture*)pM )->  LogoMatrix;
	AvgVector=( ( struct tempStructForThread_EachPicture*)pM )->  AvgVector;
	EigenVector=( ( struct tempStructForThread_EachPicture*)pM )->  EigenVector;
	BigIndex=( ( struct tempStructForThread_EachPicture*)pM )->  BigIndex;
	featseq_ForBigClass=( ( struct tempStructForThread_EachPicture*)pM )->  featseq_ForBigClass;
	logoSizeIndex=( ( struct tempStructForThread_EachPicture*)pM )->  logoSizeIndex;

	threadNumber= ( ( struct tempStructForThread_EachPicture*)pM )->threadNumber;

	//���ͼƬ��С���߳��� ��ô�߳̿�ʼ��min��ͼ��Ŀ��chipernumber��
	if (numberOfPicture /chipNumber==0)
	{
		stari=threadNumber;//ֱ�����߳����Ϊͼ���
		endi=threadNumber+1;
	}
	else
	{
		stari=numberOfPicture /chipNumber *threadNumber;

		 //��ʾ�ڼ����߳�
		if (threadNumber==chipNumber-1)//���һ���߳�
		{
			endi=numberOfPicture;
		}
		else
		{
			endi=numberOfPicture /chipNumber *(threadNumber+1);
		}
	}

	for(iForPicture = stari; iForPicture< endi ;iForPicture++)
	{//��ÿһ������ͼ

//��һ�����õ�����ͼ���ֿ鲢����������
		//	logoNumber+=i;
		tempPictureNumber=iForPicture;
		tagForMatched=0;//�����logo


		//����ʱ��
		//	saveTime(j);

		strcpy(srcTestPicturePath,Picture_srcIndex[tempPictureNumber].srcIndex);

		/* ͼƬ���� */
		img2 = cvLoadImage( srcTestPicturePath, 1 );
		//�����ж϶���Ķ���
		if (img2 ==NULL)
		{
			printf("û�ж�ӦͼƬ %s",srcTestPicturePath);
			continue ;//����û��ͼƬ
		}

		numberOfpices= myDivedeImg(&imgForPicture, img2);

		cvReleaseImage( &img2 );
		for( k=0;k< numberOfpices  &&tagForMatched==0; k++)
		{	
			if (isTooSmall(imgForPicture[k]) )
			{
				continue;//��ͼ����ܺ�С
			}

			fprintf( stderr, "Finding features in %s...\n", srcTestPicturePath );
			n2 = sift_features( imgForPicture[k], &feat2 );//ȡ��������
			fprintf( stderr, " features %d\n", n2 );


			if ( n2 >MINNUMBER_FORFEAT)
			{//������̫�پͲ�Ҫƥ����
				draw_features( imgForPicture[k],feat2,n2);
		//ƥ���һ���� feat������
			//����ʹ�þ���ת��
				PictureMatrix=cvCreateMat(n2,NUMBERFORDIVISIOM,CV_64FC1);
				featToMatrixWithoutTranslate(PictureMatrix,feat2,n2);//feat������,��Ҫת�ã�

/*//ȫ����д�� ֱ�� pca��ƥ�� ����Ȼ����ƥ��ڶ������
AvgVector= 
EigenVector=
				BigIndex= 
				featseq_ForBigClass

	��һ���� ͼƬ�������� ת���� ��  pca��ά
	�ڶ�����pca�����bigindex ��ˣ�
	for ÿһ��{
	����С�Ƕ� ������i
	{
	��������seq�õ�i��Ԫ�أ� ��ȡ��cvmat ��tag����
	���Ĳ���ԭ�����cvmat128ά��ˣ�
	�õ�����������㣬 ����seq�С�

	}
	ȡǰ����3��seq��
	ÿ���õ���Ӧ�� featlogo ����
			��ʼ�ú�������
*/

//��һ���� ͼƬ�������� ת���� ��  pca��ά
			//����ʹ��feat->����ת��
			//���� -��pca
				PictureMatrix_PCA=cvCreateMat(PictureMatrix->rows,BigIndex->cols,PictureMatrix->type);
					
				
				//cvReleaseMat(&PictureMatrix_PCA);//�����ͷ�  ��άǰ�����ͷ�

				cvProjectPCA(PictureMatrix,AvgVector,EigenVector,PictureMatrix_PCA);//��ɽ�ά
				//cvReleaseMat(&PictureMatrix_PCA);//�����ͷ�  �����ͷš�������
				//��ά����ת�ã� ��mo


				//����ʹ��
				PrintMatrix(PictureMatrix);		
				PrintMatrix(PictureMatrix_PCA);


				PictureMatrix_PCA_MO_T=cvCreateMat(PictureMatrix_PCA->cols,PictureMatrix_PCA->rows,PictureMatrix_PCA->type);
				MatrixTranslateAndMO(PictureMatrix_PCA,PictureMatrix_PCA_MO_T);
				cvReleaseMat(&PictureMatrix_PCA);//�����ͷ� ��ά�󲻿����ͷš�������

				PrintMatrix(PictureMatrix_PCA_MO_T);


		//ƥ��ڶ�����pca�����bigindex ��ˣ�
				resultPCAMatrix=cvCreateMat(BigIndex->rows,PictureMatrix_PCA_MO_T->cols,BigIndex->type);

				cvMatMul(BigIndex,PictureMatrix_PCA_MO_T,resultPCAMatrix);

		//ƥ�������������logoIndex ��p�ľ���˻��� �ٴ�ƥ��smalllclasss �õ���ȷƥ�䡣 
				//���յõ��Ƿ�ƥ����
				//���������������������������������봫�� img2��logo��size����
				

				//�޸� ���ڴ���ǰ������ ģ 
				caculateMO(PictureMatrix);

				//����
				PrintMatrix(PictureMatrix);

				tagForMatched=myMachALLL(resultPCAMatrix,featseq_ForBigClass,	
					logofeat , numberOfLogo,
					feat2,PictureMatrix
					,imgForPicture[k],
					logo_srcIndex,logoSizeIndex,
					&resultRect,&numberOfMachedLogo);//�����Ƿ�ƥ�䵽��
	
				
				cvReleaseMat(&PictureMatrix_PCA_MO_T);
				cvReleaseMat(&resultPCAMatrix);

			}
			
			if (tagForMatched)
				{		//ÿ��ƥ��� tagForMakeIt���ᱻ���£�������tagForMatched �������Ƿ�ƥ��
					//	tagForMatchedLogo=i;
					 WaitForSingleObject(mutex, INFINITE);  
					//��Ҫͬ������
						 SaveResult(tempPictureNumber ,k ,numberOfMachedLogo);
					
						//ƥ�䵽�ˣ�����ͼƬ
						myDrawRectToImage(imgForPicture[k],resultRect);
				
						myMachedPictureFilePath(srcTestPicturePath,dstTestPicturePath,
							k,numberOfMachedLogo);
						//Ҫ�½��ļ��У�����
						myCreateFatherFile(dstTestPicturePath);	//�õ����屣���·��
						cvSaveImage(dstTestPicturePath,imgForPicture[k],0);//����ͼƬ
					  ReleaseMutex(mutex);  
			}
				
			//tongbu
			/*
			 WaitForSingleObject(mutex, INFINITE);  
			 SaveAReturn();
			  ReleaseMutex(mutex);  
			*/
			
			free( feat2 );feat2=NULL;
			
			cvReleaseMat(&PictureMatrix_PCA_MO_T);
			cvReleaseMat(&PictureMatrix);
			//cvReleaseMat(&PictureMatrix_PCA);
			cvReleaseMat(&resultPCAMatrix);
	 }	//ÿһ��ͼ����


		for( k=0;k< numberOfpices; k++)
		{	
			cvReleaseImage(  &imgForPicture[k]);
		}
		free(imgForPicture); imgForPicture=NULL;

	
		if(tagForMatched)
		{
			WaitForSingleObject(mutex, INFINITE);  
		//	 SaveResult(tempPictureNumber ,0 ,tagForMatchedLogo);
			//��Ҫͬ������
			mySqlInsertResult(Picture_srcIndex[tempPictureNumber].id,0);//logoid������
			//	logo_srcIndex[i].id);//�����ӽ��

			numberForMachedPicture++;
			ReleaseMutex(mutex);  					
		}
			/**/
	}

	return 0;
}


int  myMachALLL(CvMat *resultPCAMatrix,CvSeq *featseq_ForBigClass,
	LogoFeatStruct ** logofeat , int numberOfLogo,
	struct feature  * feat2,CvMat *PictureMatrix,
	IplImage *img2,
	srcIndexStruct *logo_srcIndex,CvSize **logoSizeIndex,
	CvRect *resultRect,
	int *numberOfMachedLogo/*���ڽ������,ƥ�䵽���ĸ�logo*/

	)
{/* ����bigindex����
 for ÿһ��{
 ����С�Ƕȡ�cos��ֵ   ��������=������ ƥ��Ĵ���k
 {
  ��������logo���280��seqcolitionfeat�������飬
	 ��������seq�õ�k��Ԫ�أ� ��ȡ��cvmat ��tag����
	 
	 mymatchsmalllclasss����
	 ���Ĳ���
	 ԭ�����cvmat128ά��ˣ�
	 �õ�����������㣬 �����Ӧtag��ǵ�featseq�С�
 }
 ȡǰ����3��seq��
 ÿ���õ���Ӧ�� featlogo ����
 ��ʼ�ú�������
 
  ���أ��Ƿ�ƥ�䵽��   ƥ�䵽��rect
 */
	int i,j,k;
	int maxrows;
	double max1,max2,tempmax;

	struct LogoTagAndFeatTag* logotag;
	int cheng,logolei,featnumber;
	int maxrowInSmall;
	
	struct LogoTagAndFeatTag*index;//����seq ��total�����Ľ��

	struct feature* feat;
	struct feature* FeatforPictrue;
	struct feature* FeatForLogo;

	MachedFeatPoint *tempFeatPoint;

	CvSeq ***SeqForeachLogoCeng;
	CvSeq *Seqtemp;
	CvMemStorage** *memstorage;

	int tagForMakeIt;

	/*��ʼ������Ϊÿһ��logo��ÿһ�㣬����seq��	*/
	buildMycvSeqForeachLogoCeng(&SeqForeachLogoCeng,&memstorage, numberOfLogo);

	for (i=0 ;i<resultPCAMatrix->cols  ;i++)
	{//ÿһ��
		maxrows=getOneMax(resultPCAMatrix, i,&max1);

		//maxrows+>����������Ӧ �������ţ� i���У���ӦͼƬ�ĵ�i��������
		if (max1 >MINCOS_PCA)
		{//��һ��С���
			logotag=myMachSmallClass(featseq_ForBigClass,maxrows,PictureMatrix,i);//���� ƥ�䵽��logo��Ӧ��tagֵ

			if ( logotag  != NULL)
			{			
			//ȡ��logofeat��tag�� Ȼ��ȡ��picture�ġ�i���� ������seq��
			//�ֽ�����logo���� ��Ӧfeat����

				logolei=logotag->tagForClass;
				cheng=logotag->tagForCeng;
				featnumber=logotag->tagForFeatNumber;
				feat=logofeat[logolei][cheng].feat1;
				FeatForLogo=feat+featnumber;//��λlogo��feat

				FeatforPictrue=feat2+i;
				tempFeatPoint=featToMachedFeatPoint(FeatForLogo,FeatforPictrue);

				//cvSeqPush(featseq,&tempFeatPoint);//ѹ�� ƥ�䵽�� ͼƬ�ϵĵ�
				/**��һ��������seq����
				һ���������Ӧ��seq
				��ǰ����seq����simֵ	*/

				cvSeqPush(SeqForeachLogoCeng[logolei][cheng],tempFeatPoint);
			}
		}
	}

	 //ȡ��һ�Զ�
	reduceMachedPointscvSeqForeachLogoCeng(SeqForeachLogoCeng, numberOfLogo);
	
	
	//����ֱ�� ����  ȡ����ǰn��seq��ɨ��simֵ��
	mysortTotalSeq(SeqForeachLogoCeng, numberOfLogo,NUMBEROF_CENG, &index );
	tagForMakeIt=0;
	for (k=0; k<5 && tagForMakeIt ==0 ;k++)
	{//ֻ�ж�ǰN����
		if(index[k].tagForFeatNumber >5)//���ٴ���5����
		{//������total�� �࣬��
			i= (index[k].tagForClass);
			j=(index[k].tagForCeng);

			Seqtemp=SeqForeachLogoCeng[ i][ j];
			

			//???????????????????????????????????????��ʾ��
		//	���¶���logo �������Σ�Ȼ��stack

			//�����ÿһ��seq�ļ���simֵ
			tagForMakeIt= myLastMached_seqFeat_Fea2_Feat1( logo_srcIndex,
				i,j,
				logoSizeIndex[i][j], img2, 
				logofeat[i][j].feat1, feat2, 
				Seqtemp,  
				logofeat[i][j].numberOffeat,PictureMatrix->rows, 	Seqtemp->total
				,resultRect);
			//���̣� ÿһ��seq�ĵ㣬 ��λ���ڴ�С�� ��sim�� �ж�sim

			if (tagForMakeIt)
			{
				*numberOfMachedLogo=i;
				break;
			}
		//ƥ�䵽�˾�break��
		}
	}

	//�ͷ��ڴ�  ����index��û��������
	releaseMycvSeqForeachLogoCeng(&SeqForeachLogoCeng,&memstorage, numberOfLogo);

	return tagForMakeIt;
}

/*
int machEachPicture(int numberForLogo,LogoFeatStruct ** logofeat,CvMat** tempMatrix,
	char* srcLogoPicturePath,IplImage * img2_orig, 
struct feature  * feat2,CvMat *PictureMatrix,int n2,char *dstTestPicturePath)
{//һ��logo�Ĳ�ͬ�� �Բ���ͼ��һ��pice  ƥ��



  IplImage* img1 , * stacked;
  IplImage** tempForLogoImg;
  CvMat multiMat;
  struct feature* feat1 ;
  struct feature** nbrs;
 
  CvPoint pt1, pt2;
  double d0, d1;
  int n1, k, i,j, m = 0;
  CvMat *tagMat ;
  IplImage* tagImag,* tagImag2;
  char * nameofWindow[20]={"0","1","2","3","4","5","6","7","8","9","10"};

  int startCeng=0,endCeng=0;
  int  preRetuurn=0;
  int tagForMakeIt=0;
  
  IplImage *img2=cvCloneImage(img2_orig);

 // struct feature  *feat2 = (struct feature  *)calloc( n2, sizeof(struct feature) );//��feat3 ����һ�ݣ�Ȼ������޸�
  struct feature  * feat3;
  int  machedPointsnumber;

  int numberForFeat;
  //����ƥ�䵽�ĵ�Ĵ���
  CvSeq *featseq;
  CvMemStorage* storage ;

  CvMat* tempMatrixForLogo;

  storage = cvCreateMemStorage( 0 );
  featseq=cvCreateSeq(0,sizeof(CvSeq),sizeof(MachedFeatPoint),storage);


  //���Ժܺõ��Ż� �ڴ�ռ�  ֻ��һ������
  img1 = cvLoadImage( srcLogoPicturePath, 1 );

  tagMat=cvCreateMat(img2->height,img2->width,CV_8SC1);//��Ǿ���

  cvZero(tagMat);

  tempForLogoImg=(IplImage**)malloc(sizeof(IplImage*)*NUMBEROF_CENG);

  //��ԭͼ  �ȶ����С������feat
  for (i=0;i<NUMBEROF_CENG   && tagForMakeIt ==0 ;i++)
  {//����
	  if (i ==0)
	  {   
		  tempForLogoImg[i]=cvCloneImage(img1);
	  }  
	  if(i!=0)
	  tempForLogoImg[i]= smallSizeImg( tempForLogoImg[i-1],NUMBERFOR_RESIZE);
 
	  if (Img1BigImg2( tempForLogoImg[i],img2))
	  {
		  startCeng=i+1;
		  continue;//logo>ͼ
	  }
	  endCeng=i;
	  if (isTooSmall( tempForLogoImg[i]))
	  {
		  endCeng=i-1;
		  break;//logo ̫С��
	  }


		  {//��������
			 numberForFeat=numberForLogo ;
			 n1 = logofeat[numberForFeat][i].numberOffeat;
			 feat1=logofeat[numberForFeat][i].feat1;
			 tempMatrixForLogo=tempMatrix[i];//�õ�������ľ���

			 stacked = stack_imgs(  tempForLogoImg[i], img2 );
			 //һ����ʾ����壨�����ĸ߶Ⱥͣ���������Ҳ����

			  if ( n1 > MINNUMBER_FORFEAT)
			  {			  				  
				  cvClearSeq(featseq);
				  //Ԥ��ƥ��
				  machedPointsnumber=compareFeartureFirstTime(feat1,tempMatrixForLogo,feat2,PictureMatrix,n1,n2, tempForLogoImg[i],stacked,featseq);

				  //��ǰ�ж� feat �� ��ֵ
				  if (machedPointsnumber > ( n1 *SIGOLIKELY )/3 )
				  {
					//�������ڱȽ�
					  tagForMakeIt= sliperWindows( tempForLogoImg[i], img2, stacked,   feat1, feat2, featseq,  n1,n2, 
						  machedPointsnumber,tagMat);
							//  cvWaitKey(0);
				  }

			   }
			  
		
			  if (stacked  !=NULL )
			  {
				  cvReleaseImage( &stacked );
				  stacked=NULL;
			  }
		  }
  }

 cvReleaseMemStorage(&storage);
 
  //��ʾtagmat  CvMat תimg
  tagImag=cvCreateImageHeader(cvGetSize(tagMat),IPL_DEPTH_8U ,1);
  cvGetImage(tagMat,tagImag);
  tagImag->depth=8;
  fprintf(stderr," %d \n", tagImag->depth);

  cvThreshold(tagImag,tagImag,0.5,254,CV_THRESH_BINARY); 
  find_Drawcontours(tagImag,img2);

  if (tagForMakeIt)
  {//ֱ�ӷ��� dstTestPicturePath�� �ϲ�Ŀ¼

	  myCreateFatherFile(dstTestPicturePath);
	//Ҫ�½��ļ��У� ����
	cvSaveImage(dstTestPicturePath,img2,0);//����ͼƬ
  }
  cvReleaseImageHeader(&tagImag);//cvGetImage() �� ֻ�ǽ�mat �����ݵ�ַ���� img��imgֻҪһ��ͷ�ֹ��ˣ����Բ����ͷ�img��ֻ���ͷ�ͷ
  tagImag=NULL;
  cvReleaseMat(&tagMat); 
  tagMat=NULL;


  cvReleaseImage( &img1 );
  cvReleaseImage( &img2 );
  img1=NULL;
   
  for (i=0;i<=endCeng;i++){	 
	  cvReleaseImage(&tempForLogoImg[i]);
	  tempForLogoImg[i]=NULL;
  }

  free(tempForLogoImg);tempForLogoImg=NULL;
  return tagForMakeIt;
}

*/


int judgeFormPiont(struct feature* feat1,struct feature * feat2,CvSeq * featseq,
	int n1,int n2,int n3,
	IplImage*img1,double liangduImg1,
	IplImage * img2,IplImage*stacked ,
	CvPoint pointWindow1,CvPoint  pointWindow2)
{//��pointWindow1 pointWindow2֮��ľ����ҵ����������㣬��һһƥ��
	int k,j;	
	int muberOfpoint=0;	
	CvSeq* features2,* features3;
	CvMemStorage* storage2,* storage3;
	int simoffearture=0;
	
	struct feature* feat  ,*tempfeat;
	int cengForImag1,cengForImag2;



	MachedFeatPoint *machedstruct,*machedstructtemp;
	double lingduImg2;
	int numberOfMached;//ƥ�䵽�ĵ�
	muberOfpoint=0;//����ͼƬ�ڴ����ڵĵ���Ŀ

	storage3 = cvCreateMemStorage( 0 );//��ʼ��seq
	features3 = cvCreateSeq( 0, sizeof(CvSeq), sizeof(MachedFeatPoint), storage3 );
 
	/*
	storage2 = cvCreateMemStorage( 0 );//��ʼ��seq
	features2 = cvCreateSeq( 0, sizeof(CvSeq), sizeof(struct feature), storage2 );
	cvSeqPush( features2, feat )	;
*/

	for (k=0,feat=feat2;k<n2;k++ )
	{//���д����ڵ�ƥ��
		feat=feat2+k;
		if (feat->x >=pointWindow1.x && feat->y> pointWindow1.y && 
			feat->x <=pointWindow2.x  && feat->y <  pointWindow2.y  )
		{		
			muberOfpoint++;
		}					
	}


	numberOfMached=0;
	for (k=0;k<n3;k++ )
	{//���д����ڵ�ƥ��
		machedstruct=(MachedFeatPoint *)cvGetSeqElem(featseq,k);
		if (compareIsInWindow(machedstruct,pointWindow1,pointWindow2) )
		{
			cvSeqPush( features3, machedstruct );
			numberOfMached++;
		}					
	}


	//ͼƬһ�������� ֻ���ǵ�����ƥ�䵥��logo�㣬 ���ͼƬ��ƥ��һ��logo����Ǵ���ģ�
	for (k=0; k<numberOfMached; k++)
	{
		machedstruct=(MachedFeatPoint *)cvGetSeqElem(features3,k);
		for( j=k+1;j<numberOfMached; j++)
		{
			machedstructtemp=(MachedFeatPoint *)cvGetSeqElem(features3,j);

			if (compareIsEqual( machedstruct , machedstructtemp ) )
			{
				cvSeqRemove(features3,j);
				numberOfMached--;
			}


		}

	}


	if (muberOfpoint  > 4)//�����ڵ�̫�٣���ֱ������
	{

	//�����޸ģ� �ȼ�������ͼ�����ȣ� Ȼ����� simֵ
		lingduImg2=ComputeLiangdu(img2,pointWindow1,cvGetSize(img1));

		simoffearture= simOfFearture(n1,muberOfpoint,numberOfMached,liangduImg1,lingduImg2 );
		if( simoffearture  )
		{
			cvReleaseMemStorage(&storage3);
			return simoffearture;
		}
	}


//	cvReleaseMemStorage(&storage2);
	cvReleaseMemStorage(&storage3);
	//ǧ��ǧ��Ҫ��ס�� ��ǰ�˳�һ��Ҫ �ͷ�


	return 0;
}

int myLastMached_seqFeat_Fea2_Feat1( srcIndexStruct *logo_srcIndex,
	int lei,int ceng,//��ʱ����Բ�ʹ��
	CvSize logoSize,IplImage* img2, 
	struct feature*feat1,struct feature* feat2, 
	CvSeq *featseq, 
	int n1,int n2,int n3
	,CvRect *resultRect)
{/*
 1.ͨ�� srcindex ��ȡlogoͼƬ�������ã� �õ���Ӧ�Ĳ��logo��ͼ��
 2.ѹվ��stack�ֲ�����
 
 3.��ʽƥ�䣺����sliperwindow����
 ���̣� ÿһ��seq�ĵ㣬 ��λ���ڴ�С�� ��sim�� �ж�sim

 ���أ��Ƿ�ƥ�䵽��   ƥ�䵽��rect
 */
	int i,j;
	IplImage*img1;
	IplImage* tempForLogoImg;
	IplImage* stacked;



	int tagForCompare=0;	



	img1=cvLoadImage(logo_srcIndex[lei].srcIndex,1);

	tempForLogoImg=caculateSmallSizeImg( img1 ,NUMBERFOR_RESIZE,ceng);//��С����Ӧ�Ĳ��logo
	cvReleaseImage(&img1);

	stacked = stack_imgs( tempForLogoImg, img2 );

	tagForCompare=judgMachedFromImgaToImage(tempForLogoImg,img2,stacked,
	feat1, feat2,featseq,
	n1, n2, n3, 
	resultRect);//ƥ����
		//������  ��ֱ�ӷ���һ��rect��Ȼ�󣬻���img2�ϣ�����ͼƬ��

	cvReleaseImage(&stacked);
	return tagForCompare;
}

int judgMachedFromImgaToImage(IplImage*img1,IplImage*img2,IplImage*stacked,
struct feature* feat1,struct feature * feat2,CvSeq *featseq,
	int n1,int n2,int n3,
	CvRect *resultRect)
{/*������ͼ �������� �ж�ͼƬ�Ƿ�ƥ��
	����imge2��ƥ������reck���ɵ��ú��� �����ƾ��Σ�������
 */

	int i,j,k;

	CvPoint pointWindow1,pointWindow2;

	int ImgWidth=img2->width;
	int ImgHight=img2->height;
	CvSize sizeImg1=cvSize(img1->width,img1->height);
	CvSize sizeImg2=cvSize(img2->width,img2->height);
	MachedFeatPoint* featTemp;

	double liangduImg1=0.0;
	int tagForCompare=0;	

	/*�����Ż��ĵط���
	 �õ�ƥ������ֵ��Ȼ��õ�maxmin  �Ĵ��ڵ�*/
	//��logo���� ������ƥ���
	liangduImg1=ComputeLiangdu(img1,cvPoint(0,0),sizeImg1);
		
	tagForCompare=0;

	/*��ÿһ��ƥ�䵽�ĵ�  ��ȷ�� ͼƬlogo������ �ٱȽ� sim*/

	for(i=0;i<n3  && tagForCompare ==0 ;i++)
	{
		featTemp =(MachedFeatPoint*) cvGetSeqElem(featseq,i);
		
		GetMachedWindowPoint( featTemp, &pointWindow1, &pointWindow2, sizeImg1 ,sizeImg2);
	
		tagForCompare=judgeFormPiont( feat1,  feat2, featseq, n1,  n2,n3, img1,liangduImg1,img2, stacked , pointWindow1,  pointWindow2 ) ;
		if(tagForCompare )
		{//ƥ���img2 ������
			resultRect->x=pointWindow1.x;
			resultRect->y=pointWindow1.y;
			resultRect->width=pointWindow2.x- pointWindow1.x;
			resultRect->height=pointWindow2.y-pointWindow1.y;
		}
	}

	return  tagForCompare ;
}



int compareFeartureFirstTime( struct feature* feat1,CvMat * tempMatrixForLogo,
struct feature* feat2,CvMat *PictureMatrix,
int n1,int n2,
IplImage* img1,IplImage* stacked,	CvSeq *featseq) 
{   //������������������ ��ƥ�����������ͼ
	//���ٷ��������㣬���Ƿ���ƥ�������� +logo ��Ӧ������

	/*�����ʹ�þ��� ��� �õ�������С�㣬 ��ֵ��  ����seq��
		1��������ˣ�
		2����ÿһ����С����ֵ��
		3.�б𣬡���ֵ�������ˣ�
	*/
	struct feature** nbrs;
	struct kd_node* kd_root;
		struct feature* feat;

	struct feature* tempFeat1;
	struct feature* tempFeat2;
	int  k, i, m = 0;
	int m2=0;
	int simoffearture=0;
	CvPoint pt1, pt2;
	double d0, d1;
	IplImage*stackedTemp;

	int mi,nj;
	int tempmi;
	double max1,max2,tempmax;
	MachedFeatPoint tempFeatPoint;
	CvMat * resultMatrix;

	/*
	//������
	CvSeq *testfeatseq;
	CvMemStorage* storage ;
	IplImage* tempstackedTemp;
	*/


		clock_t start, finish;
		double duration;
	start = clock();//��ʼ��ʱ


	stackedTemp=cvCloneImage(stacked);

	resultMatrix=cvCreateMat(tempMatrixForLogo->rows,PictureMatrix->cols,tempMatrixForLogo->type);
 
	cvMatMul(tempMatrixForLogo,PictureMatrix,resultMatrix);


	//printMyMat(resultMatrix);
	//��ѯ���󣬷��� ��һ�ڶ�������飬
	for (nj=0;nj<PictureMatrix->cols ;nj++)
	{
		//�����������ʱ��͹���������5����ġ�
		max1= (double )	CV_MAT_ELEM(*resultMatrix,double,0,nj);
		max2= (double )	CV_MAT_ELEM(*resultMatrix,double,1,nj);
		tempmi=0;
		if (max1 < max2)
		{
			tempmax=max1;
			max1=max2;
			max2=tempmax;
			tempmi=1;//������ֵ��index
		}	
		
		for(mi=2;mi< tempMatrixForLogo->rows ;mi++)
		{//���һ����С����ֵ  cosӦ�������ֵ��
			tempmax=(double )	CV_MAT_ELEM(*resultMatrix,double,mi,nj) ;
			if ( tempmax >= max1)
			{
				max2=max1;
				max1=tempmax;
				tempmi=mi;
			}
			else if ( tempmax > max2)
			{
				max2=tempmax;
			}
		}
		tempmax=	 (1- max2)  * YUZHI_COS-   (1-max1)    ;
		//�жϱ�ֵ ����ֵ��
		if (  (max1 > YUZHI_COS_MAX )  || 
			(  ( max1 > YUZHI_COS_MIN ) &&  ( ((1-max1) < (1- max2)* YUZHI_COS  )  ))
			 ) 
		{ //

			tempFeat1=feat1+tempmi;
			tempFeat2=feat2+nj;
			pt1	 = cvPoint( cvRound( tempFeat1->x ), cvRound( tempFeat1->y ) );
			pt2 = cvPoint( cvRound(tempFeat2->x ), cvRound(tempFeat2->y ) );
			pt2.y += img1->height;
			cvLine( stackedTemp, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//ƥ�䵽�ľͻ���
			m++;
		
			tempFeatPoint.machedPointFeat1=cvPoint( cvRound( tempFeat1->x ), cvRound(tempFeat1->y ) );
			tempFeatPoint.machedPointFeat2=cvPoint( cvRound( tempFeat2->x ), cvRound(tempFeat2->y ) );
			cvSeqPush(featseq,&tempFeatPoint);//ѹ�� ƥ�䵽�� ͼƬ�ϵĵ�
		}

	}
    reduceMachedPoints(featseq,&m);
	/*
	if ( m>5)
	{	
	 	myDrawPoint(featseq, m, stacked, img1->height);
	 	printf(" ����ƥ�䣺 %d  ====",m);

	}*/

	// display_big_img( stackedTemp, "first_Matches" ); // ������
//	 cvWaitKey(0);  �ر����⣻ opencv��waitkek����ֹ���̵߳Ľ�����
	// myDrawPoint(featseq, m, stacked, img1->height);
	/*
	printf(" ����ƥ�䣺 %d  ====",m);
	
	 display_big_img( stackedTemp, "first_Matches" ); // ������
	 //	cvWaitKey(0);
	
	// 	cvWaitKey(0);
	//����� ƥ��logoһ�����ȥ����




 �����ʱ 0.013000kdtree��ʱ  0.207000
 
		display_big_img( stackedTemp, "first_Matches" ); // ������
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("�����ʱ %lf",duration);*/


	/*

	start = clock();//��ʼ��ʱ
	m2=0;
	kd_root = kdtree_build( feat1, n1 );//	kd_root = kdtree_build( feat2, n2 );
	storage = cvCreateMemStorage( 0 );
	testfeatseq=cvCreateSeq(0,sizeof(CvSeq),sizeof(MachedFeatPoint),storage);
	cvClearSeq(testfeatseq);
		tempstackedTemp=cvCloneImage(stacked);
	for( i = 0; i < n2; i++ )//ʹ��treeƥ�䣿 ��������㷨
	{
		feat = feat2 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 )
		{
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				pt2 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				pt1 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				pt2.y += img1->height;
				cvLine( tempstackedTemp, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//ƥ�䵽�ľͻ���
				m2++;
				feat2[i].fwd_match = nbrs[0];

				tempFeatPoint.machedPointFeat1=cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				tempFeatPoint.machedPointFeat2=cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				cvSeqPush(testfeatseq,&tempFeatPoint);//ѹ�� ƥ�䵽�� ͼƬ�ϵĵ�
			}
		}
		free( nbrs );
		nbrs=NULL;
	}	
	kdtree_release( kd_root );
	/*
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("kdtree��ʱ % lf",duration);
	*/

	/*
	printf(" kdtree ƥ�䣺 %d \n ",m2 );
	display_big_img( tempstackedTemp, "sencond_Matches" ); // ������
	cvWaitKey(0);
		/*		/*MachedFeatPoint ** feat3Points
			*feat3Points=calloc(m,sizeof(MachedFeatPoint));
			*feat3Points=cvCvtSeqToArray(featseq,*feat3Points,CV_WHOLE_SEQ);
		���� ��ת ���飬ֱ��seq ��ȥ����֮��Ҳseq����
			cvReleaseMemStorage( &storage );	
		*/


	cvReleaseMat(&resultMatrix);
	cvReleaseImage(&stackedTemp);
	stackedTemp=NULL;
	return m;
}


int compareFearture( struct feature* feat1, struct feature* feat2, 
	int n1,int n2,IplImage* img1,IplImage* stacked,CvPoint pointWindow1,CvPoint pointWindow2)
{//������������������ ��ƥ�����������ͼ
		//�޸ģ� �� feat1 ������ 
		 
	struct feature** nbrs;
	struct kd_node* kd_root;
	struct feature* feat;
	int  k, i, m = 0;
 	 int simoffearture=0;
	CvPoint pt1, pt2;
	double d0, d1;
	IplImage*stackedTemp;

	//stackedTemp=cvCreateImage(cvGetSize(stacked),stacked->depth,stacked->nChannels);
	stackedTemp=cvCloneImage(stacked);

	kd_root = kdtree_build( feat1, n1 );//	kd_root = kdtree_build( feat2, n2 );
	 
	cvRectangle(stackedTemp,cvPoint(pointWindow1.x,pointWindow1.y+img1->height),
		cvPoint(pointWindow2.x,pointWindow2.y+img1->height),CV_RGB(255,0,0),1,8,0);
	for( i = 0; i < n2; i++ )//ʹ��treeƥ�䣿 ��������㷨
	  {
		  feat = feat2 + i;
		  k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		  if( k == 2 )
		  {
			  d0 = descr_dist_sq( feat, nbrs[0] );
			  d1 = descr_dist_sq( feat, nbrs[1] );
			  if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			  {
				  pt2 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				  pt1 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				  pt2.y += img1->height;
				  cvLine( stackedTemp, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//ƥ�䵽�ľͻ���
				  m++;
				  feat2[i].fwd_match = nbrs[0];
			  }
		  }
		  free( nbrs );
		  nbrs=NULL;
	  }
	
	  kdtree_release( kd_root );

  	display_big_img( stackedTemp, "Matches" );
	//cvWaitKey( 0 );
	//Sleep(500);�����ʹ��showimge��Ч
	//cvWaitKey(50);
	cvReleaseImage(&stackedTemp);
	stackedTemp=NULL;

	return m;


}

int simOfFearture(int n1,int n2,int m,double huiduImag1,double  huiduImag2 )
{	//�ж�ƥ������ƶȣ� @=(AB)/max( A ,B )
	/*
		���A�Ǻܴ��ͼ��A�ͺܴ� B����С���ͼ�꣬���ԣ���ʹB�е�ȫ��ƥ�䵽��@Ҳ��С��
	*/
	int max;
	double temp;
	double temp_SIGOLIKELY;
	max=n1>n2 ? n1:n2;
	temp=(double)m/max;

	if (temp>0.1 && m> MINNUMBER_MACHING)
	{
		temp_SIGOLIKELY=SIGOLIKELY * ( 1.0 / ( fabs(huiduImag1- huiduImag2 ) + 1 ));	
		fprintf( stderr, " %3d:", m );	
		fprintf( stderr, "%.3lf ", temp );

		fprintf(stderr,"%.3lf",temp_SIGOLIKELY );

		//logSigma( m, temp , temp_SIGOLIKELY );

		
		if ( temp >  temp_SIGOLIKELY *BIG_SIGOLIKELY  )
		{
			logSigmaCompare( m, temp/temp_SIGOLIKELY  );
			return 2;//�������� ��ֱ������ ���ƥ�䣬���� ���ƥ�� ������ ����logoƥ��
		}
		else if ( temp > temp_SIGOLIKELY ){
			logSigmaCompare( m, temp/temp_SIGOLIKELY  );
			return 1;//һ�� ������ ��ֱ������ ���ƥ�䣬���� ���ƥ�� ��
		}
	}
	return 0;
}