#include "tools.h"
#include <stdio.h>  
#include <process.h>  



const double SIGOLIKELY=  0.15;//���ƶȵķ�ֵ
const double BIG_SIGOLIKELY=  1.5;//�����Ƶ�
const int MINNUMBER_MACHING=5;




const int MINNUMBER_FORFEAT=5;

 const int  chipNumber=4;
 int numberForMachedPicture=0;//����ƥ��ĸ���

 const char nameForPictureChange[50]="_Mach_";



 //ʹ�� 1-cos ��������ֵ
 double YUZHI_COS_MIN=0.8;
 double YUZHI_COS_MAX=0.994;//���������
 double YUZHI_COS=0.35;
  /*
 //��һ��ֱ����cos����ֵ
 const double YUZHI_COS=1.7;
 const double  YUZHI_COS_MAX=0.95;
 const double YUZHI_COS_MIN=0.80;*/


 //pcaת����Ĳ���
 int NUMBERFORDIVISIOM_PCA=20;
 const double PART_PCA=0.98;//��ά������
 const double MINCOS_PCA=0.995;// ��cos=�� ����   =35  0.95= 18;  //������ -1 --1�ķ�Χ��
