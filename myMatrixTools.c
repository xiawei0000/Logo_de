#include "myMatrixTools.h"

 const int NUMBERFORDIVISIOM=128;

void 	mysaveMatrix(CvMat *result_result,char *filepath,int pices)
{//������logo·�����棬 logosrc + matrix_i.xml

	char filename[300];
	char *filetype=".xml";
	char *filetemp="\\matrix_";
	char number[20];
	strcpy(filename,filepath);
	strcat(filename,filetemp);
	intToChar(pices,number);
	strcat(filename,number);
	strcat(filename,filetype);//ƴ��·��

	cvSave(filename,result_result,NULL,NULL,cvAttrList(0,0));

}

void myreadMatrix(CvMat **result_result,char *filepath,int pices)
{//��ȡ�ļ�matrix
	char filename[300];
	char *filetype=".xml";
	char *filetemp="\\matrix_";
	char number[20];
	strcpy(filename,filepath);
	strcat(filename,filetemp);
	intToChar(pices,number);
	strcat(filename,number);
	strcat(filename,filetype);//ƴ��·��

	* result_result=(CvMat*)cvLoad(filename,0,0,0);

}



void myreleaseLogoMatrix(CvMat  ***LogoMatrix,int numberOfLogo,int NUMBEROF_CENG)
{
	int i,j;
	for (i=0; i<numberOfLogo ;i++ )
	{
		for (j=0;j<NUMBEROF_CENG;j++)
		{
			if (LogoMatrix[i][j]!=NULL)
			{			
				cvReleaseMat(&LogoMatrix[i][j]);
			}

		}
		free(LogoMatrix[i]);
	}
	free(LogoMatrix);


}


void caculateMO(CvMat * result)
{//�����ÿһ�е�ģ
	int i,j;
	double tempindex;

	for (i=0;i< result->rows;i ++)
	{
		tempindex=0.0;
		for(j=0;j< result->cols; j++)
		{
			tempindex+=(double )CV_MAT_ELEM(*result,double,i ,j) *
				(double )CV_MAT_ELEM(*result,double,i ,j);
		}
		tempindex= sqrt(tempindex);

		for(j=0;j< result->cols; j++)
		{
			(double )	CV_MAT_ELEM(*result,double,i ,j)=
				(double )	CV_MAT_ELEM(*result,double,i ,j)
				/ tempindex	;


		}

	}
}



void MatrixTranslateAndMO(CvMat *result,CvMat *resultT)
{//result ת��+ ��ģ
	cvTranspose(result,resultT);	
	caculateMO(resultT);//��ģ
}


void featToMatrixWithoutTranslate(CvMat *LogoMatrix,
struct feature *feat ,int numberOfFeat)
{//1.feat���ϵ����� ��ת�ã�����mo
	int k,m;
	struct feature *feattemp;

	for (k=0 ;k< numberOfFeat ;k++)
	{
		for (m=0;m< NUMBERFORDIVISIOM;m++)
		{
			(double )	CV_MAT_ELEM(*LogoMatrix,double,k ,m) =
				(double ) ( ( feat[k]).descr[m]);
		}
	}

}



void featToMatrix(CvMat *LogoMatrix,struct feature *feat ,int numberOfFeat)
{//1.feat���ϵ�����2��ת�� 3.����ģ
	int k,m;
	struct feature *feattemp;
	double *tempindex;
	tempindex=(double *)malloc(sizeof(double)*numberOfFeat);

	for (k=0 ;k< numberOfFeat ;k++)
	{
		tempindex[k]=0.0;
		for (m=0;m< NUMBERFORDIVISIOM;m++)
		{
			(double )	CV_MAT_ELEM(*LogoMatrix,double,m ,k) =
				(double ) ( ( feat[k]).descr[m]);

			tempindex[k]+= (double ) ( ( feat[k]).descr[m]) * (double ) ( ( feat[k]).descr[m]);
		}
	}

	//����ģ
	for (k=0 ;k< numberOfFeat ;k++)
	{
		tempindex[k]= sqrt(tempindex[k]);

		for (m=0;m< NUMBERFORDIVISIOM;m++)
		{
			(double )	CV_MAT_ELEM(*LogoMatrix,double,m ,k) =
				(double )	CV_MAT_ELEM(*LogoMatrix,double,m ,k) / tempindex[k]	;

		}
	}

	free(tempindex);

}

void LogoFeatStructToMatrix(CvMat *LogoMatrix,LogoFeatStruct logofeat)
{
	int k,m;
	double *tempindex;
	tempindex=(double *)malloc(sizeof(double)*  (logofeat).numberOffeat);

	for (k=0 ;k< (logofeat).numberOffeat ; k++)
	{
		tempindex[k]=0.0;

		for (m=0;m< NUMBERFORDIVISIOM;m++)
		{
			(double )	CV_MAT_ELEM(*LogoMatrix,double,k ,m) =
				(double ) (logofeat) .feat1[k].descr[m];

			tempindex[k]+= (double ) (logofeat) .feat1[k].descr[m] * 
				(double ) (logofeat) .feat1[k].descr[m];
		}
	}

	//����ģ
	for (k=0 ;k< (logofeat).numberOffeat  ;k++)
	{
		tempindex[k]= sqrt(tempindex[k]);
		for (m=0;m< NUMBERFORDIVISIOM;m++)
		{
			(double )	CV_MAT_ELEM(*LogoMatrix,double,k ,m) =
				(double )	CV_MAT_ELEM(*LogoMatrix,double,k,m) / tempindex[k]	;
		}
	}

	free(tempindex);

}




void printMyMat(CvMat* tagMat)
{
	int i,j;
	for(i=0;i<tagMat->rows;i++)
	{
		for (j=0;j<tagMat->rows;j++)
		{ //Ϊ�����3�����Ǹ�������������
			if ((int) *(tagMat->data.ptr+tagMat->step *i+j) )
			{
				// fprintf( stderr, "%d",(int) CV_MAT_ELEM(*tagMat,int,i,j) );
				fprintf( stderr, " %d",(int) *(tagMat->data.ptr+tagMat->step *i+j));
			}
		}
		fprintf(stderr,"\n");
	}

}
