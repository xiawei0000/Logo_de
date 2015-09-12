
#include "mysql_main.h"
#include "mach.h"
//extern  int find_Drawcontours (IplImage *src,IplImage *dst);
extern const int NUMBEROF_CENG;//logo要多少-1 层

extern  const int MINNUMBER_MACHING;
extern const int MINNUMBER_FORFEAT;
 
extern const char nameForPictureChange[50];
extern  int numberForMachedPicture;
extern const int  chipNumber;
extern 	HANDLE  mutex;
extern 	HANDLE  FilePathmutex;
int main( int argc, char** argv )
{

	//多线程参数
	HANDLE * handle; 
	struct tempStructForThread_EachPicture *tempForThread_EachPicturemen;

	//数据库：参数
		//传入的
	int idProgect;
	int numberOfLogo;//numberOfRows;
	srcIndexStruct *logo_srcIndex;//logo的地址+id 表

	int numberOfPicture;//numberOfRows;
	srcIndexStruct *Picture_srcIndex;// 的地址+id 表


	//pca 参数

	CvMat *AvgVector=NULL;
	CvMat *EigenVector=NULL;
	CvMat  *BigIndex=NULL;
	CvSeq *featseq_ForBigClass;//存大类， 索引
	CvMemStorage* storage_ForBigClass ;


	char filePath[300]="D:\\360\\Workspace\\C++\\LAB_work\\babaoli_pictureLab\\NEW_08\\";
	//char filePath[300];
	char LogoFilepath[MAX_PATH];
	//char srcTestPath[300];


	clock_t start, finish;
	double duration;

	int i,j=1,k;

	int numberOfTestPicture;
	int tempLeng;


	CvSize **logoSizeIndex;//保存size矩阵
	LogoFeatStruct ** logofeat;//用于保存一次计算的logofeat
	CvMat  ***LogoMatrix;
	int realchipNumber;


	start = clock();//开始计时


	mutex  = CreateMutex(NULL, FALSE, NULL);  
	FilePathmutex= CreateMutex(NULL, FALSE, NULL);  


	//参数处理

	idProgect=atoi(argv[1]);
	mySqlLogoSrc( idProgect,&numberOfLogo,&logo_srcIndex);//查好logo数据
	//numberOfLogo==0 DE CHULI=================================================================	===========================
	//测试
//	myCreateFatherFile(logo_srcIndex[1].srcIndex);
	//测试
//	numberOfLogo=3;


	ClearFile( filePath);
	saveTime(0);
		/**/
	//这里必须换成矩阵， 返回
	CaculateLogoFeatFirst(  logo_srcIndex, &logofeat, numberOfLogo, NUMBEROF_CENG,&logoSizeIndex);//,&LogoMatrix );
	//?????????????????????这里必须先得到size矩阵返回。
//测试
	numberOfLogo=1;

	//建立所有的logo索引
	//logo必须保存在同一个文件夹
	//获得logo文件夹
	getFatherPath(LogoFilepath,logo_srcIndex[0].srcIndex);
	storage_ForBigClass = cvCreateMemStorage( 0 );	
	myCacultatePCA_new(  logofeat,  numberOfLogo,  NUMBEROF_CENG ,
		&AvgVector,&EigenVector,&BigIndex,&featseq_ForBigClass,storage_ForBigClass,LogoFilepath);


	

	//第二部：查数据库的picture
	numberForMachedPicture=0;
	mySqlPictureSrc( idProgect,&numberOfPicture,&Picture_srcIndex);//查好 picture数据
	//查询速度远远快于插入速度
	numberForMachedPicture=4;

	//测试：numberOfPicture
//	numberOfPicture=3;

	//最后： 用picture数组来匹配
	realchipNumber =chipNumber< numberOfPicture? chipNumber: numberOfPicture;

	//多线程参数：
	handle=(HANDLE *) malloc(sizeof(HANDLE)*realchipNumber);
	tempForThread_EachPicturemen=(struct tempStructForThread_EachPicture *)
		malloc(sizeof(struct tempStructForThread_EachPicture)*realchipNumber);

	realchipNumber=1;
	for(j=0;j<realchipNumber;j++)
	{	//失败：因为多线程时， kdtreee会修改feat的data，所以多个线程同时用的时候，就乱了。
		init_tempStructForThread_EachPicture(
			&tempForThread_EachPicturemen[j],
			Picture_srcIndex,
			 logo_srcIndex,
			numberOfPicture,
			 numberOfLogo,
			 logofeat,
	//		 LogoMatrix,替换诶pca矩阵 索引
			 AvgVector,EigenVector,
			 BigIndex,
			 featseq_ForBigClass,
			 logoSizeIndex,
			 j);
		handle[j] = (HANDLE)_beginthreadex(NULL, 0, MyThreadFun_EachPicture,& tempForThread_EachPicturemen[j], 0, NULL);  
	}

	WaitForMultipleObjects(realchipNumber, handle, TRUE, INFINITE);  

	//销毁互斥量  
	CloseHandle(mutex );  
	CloseHandle(FilePathmutex );  
	
	free(handle);
	free(tempForThread_EachPicturemen);

	//释放mat
	//myreleaseLogoMatrix(LogoMatrix,numberOfLogo,NUMBEROF_CENG);

	myreleaseFeatStruct(&logofeat, numberOfLogo, NUMBEROF_CENG );

	//释放索引的
	cvReleaseMemStorage(&storage_ForBigClass);
	myreleaseBigIndexAndSmallClass( featseq_ForBigClass,BigIndex);
	cvReleaseMat(&AvgVector);
	cvReleaseMat(&EigenVector);

	saveResult_Matched(numberForMachedPicture);
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC/60;


	saveTime(-1);
	saveConsumeTime(duration);
	printf( "/n/n/n/n/n/n 时间 ：%lf M/n", duration );
	system("pause");

}



