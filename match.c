/*
原版： logo多放小， 计算每个的特征点
图片计算一个大小的特征点，
然后一一匹配
窗口由每次logo的大小决定
缩放取1.3效果远远好于2

添加时间的记录

调整sigma

同一个图，不同颜色也不行，（灰度值不同）

*/


#include "mach.h"

//extern  int find_Drawcontours (IplImage *src,IplImage *dst);

extern const int NUMBEROF_CENG;//logo要多少-1 层
extern const double NUMBERFOR_RESIZE;//每层缩小几倍
extern const  int  SLIPER;//一行 滑动次数 =logo的10分之一
extern const int MIN_SLIPSIZE;  //最小滑动
extern const int SMALLIMAG_SIZE;//最小logo的大小
extern const double SIGOLIKELY;//相似度的阀值
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
{//释放feat
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
{//复制线程参数
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
{//直接计算所有的logo 的feat

	/* 建立线程，申请feat的空间
	结构体传参数
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

	start = clock();//开始计时
	
	realchipNumber =chipNumber< numberOfLogo? chipNumber: numberOfLogo;

	tempForThread_men=(	struct tempStructForThread_LogoCaculate *)malloc(
		sizeof(	struct tempStructForThread_LogoCaculate )*realchipNumber);
	handle=(HANDLE *) malloc(sizeof(HANDLE)*realchipNumber);

	*logofeat=(LogoFeatStruct **)malloc(sizeof(LogoFeatStruct *) *numberOfLogo );
	
	//为size分配空间
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
	//将feat转为矩阵
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
	printf("耗时：%.2lf 秒",duration);
//	system("pause");
	return;
}

unsigned int __stdcall MyThreadFun_LogoCaculate(PVOID pM)  
{  //多线程函数
	char srcLogoPicturePath[_MAX_PATH];

	IplImage* img1 ;
	IplImage** tempForLogoImg;

	struct feature* feat1 ;

	int n1, k, i, m = 0;
	int endCeng=0;

	int endi=0;
	int iForPicture;
	int logoNumber;//第几个logo
	int starti;

	//传参数
	int threadNumber;
	int numberOfLogo;
	srcIndexStruct *logo_srcIndex;
	LogoFeatStruct *** logofeat;
	int NUMBEROF_CENG;
	CvSize **logoSizeIndex;

	numberOfLogo=((struct tempStructForThread_LogoCaculate*)pM )->numberOfLogo;//logo总数
	threadNumber=((struct tempStructForThread_LogoCaculate*)pM )->threadNumber;//第几个线程
	logo_srcIndex=((struct tempStructForThread_LogoCaculate*)pM )->logo_srcIndex;//logo地址数组
	logofeat=((struct tempStructForThread_LogoCaculate*)pM )->logofeat;//最后的特征点 和个数的数组
	NUMBEROF_CENG=((struct tempStructForThread_LogoCaculate*)pM )->NUMBEROF_CENG;
	logoSizeIndex=((struct tempStructForThread_LogoCaculate*)pM )->logoSizeIndex;
	if (numberOfLogo /chipNumber==0)
	{//chip 》 logo数目
		starti=threadNumber;//直接以线程序号为图序号
		endi=threadNumber+1;
	}
	else
	{
		starti=numberOfLogo /chipNumber *threadNumber;
		//表示第几个线程
		if (threadNumber==chipNumber-1)//最后一个线程
		{
			endi=numberOfLogo;
		}
		else
		{
			endi=numberOfLogo /chipNumber *(threadNumber+1);
		}
	}


	for(iForPicture = starti; iForPicture< endi ;iForPicture++)
	{//线程的每一张图

		logoNumber=iForPicture;//logo编号 从0开始吧
		strcpy(srcLogoPicturePath,logo_srcIndex[logoNumber].srcIndex);	

		(*logofeat)[logoNumber]=(LogoFeatStruct *)malloc(sizeof(LogoFeatStruct ) *NUMBEROF_CENG );

		img1 = cvLoadImage( srcLogoPicturePath, 1 );	
		if (img1 ==NULL)
		{
			printf("没有对应图片 %s",srcLogoPicturePath);
			continue ;//可能没有图片
		}

		endCeng=0;


		tempForLogoImg=(IplImage**)malloc(sizeof(IplImage*)*NUMBEROF_CENG);
		//对原图  先多次缩小，再算feat
		for (i=0;i<NUMBEROF_CENG   ;i++)
		{//缩放
			if (i ==0)
			{   
				tempForLogoImg[i]=cvCloneImage(img1);
			}  
			if(i!=0)
				tempForLogoImg[i]= smallSizeImg( tempForLogoImg[i-1],NUMBERFOR_RESIZE);

			endCeng=i;
			
			//添加size的矩阵
			(logoSizeIndex)[iForPicture][i].width=tempForLogoImg[i]->width;
			(logoSizeIndex)[iForPicture][i].height=tempForLogoImg[i]->height;

			if (isTooSmall( tempForLogoImg[i]))
			{//泄露了

				for (;i< NUMBEROF_CENG ;i++)
				{
					(*logofeat)[logoNumber][i].feat1=NULL;
					(*logofeat)[logoNumber][i].numberOffeat=0;
					
					//添加size的矩阵
					(logoSizeIndex)[iForPicture][i].width=0;
					(logoSizeIndex)[iForPicture][i].height=0;
				}
				break;//logo 太小了
			}


			//特征点检测
			fprintf( stderr, "Finding features in %s ----%d ...\n", srcLogoPicturePath,i );
			n1 = sift_features(  tempForLogoImg[i], &feat1 );
			fprintf( stderr, " features %d\n", n1 );
			//(*logofeat)[j][i]= (LogoFeatStruct *)malloc(sizeof(LogoFeatStruct));
			(*logofeat)[logoNumber][i].feat1=feat1;
			(*logofeat)[logoNumber][i].numberOffeat=n1;
		}//每一层

		for (i=0;i<=endCeng ;i++)
		{
			cvReleaseImage(&tempForLogoImg[i]);
		}


		cvReleaseImage(&img1);
	}//每一个图
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
	//线程参数
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


	char srcTestPicturePath[_MAX_PATH];//最后读取的地址
	char srcLogoPicturePath[_MAX_PATH];
	IplImage* img2;
	struct feature  * feat2;
	int n2;

	char  tempNameForPictureChange[_MAX_PATH];
	char *nameForPictureChange="_Mach_";
	char *nameForPictureSub="子图_";
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

	CvRect resultRect;//匹配的区域返回结果

	//线程参数传递
	Picture_srcIndex=( ( struct tempStructForThread_EachPicture*)pM )-> 	Picture_srcIndex;
	logo_srcIndex=( ( struct tempStructForThread_EachPicture*)pM )->  logo_srcIndex;
	numberOfPicture=( ( struct tempStructForThread_EachPicture*)pM )->  numberOfPicture;
	numberOfLogo=( ( struct tempStructForThread_EachPicture*)pM )->   numberOfLogo;
	logofeat=( ( struct tempStructForThread_EachPicture*)pM )->  logofeat;
	

	//索引参数
	//LogoMatrix=( ( struct tempStructForThread_EachPicture*)pM )->  LogoMatrix;
	AvgVector=( ( struct tempStructForThread_EachPicture*)pM )->  AvgVector;
	EigenVector=( ( struct tempStructForThread_EachPicture*)pM )->  EigenVector;
	BigIndex=( ( struct tempStructForThread_EachPicture*)pM )->  BigIndex;
	featseq_ForBigClass=( ( struct tempStructForThread_EachPicture*)pM )->  featseq_ForBigClass;
	logoSizeIndex=( ( struct tempStructForThread_EachPicture*)pM )->  logoSizeIndex;

	threadNumber= ( ( struct tempStructForThread_EachPicture*)pM )->threadNumber;

	//如果图片数小于线程数 那么线程开始就min（图数目，chipernumber）
	if (numberOfPicture /chipNumber==0)
	{
		stari=threadNumber;//直接以线程序号为图序号
		endi=threadNumber+1;
	}
	else
	{
		stari=numberOfPicture /chipNumber *threadNumber;

		 //表示第几个线程
		if (threadNumber==chipNumber-1)//最后一个线程
		{
			endi=numberOfPicture;
		}
		else
		{
			endi=numberOfPicture /chipNumber *(threadNumber+1);
		}
	}

	for(iForPicture = stari; iForPicture< endi ;iForPicture++)
	{//对每一个测试图

//第一步：得到测试图，分块并计算特征点
		//	logoNumber+=i;
		tempPictureNumber=iForPicture;
		tagForMatched=0;//标记有logo


		//保存时间
		//	saveTime(j);

		strcpy(srcTestPicturePath,Picture_srcIndex[tempPictureNumber].srcIndex);

		/* 图片载入 */
		img2 = cvLoadImage( srcTestPicturePath, 1 );
		//必须判断读入的东西
		if (img2 ==NULL)
		{
			printf("没有对应图片 %s",srcTestPicturePath);
			continue ;//可能没有图片
		}

		numberOfpices= myDivedeImg(&imgForPicture, img2);

		cvReleaseImage( &img2 );
		for( k=0;k< numberOfpices  &&tagForMatched==0; k++)
		{	
			if (isTooSmall(imgForPicture[k]) )
			{
				continue;//分图后可能很小
			}

			fprintf( stderr, "Finding features in %s...\n", srcTestPicturePath );
			n2 = sift_features( imgForPicture[k], &feat2 );//取得特征点
			fprintf( stderr, " features %d\n", n2 );


			if ( n2 >MINNUMBER_FORFEAT)
			{//特征点太少就不要匹配了
				draw_features( imgForPicture[k],feat2,n2);
		//匹配第一步： feat到矩阵
			//这里使用矩阵转换
				PictureMatrix=cvCreateMat(n2,NUMBERFORDIVISIOM,CV_64FC1);
				featToMatrixWithoutTranslate(PictureMatrix,feat2,n2);//feat到矩阵,不要转置，

/*//全部重写： 直接 pca、匹配 矩阵，然后，再匹配第二层矩阵。
AvgVector= 
EigenVector=
				BigIndex= 
				featseq_ForBigClass

	第一步： 图片的特征点 转矩阵 ，  pca降维
	第二步：pca矩阵和bigindex 相乘，
	for 每一行{
	求最小角度 的索引i
	{
	第三步：seq得到i个元素， 获取，cvmat 和tag数组
	第四部：原矩阵和cvmat128维相乘，
	得到最近的两个点， 加入seq中。

	}
	取前最大的3个seq，
	每个得到对应的 featlogo 数组
			开始用函数计算
*/

//第一步： 图片的特征点 转矩阵 ，  pca降维
			//这里使用feat->矩阵转换
			//矩阵 -》pca
				PictureMatrix_PCA=cvCreateMat(PictureMatrix->rows,BigIndex->cols,PictureMatrix->type);
					
				
				//cvReleaseMat(&PictureMatrix_PCA);//快速释放  降维前可以释放

				cvProjectPCA(PictureMatrix,AvgVector,EigenVector,PictureMatrix_PCA);//完成降维
				//cvReleaseMat(&PictureMatrix_PCA);//快速释放  可以释放》？？？
				//降维后，再转置， 除mo


				//测试使用
				PrintMatrix(PictureMatrix);		
				PrintMatrix(PictureMatrix_PCA);


				PictureMatrix_PCA_MO_T=cvCreateMat(PictureMatrix_PCA->cols,PictureMatrix_PCA->rows,PictureMatrix_PCA->type);
				MatrixTranslateAndMO(PictureMatrix_PCA,PictureMatrix_PCA_MO_T);
				cvReleaseMat(&PictureMatrix_PCA);//快速释放 降维后不可以释放》？？？

				PrintMatrix(PictureMatrix_PCA_MO_T);


		//匹配第二步：pca矩阵和bigindex 相乘，
				resultPCAMatrix=cvCreateMat(BigIndex->rows,PictureMatrix_PCA_MO_T->cols,BigIndex->type);

				cvMatMul(BigIndex,PictureMatrix_PCA_MO_T,resultPCAMatrix);

		//匹配第三步：分析logoIndex 和p的矩阵乘积， 再次匹配smalllclasss 得到精确匹配。 
				//最终得到是否匹配结果
				//？？？？？？？？？？？？？？？必须传入 img2，logo的size数组
				

				//修改 ：在传入前，先求 模 
				caculateMO(PictureMatrix);

				//测试
				PrintMatrix(PictureMatrix);

				tagForMatched=myMachALLL(resultPCAMatrix,featseq_ForBigClass,	
					logofeat , numberOfLogo,
					feat2,PictureMatrix
					,imgForPicture[k],
					logo_srcIndex,logoSizeIndex,
					&resultRect,&numberOfMachedLogo);//返回是否匹配到了
	
				
				cvReleaseMat(&PictureMatrix_PCA_MO_T);
				cvReleaseMat(&resultPCAMatrix);

			}
			
			if (tagForMatched)
				{		//每次匹配后 tagForMakeIt都会被更新，所以用tagForMatched 来记忆是否匹配
					//	tagForMatchedLogo=i;
					 WaitForSingleObject(mutex, INFINITE);  
					//需要同步处理
						 SaveResult(tempPictureNumber ,k ,numberOfMachedLogo);
					
						//匹配到了，保存图片
						myDrawRectToImage(imgForPicture[k],resultRect);
				
						myMachedPictureFilePath(srcTestPicturePath,dstTestPicturePath,
							k,numberOfMachedLogo);
						//要新建文件夹：保存
						myCreateFatherFile(dstTestPicturePath);	//得到具体保存的路径
						cvSaveImage(dstTestPicturePath,imgForPicture[k],0);//保存图片
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
	 }	//每一份图结束


		for( k=0;k< numberOfpices; k++)
		{	
			cvReleaseImage(  &imgForPicture[k]);
		}
		free(imgForPicture); imgForPicture=NULL;

	
		if(tagForMatched)
		{
			WaitForSingleObject(mutex, INFINITE);  
		//	 SaveResult(tempPictureNumber ,0 ,tagForMatchedLogo);
			//需要同步处理
			mySqlInsertResult(Picture_srcIndex[tempPictureNumber].id,0);//logoid无用了
			//	logo_srcIndex[i].id);//这是子结果

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
	int *numberOfMachedLogo/*用于结果处理,匹配到了哪个logo*/

	)
{/* 传入bigindex矩阵
 for 每一列{
 求最小角度《cos阈值   的行坐标=》就是 匹配的大类k
 {
  建立所有logo类的280个seqcolitionfeat【】数组，
	 第三步：seq得到k个元素， 获取，cvmat 和tag数组
	 
	 mymatchsmalllclasss（）
	 第四部：
	 原矩阵和cvmat128维相乘，
	 得到最近的两个点， 加入对应tag标记的featseq中。
 }
 取前最大的3个seq，
 每个得到对应的 featlogo 数组
 开始用函数计算
 
  返回：是否匹配到，   匹配到的rect
 */
	int i,j,k;
	int maxrows;
	double max1,max2,tempmax;

	struct LogoTagAndFeatTag* logotag;
	int cheng,logolei,featnumber;
	int maxrowInSmall;
	
	struct LogoTagAndFeatTag*index;//保存seq 按total排序后的结果

	struct feature* feat;
	struct feature* FeatforPictrue;
	struct feature* FeatForLogo;

	MachedFeatPoint *tempFeatPoint;

	CvSeq ***SeqForeachLogoCeng;
	CvSeq *Seqtemp;
	CvMemStorage** *memstorage;

	int tagForMakeIt;

	/*初始条件：为每一个logo的每一层，建立seq表：	*/
	buildMycvSeqForeachLogoCeng(&SeqForeachLogoCeng,&memstorage, numberOfLogo);

	for (i=0 ;i<resultPCAMatrix->cols  ;i++)
	{//每一列
		maxrows=getOneMax(resultPCAMatrix, i,&max1);

		//maxrows+>是行数，对应 大类的序号， i是列，对应图片的第i个特征点
		if (max1 >MINCOS_PCA)
		{//是一个小类的
			logotag=myMachSmallClass(featseq_ForBigClass,maxrows,PictureMatrix,i);//返回 匹配到的logo对应的tag值

			if ( logotag  != NULL)
			{			
			//取得logofeat【tag】 然后取得picture的【i】个 ，加入seq中
			//分解出层和logo的类 对应feat数组

				logolei=logotag->tagForClass;
				cheng=logotag->tagForCeng;
				featnumber=logotag->tagForFeatNumber;
				feat=logofeat[logolei][cheng].feat1;
				FeatForLogo=feat+featnumber;//定位logo的feat

				FeatforPictrue=feat2+i;
				tempFeatPoint=featToMachedFeatPoint(FeatForLogo,FeatforPictrue);

				//cvSeqPush(featseq,&tempFeatPoint);//压入 匹配到的 图片上的点
				/**下一步：建立seq数组
				一个个加入对应的seq
				对前三个seq计算sim值	*/

				cvSeqPush(SeqForeachLogoCeng[logolei][cheng],tempFeatPoint);
			}
		}
	}

	 //取出一对多
	reduceMachedPointscvSeqForeachLogoCeng(SeqForeachLogoCeng, numberOfLogo);
	
	
	//还是直接 排序，  取最大的前n个seq，扫描sim值。
	mysortTotalSeq(SeqForeachLogoCeng, numberOfLogo,NUMBEROF_CENG, &index );
	tagForMakeIt=0;
	for (k=0; k<5 && tagForMakeIt ==0 ;k++)
	{//只判断前N个？
		if(index[k].tagForFeatNumber >5)//至少大于5个点
		{//获得最大total的 类，层
			i= (index[k].tagForClass);
			j=(index[k].tagForCeng);

			Seqtemp=SeqForeachLogoCeng[ i][ j];
			

			//???????????????????????????????????????演示：
		//	重新读入logo ，计算层次，然后stack

			//这里对每一个seq的计算sim值
			tagForMakeIt= myLastMached_seqFeat_Fea2_Feat1( logo_srcIndex,
				i,j,
				logoSizeIndex[i][j], img2, 
				logofeat[i][j].feat1, feat2, 
				Seqtemp,  
				logofeat[i][j].numberOffeat,PictureMatrix->rows, 	Seqtemp->total
				,resultRect);
			//过程： 每一个seq的点， 定位窗口大小， 算sim， 判断sim

			if (tagForMakeIt)
			{
				*numberOfMachedLogo=i;
				break;
			}
		//匹配到了就break；
		}
	}

	//释放内存  还有index的没。。。。
	releaseMycvSeqForeachLogoCeng(&SeqForeachLogoCeng,&memstorage, numberOfLogo);

	return tagForMakeIt;
}

/*
int machEachPicture(int numberForLogo,LogoFeatStruct ** logofeat,CvMat** tempMatrix,
	char* srcLogoPicturePath,IplImage * img2_orig, 
struct feature  * feat2,CvMat *PictureMatrix,int n2,char *dstTestPicturePath)
{//一个logo的不同层 对测试图的一个pice  匹配



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

 // struct feature  *feat2 = (struct feature  *)calloc( n2, sizeof(struct feature) );//将feat3 拷贝一份，然后可以修改
  struct feature  * feat3;
  int  machedPointsnumber;

  int numberForFeat;
  //用于匹配到的点的储存
  CvSeq *featseq;
  CvMemStorage* storage ;

  CvMat* tempMatrixForLogo;

  storage = cvCreateMemStorage( 0 );
  featseq=cvCreateSeq(0,sizeof(CvSeq),sizeof(MachedFeatPoint),storage);


  //可以很好地优化 内存空间  只用一个来做
  img1 = cvLoadImage( srcLogoPicturePath, 1 );

  tagMat=cvCreateMat(img2->height,img2->width,CV_8SC1);//标记矩阵

  cvZero(tagMat);

  tempForLogoImg=(IplImage**)malloc(sizeof(IplImage*)*NUMBEROF_CENG);

  //对原图  先多次缩小，再算feat
  for (i=0;i<NUMBEROF_CENG   && tagForMakeIt ==0 ;i++)
  {//缩放
	  if (i ==0)
	  {   
		  tempForLogoImg[i]=cvCloneImage(img1);
	  }  
	  if(i!=0)
	  tempForLogoImg[i]= smallSizeImg( tempForLogoImg[i-1],NUMBERFOR_RESIZE);
 
	  if (Img1BigImg2( tempForLogoImg[i],img2))
	  {
		  startCeng=i+1;
		  continue;//logo>图
	  }
	  endCeng=i;
	  if (isTooSmall( tempForLogoImg[i]))
	  {
		  endCeng=i-1;
		  break;//logo 太小了
	  }


		  {//特征点检测
			 numberForFeat=numberForLogo ;
			 n1 = logofeat[numberForFeat][i].numberOffeat;
			 feat1=logofeat[numberForFeat][i].feat1;
			 tempMatrixForLogo=tempMatrix[i];//得到单个层的矩阵

			 stacked = stack_imgs(  tempForLogoImg[i], img2 );
			 //一起显示的面板（两个的高度和）将特征点也画上

			  if ( n1 > MINNUMBER_FORFEAT)
			  {			  				  
				  cvClearSeq(featseq);
				  //预先匹配
				  machedPointsnumber=compareFeartureFirstTime(feat1,tempMatrixForLogo,feat2,PictureMatrix,n1,n2, tempForLogoImg[i],stacked,featseq);

				  //提前判断 feat 》 阀值
				  if (machedPointsnumber > ( n1 *SIGOLIKELY )/3 )
				  {
					//滑动窗口比较
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
 
  //显示tagmat  CvMat 转img
  tagImag=cvCreateImageHeader(cvGetSize(tagMat),IPL_DEPTH_8U ,1);
  cvGetImage(tagMat,tagImag);
  tagImag->depth=8;
  fprintf(stderr," %d \n", tagImag->depth);

  cvThreshold(tagImag,tagImag,0.5,254,CV_THRESH_BINARY); 
  find_Drawcontours(tagImag,img2);

  if (tagForMakeIt)
  {//直接分析 dstTestPicturePath的 上层目录

	  myCreateFatherFile(dstTestPicturePath);
	//要新建文件夹： 保存
	cvSaveImage(dstTestPicturePath,img2,0);//保存图片
  }
  cvReleaseImageHeader(&tagImag);//cvGetImage() 中 只是将mat 的数据地址给了 img，img只要一个头局够了，所以不能释放img，只能释放头
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
{//在pointWindow1 pointWindow2之间的矩形找到所有特征点，并一一匹配
	int k,j;	
	int muberOfpoint=0;	
	CvSeq* features2,* features3;
	CvMemStorage* storage2,* storage3;
	int simoffearture=0;
	
	struct feature* feat  ,*tempfeat;
	int cengForImag1,cengForImag2;



	MachedFeatPoint *machedstruct,*machedstructtemp;
	double lingduImg2;
	int numberOfMached;//匹配到的点
	muberOfpoint=0;//测试图片在窗口内的点数目

	storage3 = cvCreateMemStorage( 0 );//初始化seq
	features3 = cvCreateSeq( 0, sizeof(CvSeq), sizeof(MachedFeatPoint), storage3 );
 
	/*
	storage2 = cvCreateMemStorage( 0 );//初始化seq
	features2 = cvCreateSeq( 0, sizeof(CvSeq), sizeof(struct feature), storage2 );
	cvSeqPush( features2, feat )	;
*/

	for (k=0,feat=feat2;k<n2;k++ )
	{//所有窗口内点匹配
		feat=feat2+k;
		if (feat->x >=pointWindow1.x && feat->y> pointWindow1.y && 
			feat->x <=pointWindow2.x  && feat->y <  pointWindow2.y  )
		{		
			muberOfpoint++;
		}					
	}


	numberOfMached=0;
	for (k=0;k<n3;k++ )
	{//所有窗口内点匹配
		machedstruct=(MachedFeatPoint *)cvGetSeqElem(featseq,k);
		if (compareIsInWindow(machedstruct,pointWindow1,pointWindow2) )
		{
			cvSeqPush( features3, machedstruct );
			numberOfMached++;
		}					
	}


	//图片一个区域内 只能是单个点匹配单个logo点， 多个图片点匹配一个logo点就是错误的，
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


	if (muberOfpoint  > 4)//窗口内点太少，就直接跳过
	{

	//这里修改： 先计算区域图像亮度， 然后计算 sim值
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
	//千万千万要记住， 提前退出一定要 释放


	return 0;
}

int myLastMached_seqFeat_Fea2_Feat1( srcIndexStruct *logo_srcIndex,
	int lei,int ceng,//到时候可以不使用
	CvSize logoSize,IplImage* img2, 
	struct feature*feat1,struct feature* feat2, 
	CvSeq *featseq, 
	int n1,int n2,int n3
	,CvRect *resultRect)
{/*
 1.通过 srcindex 读取logo图片：测试用， 得到对应的层次logo的图像
 2.压站到stack局部变量
 
 3.正式匹配：调用sliperwindow（）
 过程： 每一个seq的点， 定位窗口大小， 算sim， 判断sim

 返回：是否匹配到，   匹配到的rect
 */
	int i,j;
	IplImage*img1;
	IplImage* tempForLogoImg;
	IplImage* stacked;



	int tagForCompare=0;	



	img1=cvLoadImage(logo_srcIndex[lei].srcIndex,1);

	tempForLogoImg=caculateSmallSizeImg( img1 ,NUMBERFOR_RESIZE,ceng);//缩小到对应的层的logo
	cvReleaseImage(&img1);

	stacked = stack_imgs( tempForLogoImg, img2 );

	tagForCompare=judgMachedFromImgaToImage(tempForLogoImg,img2,stacked,
	feat1, feat2,featseq,
	n1, n2, n3, 
	resultRect);//匹配结果
		//保存结果  ：直接返回一个rect，然后，画在img2上，保存图片。

	cvReleaseImage(&stacked);
	return tagForCompare;
}

int judgMachedFromImgaToImage(IplImage*img1,IplImage*img2,IplImage*stacked,
struct feature* feat1,struct feature * feat2,CvSeq *featseq,
	int n1,int n2,int n3,
	CvRect *resultRect)
{/*从两个图 的特征点 判断图片是否匹配
	返回imge2的匹配区域reck。由调用函数 来绘制矩形，并保存
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

	/*可以优化的地方：
	 得到匹配点的最值，然后得到maxmin  的窗口点*/
	//用logo窗口 滑动来匹配点
	liangduImg1=ComputeLiangdu(img1,cvPoint(0,0),sizeImg1);
		
	tagForCompare=0;

	/*用每一个匹配到的点  ，确定 图片logo的区域， 再比较 sim*/

	for(i=0;i<n3  && tagForCompare ==0 ;i++)
	{
		featTemp =(MachedFeatPoint*) cvGetSeqElem(featseq,i);
		
		GetMachedWindowPoint( featTemp, &pointWindow1, &pointWindow2, sizeImg1 ,sizeImg2);
	
		tagForCompare=judgeFormPiont( feat1,  feat2, featseq, n1,  n2,n3, img1,liangduImg1,img2, stacked , pointWindow1,  pointWindow2 ) ;
		if(tagForCompare )
		{//匹配的img2 的区域
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
{   //计算两点矩阵的特征点 的匹配个数。绘制图
	//不再返回特征点，而是返回匹配点的坐标 +logo 对应的坐标

	/*在这里，使用矩阵 相乘 得到两个最小点， 阈值，  加入seq中
		1。矩阵相乘，
		2。求每一列最小两个值，
		3.判别，》阈值，就是了，
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
	//测试用
	CvSeq *testfeatseq;
	CvMemStorage* storage ;
	IplImage* tempstackedTemp;
	*/


		clock_t start, finish;
		double duration;
	start = clock();//开始计时


	stackedTemp=cvCloneImage(stacked);

	resultMatrix=cvCreateMat(tempMatrixForLogo->rows,PictureMatrix->cols,tempMatrixForLogo->type);
 
	cvMatMul(tempMatrixForLogo,PictureMatrix,resultMatrix);


	//printMyMat(resultMatrix);
	//查询矩阵，返回 第一第二大的数组，
	for (nj=0;nj<PictureMatrix->cols ;nj++)
	{
		//特征点产生的时候就过滤了少于5个点的。
		max1= (double )	CV_MAT_ELEM(*resultMatrix,double,0,nj);
		max2= (double )	CV_MAT_ELEM(*resultMatrix,double,1,nj);
		tempmi=0;
		if (max1 < max2)
		{
			tempmax=max1;
			max1=max2;
			max2=tempmax;
			tempmi=1;//标记最大值的index
		}	
		
		for(mi=2;mi< tempMatrixForLogo->rows ;mi++)
		{//求得一列最小两个值  cos应该是最大值啊
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
		//判断比值 》阈值？
		if (  (max1 > YUZHI_COS_MAX )  || 
			(  ( max1 > YUZHI_COS_MIN ) &&  ( ((1-max1) < (1- max2)* YUZHI_COS  )  ))
			 ) 
		{ //

			tempFeat1=feat1+tempmi;
			tempFeat2=feat2+nj;
			pt1	 = cvPoint( cvRound( tempFeat1->x ), cvRound( tempFeat1->y ) );
			pt2 = cvPoint( cvRound(tempFeat2->x ), cvRound(tempFeat2->y ) );
			pt2.y += img1->height;
			cvLine( stackedTemp, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//匹配到的就划线
			m++;
		
			tempFeatPoint.machedPointFeat1=cvPoint( cvRound( tempFeat1->x ), cvRound(tempFeat1->y ) );
			tempFeatPoint.machedPointFeat2=cvPoint( cvRound( tempFeat2->x ), cvRound(tempFeat2->y ) );
			cvSeqPush(featseq,&tempFeatPoint);//压入 匹配到的 图片上的点
		}

	}
    reduceMachedPoints(featseq,&m);
	/*
	if ( m>5)
	{	
	 	myDrawPoint(featseq, m, stacked, img1->height);
	 	printf(" 矩阵匹配： %d  ====",m);

	}*/

	// display_big_img( stackedTemp, "first_Matches" ); // 测试用
//	 cvWaitKey(0);  特变主意； opencv的waitkek会阻止多线程的结束。
	// myDrawPoint(featseq, m, stacked, img1->height);
	/*
	printf(" 矩阵匹配： %d  ====",m);
	
	 display_big_img( stackedTemp, "first_Matches" ); // 测试用
	 //	cvWaitKey(0);
	
	// 	cvWaitKey(0);
	//多个点 匹配logo一个点的去除：




 矩阵耗时 0.013000kdtree耗时  0.207000
 
		display_big_img( stackedTemp, "first_Matches" ); // 测试用
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("矩阵耗时 %lf",duration);*/


	/*

	start = clock();//开始计时
	m2=0;
	kd_root = kdtree_build( feat1, n1 );//	kd_root = kdtree_build( feat2, n2 );
	storage = cvCreateMemStorage( 0 );
	testfeatseq=cvCreateSeq(0,sizeof(CvSeq),sizeof(MachedFeatPoint),storage);
	cvClearSeq(testfeatseq);
		tempstackedTemp=cvCloneImage(stacked);
	for( i = 0; i < n2; i++ )//使用tree匹配？ 具体过程算法
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
				cvLine( tempstackedTemp, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//匹配到的就划线
				m2++;
				feat2[i].fwd_match = nbrs[0];

				tempFeatPoint.machedPointFeat1=cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				tempFeatPoint.machedPointFeat2=cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				cvSeqPush(testfeatseq,&tempFeatPoint);//压入 匹配到的 图片上的点
			}
		}
		free( nbrs );
		nbrs=NULL;
	}	
	kdtree_release( kd_root );
	/*
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("kdtree耗时 % lf",duration);
	*/

	/*
	printf(" kdtree 匹配： %d \n ",m2 );
	display_big_img( tempstackedTemp, "sencond_Matches" ); // 测试用
	cvWaitKey(0);
		/*		/*MachedFeatPoint ** feat3Points
			*feat3Points=calloc(m,sizeof(MachedFeatPoint));
			*feat3Points=cvCvtSeqToArray(featseq,*feat3Points,CV_WHOLE_SEQ);
		这里 不转 数组，直接seq 出去，让之后也seq处理
			cvReleaseMemStorage( &storage );	
		*/


	cvReleaseMat(&resultMatrix);
	cvReleaseImage(&stackedTemp);
	stackedTemp=NULL;
	return m;
}


int compareFearture( struct feature* feat1, struct feature* feat2, 
	int n1,int n2,IplImage* img1,IplImage* stacked,CvPoint pointWindow1,CvPoint pointWindow2)
{//计算两点矩阵的特征点 的匹配个数。绘制图
		//修改， 用 feat1 建立树 
		 
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
	for( i = 0; i < n2; i++ )//使用tree匹配？ 具体过程算法
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
				  cvLine( stackedTemp, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//匹配到的就划线
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
	//Sleep(500);这个会使得showimge无效
	//cvWaitKey(50);
	cvReleaseImage(&stackedTemp);
	stackedTemp=NULL;

	return m;


}

int simOfFearture(int n1,int n2,int m,double huiduImag1,double  huiduImag2 )
{	//判断匹配的相似度： @=(AB)/max( A ,B )
	/*
		如果A是很大的图，A就很大， B是缩小后的图标，所以，即使B中点全部匹配到了@也很小。
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
			return 2;//很相似了 ，直接跳出 点的匹配，跳出 层的匹配 ，跳出 其他logo匹配
		}
		else if ( temp > temp_SIGOLIKELY ){
			logSigmaCompare( m, temp/temp_SIGOLIKELY  );
			return 1;//一般 相似了 ，直接跳出 点的匹配，跳出 层的匹配 。
		}
	}
	return 0;
}