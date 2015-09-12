#include "myPictureTools.h"

const int MAXLOG_SIZE=300;
const int MAXSIZE_FORPICTURE=700;//�����ͼƬ

 const int MAXNUMBER_LOGO=3;//ƥ��ͼ�� �����1:3
 const int NUMBEROF_CENG=8;//logoҪ����-1 ��
 const double NUMBERFOR_RESIZE=1.3;//ÿ����С����
 const  int  SLIPER=10;//һ�� �������� =logo��10��֮һ
 const int MIN_SLIPSIZE=5;  //��С����
 const int SMALLIMAG_SIZE=25;//��Сlogo�Ĵ�С

void myDrawRectToImage(IplImage *img,CvRect rect)
{//��img�� ��һ��rect
	cvRectangleR(img,rect,CV_RGB(255,0,0),3,8,0);  

}



void GetMachedWindowPoint(MachedFeatPoint* featTemp,
	CvPoint * p1,CvPoint * p2,CvSize img1size,CvSize img1size2 )
{//��������ƥ�䵽�ĵ� ������ ƥ��Ĵ���λ��
	p1->x=( featTemp->machedPointFeat2.x -featTemp->machedPointFeat1.x );
	p1->y=( featTemp->machedPointFeat2.y -featTemp->machedPointFeat1.y);

	p2->x= img1size.width -featTemp->machedPointFeat1.x + featTemp->machedPointFeat2.x;
	p2->y= img1size.height -featTemp->machedPointFeat1.y + featTemp->machedPointFeat2.y;

	//��ֹԽ��
	if (p1->x < 0)
	{
		p1->x =0;
	}
	if (p1->y< 0)
	{
		p1->y = 0;
	}

	if (p2->x > img1size2.width)
	{
		p2->x = img1size2.width;
	}
	if (p2->y > img1size2.height)
	{
		p2->y = img1size2.height;
	}
	/*
	 p1->x =p1->x < 0 ? 0:p1->x;
	 p2->x =p2->x > img1size2.width? img1size2.width: p2->x;

	 p1->y =p1->y< 0 ? 0: p1->y;
	 p2->y  =p2->y > img1size2.height ? img1size2.height :p2->y;
	 */
	 
}
int myDivedeImg(IplImage** *imgForPicture,IplImage* img2)
{/*��ͼ�� ��ͼ��MAXSIZE_FORPICTURE width  height�� ��
 ���ǣ�location[0] =j*MAXSIZE_FORPICTURE ;
		location[1]= i*MAXSIZE_FORPICTURE ;

						location[3]=(i+1) *MAXSIZE_FORPICTURE +MAXLOG_SIZE;
						location[2]=(j+1) *MAXSIZE_FORPICTURE +MAXLOG_SIZE;
						4����
		
 */
	int i=0,j;
	int numberWidth,numberHeight;
	int kforpicture;
	int location[4];
	CvRect roi ;
	if (img2->width%MAXSIZE_FORPICTURE==0)
	{//����
		numberWidth=img2->width/MAXSIZE_FORPICTURE ;
	}
	else
		{
			numberWidth=img2->width/MAXSIZE_FORPICTURE +1;
	}

	if (img2->height% MAXSIZE_FORPICTURE==0)
	{
			numberHeight=img2->height/ MAXSIZE_FORPICTURE;
	}
	else{
	numberHeight=img2->height/ MAXSIZE_FORPICTURE +1;
	}


	*imgForPicture=(IplImage**)malloc(sizeof( IplImage *)* numberWidth*numberHeight);
	
	kforpicture=0;
	for (i=0;i<numberHeight ;i++)
	{
		for (j=0;j< numberWidth ;j++)
		{
				location[0] =j*MAXSIZE_FORPICTURE ;
				location[1]= i*MAXSIZE_FORPICTURE ;

	
				location[2]=(j+1) *MAXSIZE_FORPICTURE +MAXLOG_SIZE;
				location[3]=(i+1) *MAXSIZE_FORPICTURE +MAXLOG_SIZE;
			
			
				if ( location[2] >img2->width )
				{//�����Ĵ�С 
					location[2]=img2->width;
				}
				if (location[3] >img2->height)
				{
					location[3]=img2->height;
				}

				roi=cvRect(location[0],location[1], 
					location[2]-location[0] ,location[3]-location[1]);
				cvSetImageROI(img2,roi);

				(*imgForPicture)[kforpicture]=cvCreateImage(cvSize(location[2]-location[0] ,location[3]-location[1])
					, img2->depth,img2->nChannels);
				cvCopy(img2,(*imgForPicture)[kforpicture],NULL);
				//cvcloneû��roi������ ��cvcopy �У�������Ҫ�Ƚ���imge

			//	cvShowImage("",(*imgForPicture)[kforpicture]);
			//	cvWaitKey(0);
				kforpicture++;
		}
	}

	return numberWidth*numberHeight;
}





void drawTagForWindow(CvMat * tagMat,int x,int y,CvSize size)
{//��window�����е㻯Ϊ1
	int i,j;

	for(i=x;i<x+size.width  && i< tagMat->cols;i++)
	{//ͼ���У�x��ʾ��y�еĵ�x�У����µ����꣩
		for(j=y;j<y+size.height  && j< tagMat->rows;j++)
		{
			*(uchar*)CV_MAT_ELEM_PTR(*tagMat,j,i)=1;//������ ��һ����ʾ�У��ڶ�����ʾ�ڼ���
		}

	}
}

void myDrawPoint(CvSeq *featseq,int numberOfMached,IplImage* stacked,int height)
{
	MachedFeatPoint *machedstruct;
	int k;
	CvPoint pt1,pt2;
	IplImage* tempStacked;


	tempStacked=cvCloneImage(stacked);


	for (k=0; k<numberOfMached; k++)
	{
		machedstruct=(MachedFeatPoint *)cvGetSeqElem(featseq,k);

		pt1	 = machedstruct->machedPointFeat1;
		pt2 =  machedstruct->machedPointFeat2;
		pt2.y += height;
		cvLine( tempStacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
	}

	cvNamedWindow( "��������", 1 );
	cvShowImage(  "��������", tempStacked );
	cvWaitKey(1);
	cvReleaseImage(&tempStacked);

}

void reduceMachedPoints(CvSeq *featseq,int * numberOfMached )
{
//����һ��ͼƬ��� logo��ĿΪ5���������������ģ����Ǵ����
	/**
	1.ѭ������ͬ����5�����Ͷ���Ϊ�Ǵ���㣬ȫ��ɾ����
	2.����5��������
	*/
	int k,j,n;
	MachedFeatPoint *machedstruct,*machedstructtemp;
	int tagForRemove=0;
	int *tagIndex;
	int tag=0;
	int numberOftag;
	tagIndex=(int *)malloc(sizeof(int) *MAXNUMBER_LOGO );

	//��һ�֣� һ��p ƥ����logo�� ��ֱ��ɾ��

	for (k=0; k< *numberOfMached; k++)
	{
		machedstruct=(MachedFeatPoint *)cvGetSeqElem(featseq,k);
		for( j=k+1;j<*numberOfMached; j++)
		{
			machedstructtemp=(MachedFeatPoint *)cvGetSeqElem(featseq,j);

			if (compareIsEqualFeat2( machedstruct , machedstructtemp ) )
			{
				cvSeqRemove(featseq,j);
				(*numberOfMached)--;
			}

		}

	}


	//�ڶ��֣�5������ p�� ƥ��һ��logo�㣬ȫ��ɾ��
	for (k=0; k< *numberOfMached; k++)
	{
		tagForRemove=0;
		tag=0;
		tagIndex[0]=k;
		tag++;

		machedstruct=(MachedFeatPoint *)cvGetSeqElem(featseq,k);

		for( j=k+1;j< *numberOfMached; j++)
		{//һ��ɨ��
			machedstructtemp=(MachedFeatPoint *)cvGetSeqElem(featseq,j);

			if (compareIsEqual( machedstruct , machedstructtemp ) )
			{
				if (tagForRemove !=0)
				{
					cvSeqRemove(featseq,j);
					(*numberOfMached)--;
				}
				else
				{
					tagIndex[tag]=j;
					tag++;
					if (tag ==MAXNUMBER_LOGO)//����
					{//ȫ��ɾ��
						tagForRemove=1;
						for (n=0;n<MAXNUMBER_LOGO;n++)
						{
							cvSeqRemove(featseq,tagIndex[n]-n);
							(*numberOfMached)--;
						}
					}
				}
			}


		}

	}

 
}

int compareIsEqualFeat2(MachedFeatPoint * machedstruct,MachedFeatPoint * machedstructTemp)
{//�ṹ���У� һ��p ƥ����logo�� ��ֱ��ɾ��

	if (machedstruct->machedPointFeat2 .x == machedstructTemp->machedPointFeat2.x
		&& machedstruct->machedPointFeat2.y == machedstructTemp->machedPointFeat2.y)
		return 1;
	return 0;
}


int compareIsEqual(MachedFeatPoint * machedstruct,MachedFeatPoint * machedstructTemp)
{//�ṹ���У�ƥ����ǲ���logo��ͬһ����

	if (machedstruct->machedPointFeat1.x == machedstructTemp->machedPointFeat1.x
		&& machedstruct->machedPointFeat1.y == machedstructTemp->machedPointFeat1.y)
		return 1;
	return 0;
}


int compareIsInWindow(MachedFeatPoint * machedstruct,
	CvPoint pointWindow1, CvPoint pointWindow2)
{

	if (machedstruct->machedPointFeat2.x >=pointWindow1.x &&
		machedstruct->machedPointFeat2.y> pointWindow1.y && 
		machedstruct->machedPointFeat2.x <=pointWindow2.x  && 
		machedstruct->machedPointFeat2.y <  pointWindow2.y  )
		return 1;
	return 0;
}


void printMyImag(IplImage* img)
{
	int i,j;
	for(i=0;i<img->width;i++)
	{
		for (j=0;j<img->height;j++)
		{ //Ϊ�����3�����Ǹ�������������

			// fprintf( stderr, "%d",(int) CV_MAT_ELEM(*tagMat,int,i,j) );
			fprintf( stderr, "%d",(int) *(img->imageData+img->widthStep*i+j));

		}
		fprintf(stderr,"\n");
	}

}



IplImage* caculateSmallSizeImg( IplImage* img ,double nForSize,int ceng)
{//��ͼƬ��СnForSize^ceng ��֮һ  �õ���ceng���ͼƬ

	int i;
	CvSize c= cvSize((int)(img->width), (int)(img->height));
	IplImage*smaller;

	for (i=1 ;i <= ceng ;i++)//���Ǵ�0��ʼ��
	{
		c=cvSize((int)(c.width /nForSize), (int)(c.height /nForSize));

	}
	smaller = cvCreateImage(c,		img->depth, img->nChannels );

	cvResize( img, smaller,CV_INTER_CUBIC);//Ч�����
	return smaller;
}

IplImage* smallSizeImg( IplImage* img ,double nForSize)
{//��ͼƬ��СnForSize��֮һ
	CvSize c= cvSize((int)(img->width /nForSize), (int)(img->height /nForSize));
	IplImage* smaller = cvCreateImage(c,
		img->depth, img->nChannels );
	cvResize( img, smaller,CV_INTER_CUBIC);//Ч�����
	// CV_INTER_AREA );
	//cvSmooth(smaller,smaller,CV_MEDIAN,3,3,0,0);
	return smaller;
}

int Img1BigImg2(IplImage*img1,IplImage*img2)
{
	if (img1->width > img2->width || img1->height > img2->height)
	{
		return  1;//ͼƬ1����ͼ2
	}
	return 0;

}
int isTooSmall(IplImage*img1)
{

	if (img1->width<SMALLIMAG_SIZE ||  img1->height<SMALLIMAG_SIZE )
	{
		return 1;
	}
	return 0;

}







int  find_Drawcontours (IplImage *src,IplImage *dst)
{

	CvMemStorage *storage = cvCreateMemStorage(0);  
	CvSeq *seq = NULL;  
	int cnt ;	 
	cvThreshold(src, src,10, 255, CV_THRESH_BINARY);      
	//	cvShowImage("src",src);  

	cnt= cvFindContours(src,storage,&seq,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0)); // ,8,1,2,cvPoint(0,0)
	//���ܼӺ���ı���
	printf("%d",cnt);
	for(;seq !=NULL;	seq = seq->h_next )
	{

		CvRect rect = cvBoundingRect(seq,1);  
		cvDrawContours(dst,seq,CV_RGB(255,0,0),CV_RGB(255,0,0),0,1,8,cvPoint(0,0));  
		cvRectangleR(dst,rect,CV_RGB(255,0,0),3,8,0);  
		//	cvShowImage("dst",dst);  
		//cvWaitKey(0);  

	}

	//cvWaitKey(0); 
	//cvReleaseImage(&src);  
	//cvReleaseImage(&dst);  
	cvReleaseMemStorage(&storage);    
	storage=NULL;
	return 0;
}







