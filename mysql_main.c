#include"mysql_main.h"
//extern const int MAXLENG_QBUF;


void  __main(int argc, char **argv) //char **argv �൱�� char *argv[]
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
{/*���� logo�����index �� 
	ͼƬ�����index��
	д�룺ͼƬid �� ��Ȩ����2�� logo id
 */
	MYSQL mysql,*sock;    //�������ݿ����ӵľ�����������ڼ������е�MySQL����
	MYSQL_RES *res;       //��ѯ��������ṹ����
	//	MYSQL_FIELD *fd ;     //�����ֶ���Ϣ�Ľṹ
	//	MYSQL_ROW row ;       //���һ�в�ѯ������ַ�������
	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���

	int k;

	//char һ�㽫\\\\ת��Ϊ\\  ��sql��buf��\\�ٴ�ת��Ϊ\  ;
	//char *srcIndex="D:\\\\360\\\\Workspace\\\\C++\\\\LAB_work\\\\babaoli_pictureLab\\\\NEW_08\\\\burberry\\\\";//���ص�logo ��ַ����
	char tempsrc[_MAX_PATH];
	int temp=0;
	char tempint[20];

	//int URLImage_idURLImage;
	const int TortType_idTortType=2;
	//int AuthorityImage_idAuthorityImage;


	/////////////////��������
	mysql_init(&mysql);//��ʼ��
	if (!(sock = mysql_real_connect//��������   ����sockָ��
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//����ķ���
		perror("");
		//exit(1);
	}

	///////////////////////////��ɲ���

	{
		sprintf(qbuf,"INSERT INTO `imagetorts` (URLImage_idURLImage,TortType_idTortType) VALUES ( \'%d\', \'%d\');"
			,URLImage_idURLImage,TortType_idTortType);

		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		}

	}

	/////////////////////////////�ͷ�
	//mysql_free_result(res);
	mysql_close(sock);
//	system("pause");
//	exit(0);
	return ;   //. Ϊ�˼��ݴ󲿷ֵı������������


}
void mySqlGetPictureAddress
	(int idProgect,
	MYSQL *sock, 
	srcIndexStruct **srcIndex,
	int *numberOfRows
	)
{//��idbrand ���Ҷ�Ӧ��picture

	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���
	int k;
	//int numberOfRows;

	MYSQL_RES *res;  
	MYSQL_ROW row  ;
	int idURLBase;

	//��һ���� ��urlbase�� �ҵ�idproject��Ӧ�� idurlbase

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

	//�ڶ����� ��urlimage����urlBase_idURLbase=idurlBase  �õ�����ImageAdddresss
	/*sprintf(qbuf,"select  idURLImage ,ImageAddress  from urlimage \
				 where URLBase_idURLBase=%d;",idURLBase);
*/

	//������
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

	*numberOfRows=mysql_num_rows(res);//���ٸ���= picture��Ŀ

	//logo ��ַ���� char�������
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
{//��idbrand ���Ҷ�Ӧ��logo


	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���
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

		//�鵽���������ã���������������������
	if (!(res=mysql_store_result(sock))) {
		fprintf(stderr,"Couldn't get result from %s\n", mysql_error(sock));
		exit(1);
	}

	*numberOfRows = mysql_num_rows(res);//���ٸ���= logo��Ŀ

	//logo ��ַ���� char�������
		
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
	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���
	int k;
	int idBrand;
	
	MYSQL_RES *res;  
	MYSQL_ROW row  ;

	/*
	sprintf(qbuf,"select   Brand_idBrand  from view_projectimage \
				 where idProject=%d;",idProgect);
				 */
	//û������ ����ֱ��progect��
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
	MYSQL mysql,*sock;    //�������ݿ����ӵľ�����������ڼ������е�MySQL����
	MYSQL_RES *res;       //��ѯ��������ṹ����
//	MYSQL_FIELD *fd ;     //�����ֶ���Ϣ�Ľṹ
//	MYSQL_ROW row ;       //���һ�в�ѯ������ַ�������
	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���
//	int numberOfRows;
	int k;
	int idBrand;
//	char **srcIndex;//���ص�logo ��ַ����

	/////////////////��������
	mysql_init(&mysql);//��ʼ��
	if (!(sock = mysql_real_connect//��������   ����sockָ��
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//����ķ���
		perror("");
	 	exit(1);
	}

	/*
	sprintf(qbuf,SELECT_QUERY,atoi(argv[1]));//SELECT_QUERY �Ǻ궨�� ���+%s
	if(mysql_query(sock,qbuf)) {//��ѯ  ��䱣����qbuf����
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		exit(1);
	}
	*/
	//"insert into T_man values('abb',112); ",
	//	(unsigned long)strlen("insert into T_man values('abb',112); ") ) )

	///////////////////////////��ɲ�ѯ

	//��һ������brandid

	idBrand=mySqlGetidBrand
		( idProgect ,
		sock);

	//��logo·��
	mySqlGetLogoAddress
		( idBrand,
		sock , 
		srcIndex
		,numberOfRows);


	/////////////////////////////�ͷ�
	//mysql_free_result(res);
	mysql_close(sock);
	//system("pause");
	return ;   //. Ϊ�˼��ݴ󲿷ֵı������������
}



void mySqlPictureSrc(int idProgect,int*numberOfRows,srcIndexStruct **srcIndex)
{//���ͼƬ��src����
	MYSQL mysql,*sock;    //�������ݿ����ӵľ�����������ڼ������е�MySQL����
	MYSQL_RES *res;       //��ѯ��������ṹ����
//	MYSQL_FIELD *fd ;     //�����ֶ���Ϣ�Ľṹ
//	MYSQL_ROW row ;       //���һ�в�ѯ������ַ�������
	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���

	
	int k;
	int idBrand;
	//���ص�logo ��ַ����

	/////////////////��������
	mysql_init(&mysql);//��ʼ��
	if (!(sock = mysql_real_connect//��������   ����sockָ��
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//����ķ���
		perror("");
		exit(1);
	}


	///////////////////////////��ɲ�ѯ


	//��picture·��
	mySqlGetPictureAddress
		( idProgect,
		sock , 
		srcIndex
		,numberOfRows);

	/////////////////////////////�ͷ�
	mysql_close(sock);
	return ;   //. Ϊ�˼��ݴ󲿷ֵı������������
}