#ifndef MYFILETOOLS_H
#define MYFILETOOLS_H

#include "myBaseTools.h"


#include "myFileTools.h"

static char filename[300];//ֻ����һ���ļ���
HANDLE  mutex ;  //���ڻ���ı����ļ�
HANDLE  FilePathmutex;//�����ļ�Ŀ¼

void ClearFile(char * name );


void myMachedPictureFilePath(char *srcTestPicturePath,char *dstTestPicturePath,/*Ŀ��·��*/
	int kPices/*��ͼ���*/,int logoi/*logo���*/);


void saveResult_Matched(int matchedNumber);

void saveConsumeTime(double time);

static void timeTostring(char * time);

void getFatherPath(char* FatherPath,char *childPath);

void myCreateFatherFile(char *dstTestPicturePath);

void logSigma(int d,double temp ,double temp_SIGOLIKELY );

void logSigmaCompare(int d,double bili );


void saveTime(int i);

void SaveAReturn(  );


void SaveLastResult( int j, int k, int i);

void SaveResult( int j, int k, int i);


void pathCatNumber(char * str,int n);


char * changepath(const char * srcTest,char *catstr ,char *src);

#endif