#include "tools.h"
#include <stdio.h>  
#include <process.h>  



const double SIGOLIKELY=  0.15;//相似度的阀值
const double BIG_SIGOLIKELY=  1.5;//很相似的
const int MINNUMBER_MACHING=5;




const int MINNUMBER_FORFEAT=5;

 const int  chipNumber=4;
 int numberForMachedPicture=0;//最终匹配的个数

 const char nameForPictureChange[50]="_Mach_";



 //使用 1-cos 倒数的阈值
 double YUZHI_COS_MIN=0.8;
 double YUZHI_COS_MAX=0.994;//测试这个点
 double YUZHI_COS=0.35;
  /*
 //第一种直接用cos的阈值
 const double YUZHI_COS=1.7;
 const double  YUZHI_COS_MAX=0.95;
 const double YUZHI_COS_MIN=0.80;*/


 //pca转换后的参数
 int NUMBERFORDIVISIOM_PCA=20;
 const double PART_PCA=0.98;//降维的力度
 const double MINCOS_PCA=0.995;// 按cos=？ 分类   =35  0.95= 18;  //换到了 -1 --1的范围里
