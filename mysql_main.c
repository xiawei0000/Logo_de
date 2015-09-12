#include"mysql_main.h"
//extern const int MAXLENG_QBUF;


void  __main(int argc, char **argv) //char **argv 相当于 char *argv[]
{
	//init_insetLogo();
	//	init_insetPicture();
//	mySqlLogoSrc(1);
	int numberOfRows;
	srcIndexStruct *srcIndex;
	//mySqlPictureSrc(1,&numberOfRows,&srcIndex);

	system("pause");
	return ;
}
/*
void mySqlInsertResult(int )
{
	mySqlInsertResultFromId(int URLImage_idURLImage,int AuthorityImage_idAuthorityImage);

}

FromId
*/


void mySqlInsertResult(int URLImage_idURLImage,int AuthorityImage_idAuthorityImage)
{/*读入 logo数组的index ， 
	图片数组的index，
	写入：图片id ， 侵权类型2， logo id
 */
	MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
	MYSQL_RES *res;       //查询结果集，结构类型
	//	MYSQL_FIELD *fd ;     //包含字段信息的结构
	//	MYSQL_ROW row ;       //存放一行查询结果的字符串数组
	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串

	int k;

	//char 一层将\\\\转义为\\  而sql的buf将\\再次转义为\  ;
	//char *srcIndex="D:\\\\360\\\\Workspace\\\\C++\\\\LAB_work\\\\babaoli_pictureLab\\\\NEW_08\\\\burberry\\\\";//返回的logo 地址数组
	char tempsrc[_MAX_PATH];
	int temp=0;
	char tempint[20];

	//int URLImage_idURLImage;
	const int TortType_idTortType=2;
	//int AuthorityImage_idAuthorityImage;


	/////////////////建立连接
	mysql_init(&mysql);//初始化
	if (!(sock = mysql_real_connect//建立连接   返回sock指针
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//错误的返回
		perror("");
		//exit(1);
	}

	///////////////////////////完成插入

	{
		sprintf(qbuf,"INSERT INTO `imagetorts` (URLImage_idURLImage,TortType_idTortType) VALUES ( \'%d\', \'%d\');"
			,URLImage_idURLImage,TortType_idTortType);

		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		}

	}

	/////////////////////////////释放
	//mysql_free_result(res);
	mysql_close(sock);
//	system("pause");
//	exit(0);
	return ;   //. 为了兼容大部分的编译器加入此行


}
void mySqlGetPictureAddress
	(int idProgect,
	MYSQL *sock, 
	srcIndexStruct **srcIndex,
	int *numberOfRows
	)
{//从idbrand 查找对应的picture

	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串
	int k;
	//int numberOfRows;

	MYSQL_RES *res;  
	MYSQL_ROW row  ;
	int idURLBase;

	//第一步： 查urlbase表， 找到idproject对应的 idurlbase

	sprintf(qbuf,"select   idURLBase  from urlbase \
				 where Project_idProject=%d;",idProgect);
	if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
	{
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		exit(1);
	}
	
	if (!(res=mysql_store_result(sock))) {
		fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
		exit(1);
	}
	
	row = mysql_fetch_row(res);
	idURLBase=atoi(row[0]);

	//第二步： 查urlimage表，找urlBase_idURLbase=idurlBase  得到所有ImageAdddresss
	/*sprintf(qbuf,"select  idURLImage ,ImageAddress  from urlimage \
				 where URLBase_idURLBase=%d;",idURLBase);
*/

	//测试用
	sprintf(qbuf,"select  idURLImage ,ImageAddress  from urlimage \
				 where URLBase_idURLBase=%d AND idURLImage >100000;",idURLBase);

	if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
	{
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		exit(1);
	}


	if (!(res=mysql_store_result(sock))) {
		fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
		exit(1);
	}

	*numberOfRows=mysql_num_rows(res);//多少个行= picture数目

	//logo 地址是由 char数组组成
	*srcIndex=(srcIndexStruct *)malloc(sizeof(srcIndexStruct )*(*numberOfRows));

	for (k=0;k < *numberOfRows; k++)
	{	
		(*srcIndex)[k].srcIndex=(char *)malloc(sizeof(char)*_MAX_PATH);
	}

	k=0;
	while (row = mysql_fetch_row(res)) 
	{
		strcpy( (*srcIndex)[k].srcIndex,  row[1]);
		 (*srcIndex)[k].id= atoi( row[0]);
		k++;
	} 

	mysql_free_result(res);

	return ;
}



void mySqlGetLogoAddress
	(int idBrand,
	MYSQL *sock, 
	srcIndexStruct **srcIndex,int* numberOfRows
	)
{//从idbrand 查找对应的logo


	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串
	int k;
//	int numberOfRows;

	MYSQL_RES *res;  
	MYSQL_ROW row  ;

	sprintf(qbuf,"select    idLogo,  logoAddress  from logo \
				 where brand_idbrand=%d;",idBrand);
	if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
	{
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		exit(1);
	}

		//查到后，如何排序好？？？？？？？？？？？
	if (!(res=mysql_store_result(sock))) {
		fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
		exit(1);
	}

	*numberOfRows = mysql_num_rows(res);//多少个行= logo数目

	//logo 地址是由 char数组组成
		
	*srcIndex=(srcIndexStruct *)malloc(sizeof(srcIndexStruct )*(*numberOfRows));

	for (k=0;k < *numberOfRows; k++)
	{	
		(*srcIndex)[k].srcIndex=(char *)malloc(sizeof(char)*_MAX_PATH);
	}

	k=0;
	while (row = mysql_fetch_row(res)) 
	{
		strcpy( (*srcIndex)[k].srcIndex,  row[1]);
		(*srcIndex)[k].id= atoi( row[0]);
		k++;
	} 


	mysql_free_result(res);

	return ;
}


int mySqlGetidBrand
(int idProgect ,
MYSQL *sock)
{
	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串
	int k;
	int idBrand;
	
	MYSQL_RES *res;  
	MYSQL_ROW row  ;

	/*
	sprintf(qbuf,"select   Brand_idBrand  from view_projectimage \
				 where idProject=%d;",idProgect);
				 */
	//没有数据 ，先直接progect找
	sprintf(qbuf,"select   Brand_idBrand  from project \
				 where idProject=%d;",idProgect);

	if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
	{
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		exit(1);
	}


	if (!(res=mysql_store_result(sock))) {
		fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
		exit(1);
	}

	//	printf("number of fields returned: %d\n",mysql_num_fields(res));
	//fd=mysql_fetch_field(res);
	row = mysql_fetch_row(res);
	idBrand=atoi(row[0]);

	mysql_free_result(res);

	return idBrand;
}

void mySqlLogoSrc(int idProgect,int*numberOfRows,srcIndexStruct **srcIndex)
{
	MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
	MYSQL_RES *res;       //查询结果集，结构类型
//	MYSQL_FIELD *fd ;     //包含字段信息的结构
//	MYSQL_ROW row ;       //存放一行查询结果的字符串数组
	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串
//	int numberOfRows;
	int k;
	int idBrand;
//	char **srcIndex;//返回的logo 地址数组

	/////////////////建立连接
	mysql_init(&mysql);//初始化
	if (!(sock = mysql_real_connect//建立连接   返回sock指针
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//错误的返回
		perror("");
	 	exit(1);
	}

	/*
	sprintf(qbuf,SELECT_QUERY,atoi(argv[1]));//SELECT_QUERY 是宏定义 语句+%s
	if(mysql_query(sock,qbuf)) {//查询  语句保存在qbuf里面
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		exit(1);
	}
	*/
	//"insert into T_man values('abb',112); ",
	//	(unsigned long)strlen("insert into T_man values('abb',112); ") ) )

	///////////////////////////完成查询

	//第一步：查brandid

	idBrand=mySqlGetidBrand
		( idProgect ,
		sock);

	//查logo路径
	mySqlGetLogoAddress
		( idBrand,
		sock , 
		srcIndex
		,numberOfRows);


	/////////////////////////////释放
	//mysql_free_result(res);
	mysql_close(sock);
	//system("pause");
	return ;   //. 为了兼容大部分的编译器加入此行
}



void mySqlPictureSrc(int idProgect,int*numberOfRows,srcIndexStruct **srcIndex)
{//获得图片的src数组
	MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
	MYSQL_RES *res;       //查询结果集，结构类型
//	MYSQL_FIELD *fd ;     //包含字段信息的结构
//	MYSQL_ROW row ;       //存放一行查询结果的字符串数组
	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串

	
	int k;
	int idBrand;
	//返回的logo 地址数组

	/////////////////建立连接
	mysql_init(&mysql);//初始化
	if (!(sock = mysql_real_connect//建立连接   返回sock指针
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//错误的返回
		perror("");
		exit(1);
	}


	///////////////////////////完成查询


	//查picture路径
	mySqlGetPictureAddress
		( idProgect,
		sock , 
		srcIndex
		,numberOfRows);

	/////////////////////////////释放
	mysql_close(sock);
	return ;   //. 为了兼容大部分的编译器加入此行
}