#ifndef MYFILETOOLS_H
#define MYFILETOOLS_H

#include "myBaseTools.h"


#include "myFileTools.h"

static char filename[300];//只保存一个文件名
HANDLE  mutex ;  //用于互斥的保存文件
HANDLE  FilePathmutex;//用于文件目录

void ClearFile(char * name );


void myMachedPictureFilePath(char *srcTestPicturePath,char *dstTestPicturePath,/*目标路径*/
	int kPices/*子图序号*/,int logoi/*logo序号*/);


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