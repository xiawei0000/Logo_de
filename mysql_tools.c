#include "mysql_tools.h"

//const int MAXLENG_QBUF=500;



void init_insetPicture()
{
	MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
	MYSQL_RES *res;       //查询结果集，结构类型
	//	MYSQL_FIELD *fd ;     //包含字段信息的结构
	//	MYSQL_ROW row ;       //存放一行查询结果的字符串数组
	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串

	int k;
	int idBrand;

	//char 一层将\\\\转义为\\  而sql的buf将\\再次转义为\  ;
	char *srcIndex="D:\\\\360\\\\Workspace\\\\C++\\\\LAB_work\\\\babaoli_pictureLab\\\\NEW_08\\\\burberry\\\\";//返回的logo 地址数组
	char tempsrc[_MAX_PATH];
	int temp=0;
	char tempint[20];
	/////////////////建立连接
	mysql_init(&mysql);//初始化
	if (!(sock = mysql_real_connect//建立连接   返回sock指针
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//错误的返回
		perror("");
		exit(1);
	}

	///////////////////////////完成插入
	for (k=0;k<=16935;k++)
	{
		strcpy(tempsrc,srcIndex);
		intToChar(k,tempint);
		strcat(tempsrc,tempint);//(idLogo,logoName,logoAddress, brand_idbrand)
		strcat(tempsrc,".jpg");
		sprintf(qbuf,"insert into urlimage(idURLImage,ImageName,UploadDate,URLBase_idURLBase,ImageType_idImageType,ImageAddress)\
					 values (%d,\'%d \',\'2013-12-24 15:53:10\',1,2,\'%s\');",k,k,tempsrc);
		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		//	exit(1);
		}

	}

	/////////////////////////////释放
	//mysql_free_result(res);
	mysql_close(sock);
	//system("pause");
	//exit(0);
	return ;   //. 为了兼容大部分的编译器加入此行
}



void init_insetLogo()
{
	MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
	MYSQL_RES *res;       //查询结果集，结构类型
	//	MYSQL_FIELD *fd ;     //包含字段信息的结构
	//	MYSQL_ROW row ;       //存放一行查询结果的字符串数组
	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串

	int k;
	int idBrand;

	//char 一层将\\\\转义为\\  而sql的buf将\\再次转义为\  ;
	char *srcIndex="D:\\\\360\\\\Workspace\\\\C++\\\\LAB_work\\\\babaoli_pictureLab\\\\LOGO_ALL\\\\";//返回的logo 地址数组
	char tempsrc[_MAX_PATH];
	int temp=0;
	char tempint[20];
	/////////////////建立连接
	mysql_init(&mysql);//初始化
	if (!(sock = mysql_real_connect//建立连接   返回sock指针
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//错误的返回
		perror("");
		exit(1);
	}

	///////////////////////////完成插入
	for (k=1;k<=29;k++)
	{
		strcpy(tempsrc,srcIndex);
		intToChar(k,tempint);
		strcat(tempsrc,tempint);
		strcat(tempsrc,".jpg");
		sprintf(qbuf,"insert into logo (idLogo,logoName,logoAddress, brand_idbrand)\
					 values (\'%d\',\'%d \',\'%s\',2);",k,k,tempsrc);
		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
			exit(1);
		}

	}

	/////////////////////////////释放
	//mysql_free_result(res);
	mysql_close(sock);
	//system("pause");
//	exit(0);
	return ;   //. 为了兼容大部分的编译器加入此行
}

