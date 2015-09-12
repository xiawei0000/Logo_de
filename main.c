
#include "mysql_main.h"
#include "mach.h"
//extern  int find_Drawcontours (IplImage *src,IplImage *dst);
extern const int NUMBEROF_CENG;//logoҪ����-1 ��

extern  const int MINNUMBER_MACHING;
extern const int MINNUMBER_FORFEAT;
 
extern const char nameForPictureChange[50];
extern  int numberForMachedPicture;
extern const int  chipNumber;
extern 	HANDLE  mutex;
extern 	HANDLE  FilePathmutex;
int main( int argc, char** argv )
{

	//���̲߳���
	HANDLE * handle; 
	struct tempStructForThread_EachPicture *tempForThread_EachPicturemen;

	//���ݿ⣺����
		//�����
	int idProgect;
	int numberOfLogo;//numberOfRows;
	srcIndexStruct *logo_srcIndex;//logo�ĵ�ַ+id ��

	int numberOfPicture;//numberOfRows;
	srcIndexStruct *Picture_srcIndex;// �ĵ�ַ+id ��


	//pca ����

	CvMat *AvgVector=NULL;
	CvMat *EigenVector=NULL;
	CvMat  *BigIndex=NULL;
	CvSeq *featseq_ForBigClass;//����࣬ ����
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


	CvSize **logoSizeIndex;//����size����
	LogoFeatStruct ** logofeat;//���ڱ���һ�μ����logofeat
	CvMat  ***LogoMatrix;
	int realchipNumber;


	start = clock();//��ʼ��ʱ


	mutex  = CreateMutex(NULL, FALSE, NULL);  
	FilePathmutex= CreateMutex(NULL, FALSE, NULL);  


	//��������

	idProgect=atoi(argv[1]);
	mySqlLogoSrc( idProgect,&numberOfLogo,&logo_srcIndex);//���logo����
	//numberOfLogo==0 DE CHULI=================================================================	===========================
	//����
//	myCreateFatherFile(logo_srcIndex[1].srcIndex);
	//����
//	numberOfLogo=3;


	ClearFile( filePath);
	saveTime(0);
		/**/
	//������뻻�ɾ��� ����
	CaculateLogoFeatFirst(  logo_srcIndex, &logofeat, numberOfLogo, NUMBEROF_CENG,&logoSizeIndex);//,&LogoMatrix );
	//?????????????????????��������ȵõ�size���󷵻ء�
//����
	numberOfLogo=1;

	//�������е�logo����
	//logo���뱣����ͬһ���ļ���
	//���logo�ļ���
	getFatherPath(LogoFilepath,logo_srcIndex[0].srcIndex);
	storage_ForBigClass = cvCreateMemStorage( 0 );	
	myCacultatePCA_new(  logofeat,  numberOfLogo,  NUMBEROF_CENG ,
		&AvgVector,&EigenVector,&BigIndex,&featseq_ForBigClass,storage_ForBigClass,LogoFilepath);


	

	//�ڶ����������ݿ��picture
	numberForMachedPicture=0;
	mySqlPictureSrc( idProgect,&numberOfPicture,&Picture_srcIndex);//��� picture����
	//��ѯ�ٶ�ԶԶ���ڲ����ٶ�
	numberForMachedPicture=4;

	//���ԣ�numberOfPicture
//	numberOfPicture=3;

	//��� ��picture������ƥ��
	realchipNumber =chipNumber< numberOfPicture? chipNumber: numberOfPicture;

	//���̲߳�����
	handle=(HANDLE *) malloc(sizeof(HANDLE)*realchipNumber);
	tempForThread_EachPicturemen=(struct tempStructForThread_EachPicture *)
		malloc(sizeof(struct tempStructForThread_EachPicture)*realchipNumber);

	realchipNumber=1;
	for(j=0;j<realchipNumber;j++)
	{	//ʧ�ܣ���Ϊ���߳�ʱ�� kdtreee���޸�feat��data�����Զ���߳�ͬʱ�õ�ʱ�򣬾����ˡ�
		init_tempStructForThread_EachPicture(
			&tempForThread_EachPicturemen[j],
			Picture_srcIndex,
			 logo_srcIndex,
			numberOfPicture,
			 numberOfLogo,
			 logofeat,
	//		 LogoMatrix,�滻��pca���� ����
			 AvgVector,EigenVector,
			 BigIndex,
			 featseq_ForBigClass,
			 logoSizeIndex,
			 j);
		handle[j] = (HANDLE)_beginthreadex(NULL, 0, MyThreadFun_EachPicture,& tempForThread_EachPicturemen[j], 0, NULL);  
	}

	WaitForMultipleObjects(realchipNumber, handle, TRUE, INFINITE);  

	//���ٻ�����  
	CloseHandle(mutex );  
	CloseHandle(FilePathmutex );  
	
	free(handle);
	free(tempForThread_EachPicturemen);

	//�ͷ�mat
	//myreleaseLogoMatrix(LogoMatrix,numberOfLogo,NUMBEROF_CENG);

	myreleaseFeatStruct(&logofeat, numberOfLogo, NUMBEROF_CENG );

	//�ͷ�������
	cvReleaseMemStorage(&storage_ForBigClass);
	myreleaseBigIndexAndSmallClass( featseq_ForBigClass,BigIndex);
	cvReleaseMat(&AvgVector);
	cvReleaseMat(&EigenVector);

	saveResult_Matched(numberForMachedPicture);
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC/60;


	saveTime(-1);
	saveConsumeTime(duration);
	printf( "/n/n/n/n/n/n ʱ�� ��%lf M/n", duration );
	system("pause");

}



