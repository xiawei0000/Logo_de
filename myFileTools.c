#include "myFileTools.h"

static char filename[300];//ֻ����һ���ļ���
HANDLE  mutex ;  //���ڻ���ı����ļ�
HANDLE  FilePathmutex;//�����ļ�Ŀ¼

void ClearFile(char * name )
{	//�����ʼ�ĳ�ʼ�� �ļ���
	FILE * savefile;
	//	char filename[200];
	char *filetype="save.txt";

	time_t rawtime;
	struct tm * timeinfo;
	char strtime[300];
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	strcpy(strtime,asctime (timeinfo));
	strcpy(filename,name);

	timeTostring(strtime);
	strcat(filename, strtime );
	strcat(filename,filetype);

	if( !( savefile=fopen( filename,"w") ) )
	{
		printf("���ļ����� %s",filename);
		fclose(savefile);
		return ;
	}

	fputs("��¼��� \n \n",savefile);

	fclose(savefile);
}

void myMachedPictureFilePath(char *srcTestPicturePath,char *dstTestPicturePath,/*Ŀ��·��*/
	int kPices/*��ͼ���*/,int logoi/*logo���*/)
{//ͨ�� ԭ����ͼ·����+ k�֣�+logoi =�� �õ�������·��
	char  tempNameForPictureChange[_MAX_PATH];
	char *nameForPictureChange="_Mach_";
	char *nameForPictureSub="��ͼ_";
	char tempnumber[10];


	//�����ͼƬ

	//��ͼ_K 
	strcpy(tempNameForPictureChange,nameForPictureSub);//�����ͼ��Ĵ���
	intToChar(kPices,tempnumber);
	strcat(tempNameForPictureChange,tempnumber);


	strcat(tempNameForPictureChange,nameForPictureChange);
	//_macth i
	intToChar(logoi,tempnumber);
	strcat(tempNameForPictureChange,tempnumber);



	//ƴ�ӱ���·�� //Ĭ�� ��ʽ��4��char   ��jpg
	changepath( srcTestPicturePath,tempNameForPictureChange,dstTestPicturePath);

}


void saveResult_Matched(int matchedNumber)
{
	FILE * savefile;

	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("���ļ����� %s",filename);
		fclose(savefile);
		return ;
	}

	fprintf(savefile,"һ��ƥ�䵽%d ��logo",matchedNumber);
	fclose(savefile);

}

void saveConsumeTime(double time)
{
	FILE * savefile;
	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("���ļ����� %s",filename);
		fclose(savefile);
		return ;
	}

	fprintf(savefile,"�ܺ�ʱ ��%.2lf ����",time);
	fclose(savefile);


}


static void timeTostring(char * time)
{//ʱ������и� 10=�س�
	int len;
	
	int i;
	len=strlen(time);
	for(i=0;i< len; i++)
	{
		if (time[i] ==' ' || time[i]== ':')
		{
			time[i]='_';
		}
		if (i ==len-1)
		{
			time[i]=0;

		}
	}


}

void getFatherPath(char* FatherPath,char *childPath)
{
	int len,i,j,k;
	len=strlen(childPath);
	for (i=len-1;i >=0 ; i-- )
	{//�õ���Ŀ¼
		if (childPath[i] == '\\')
		{
			break;
		}
	}
	//���Ƹ�Ŀ¼��FatherPaht
	for(j=0; j<= i; j++)
	{// / Ҳ���ƹ�ȥ

		FatherPath[j]=childPath[j];
	}
	FatherPath[j]=0;//��β
}

void myCreateFatherFile(char *dstTestPicturePath)
{
	//�ҵ�·���ĸ�Ŀ¼��Ȼ���������ļ���Ŀ¼����󷵻ر����·��
	//D:\360\Workspace\C++\LAB_work\babaoli_pictureLab\NEW_08\burberry\ ***_match.jpg
	/*
		��һ������ɨ�裬\��  ���Ƶ� ������
		�ڶ�����������Ŀ¼�� \burberry\Result_File_
	*/
	char *FatherFileName="Picture_Result_File";
	char FathrePath[_MAX_PATH];
	//	char childFile[_MAX_PATH];
	int len,i,j,k;
	len=strlen(dstTestPicturePath);
	for (i=len-1;i >=0 ; i-- )
	{//�õ���Ŀ¼
		if (dstTestPicturePath[i] == '\\')
		{
			break;
		}
	}

	//���Ƹ�Ŀ¼��FatherPaht
	for(j=0; j<= i; j++)
	{// / Ҳ���ƹ�ȥ

		FathrePath[j]=dstTestPicturePath[j];
	}
	FathrePath[j]=0;
	strcat(FathrePath,FatherFileName);


	if (_access(FathrePath,0)  != -1)
	{//Ŀ¼���ڣ�
		;
	}
	else
	{//ͬ������
		WaitForSingleObject(FilePathmutex, INFINITE);  
			_mkdir(FathrePath);//������Ŀ¼���
		ReleaseMutex(FilePathmutex); 
	
	}


	//i++;//����\  
	for (k=i,j=strlen(FathrePath);k< len;k++,j++)
	{
		FathrePath[j]=dstTestPicturePath[k];
	}
	FathrePath[j]=0;

	strcpy(dstTestPicturePath,FathrePath);

}


void logSigma(int d,double temp ,double temp_SIGOLIKELY )
{	
	FILE * savefile;
	//	char filename[200];
	char *filetype="save.txt";

	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("���ļ����� %s",filename);
		fclose(savefile);
		return ;
	}
	
	fprintf(savefile,"	%3d %.3lf %.3lf ",d,temp,temp_SIGOLIKELY);
	fclose(savefile);
}

void logSigmaCompare(int d,double bili )
{	
	FILE * savefile;
	//	char filename[200];
	char *filetype="save.txt";

	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("���ļ����� %s",filename);
		fclose(savefile);
		return ;
	}

	fprintf(savefile,"	%3d   %.3lf ",d,bili);
	fclose(savefile);
}


void saveTime(int i)
{
	double 	duration;
	FILE * savefile;

	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	//printf ( "\007The current date/time is: %s", asctime (timeinfo) );

	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("���ļ����� %s",filename);
		fclose(savefile);
		return ;
	}
	if(i == -1)
	{	
		fprintf(savefile,"\n ������� *****************\n ��ǰʱ��:");

	}
	else if( i==0){
		fprintf(savefile,"\n ��ʼ���*****************\n ��ǰʱ��:");
	}
	else  
	{
		fprintf(savefile,"\n ��ʼ���ͼƬT%d    ��ǰʱ�䣺",i);
	}

//	fputs("",savefile);
	fputs(asctime (timeinfo),savefile);
	fprintf(savefile,"\n");
	fclose(savefile);
}

void SaveAReturn(  )
{
	FILE * savefile;
	
	/*char filename[200];
	char *filetype="save.txt";

	strcpy(filename,name);
	strcat(filename,filetype);
	*/
	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("���ļ����� %s",filename);
		fclose(savefile);
		return ;
	}

	fputs("\n",savefile);

	fclose(savefile);
}


void SaveLastResult( int j, int k, int i)
{//��������ƥ������ ����ͳ��ƥ���˶��ٸ��� ���̺߳� SaveResult�����ң� �Ϳ������
	FILE * savefile;	
	char pictureName[100];	
	char temp[100];


	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("���ļ����� %s",filename);
		fclose(savefile);
		return ;
	}
	strcpy(pictureName,"\n����ͼƬT");

	intToChar(j,temp);
	strcat(pictureName,temp);

	if (k >0)
	{
		strcat(pictureName,"��ͼ");
		intToChar(k,temp);
		strcat(pictureName,temp);
	}

	strcat(pictureName," ƥ��LOGO ��");
	intToChar(i,temp);
	strcat(pictureName,temp);

	fputs(pictureName,savefile);
	fputs("\n",savefile);

	fclose(savefile);
}

void SaveResult( int j, int k, int i)
{
	FILE * savefile;	
	char pictureName[100];	
	char temp[100];

/*	char filename[200];
	char *filetype="save.txt";
	strcpy(filename,name);
	strcat(filename,filetype);
	*/

	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("���ļ����� %s",filename);
		fclose(savefile);
		return ;
	}
	strcpy(pictureName,"\n����ͼƬT");

	intToChar(j,temp);
	strcat(pictureName,temp);

	if (k >0)
	{
		strcat(pictureName,"��ͼ");
		intToChar(k,temp);
		strcat(pictureName,temp);
	}

	strcat(pictureName," ƥ��LOGO ��");
	intToChar(i,temp);
	strcat(pictureName,temp);

	fputs(pictureName,savefile);
	fputs("\n",savefile);

	fclose(savefile);
}



void pathCatNumber(char * str,int n)
{//str��Ŀ¼ Ҫ���� n+ .jpg 
	const char * picture=".jpg";
	int i;
	int len=strlen(str);
	char  number[50];
	intToChar(n,number);
	strcat(str,number);
	strcat(str,picture);
/*	 
	for(i=0;i<strlen(picture);i++)
	{
		str[len++]=picture[i];
	}
	str[len]=0;
	*/
}

char * changepath(const char * srcTest,char *catstr ,char *src)
{//��dst���뵽src��
	int i=0,j;

	const int LENGOF_END=4;
	int strend=0;
	int leng;

	strcpy(src,srcTest);
	leng=strlen(catstr);
	i=strlen(srcTest); //�õ��ַ�����β
	i-=LENGOF_END;//.jpg 4���ַ�
	strend=i;

	
	for (j=0;j<leng;j++)
	{
		src[strend+j]=catstr[j];
	}
	for (j=0;j<LENGOF_END;j++)
	{
		src[strend+leng+j]=srcTest[strend+j];
	}

	src[strend+leng+LENGOF_END]=0;
	return src;
}



