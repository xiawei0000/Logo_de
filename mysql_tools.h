
#ifndef _MYSQL_TOOLS_H
#define  _MYSQL_TOOLS_H
#include "mysql.h"
#include <windows.h>
#include"tools.h"
#include <stdio.h>
#include <stdlib.h>
#define  MAXLENG_QBUF 500
#pragma comment (lib, "libmysql.lib")
#pragma comment (lib, "mysqlclient.lib")
//�������ݿ�����ĺ꣬Ҳ���Բ��������ź���ֱ��д������


typedef struct SrcIndexSturct{
	char * srcIndex;//������ַ
	int id;//��ַ��Ӧ��id
}srcIndexStruct;

void init_insetPicture();
void init_insetLogo();
#endif