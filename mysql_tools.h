
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
//定义数据库操作的宏，也可以不定义留着后面直接写进代码


typedef struct SrcIndexSturct{
	char * srcIndex;//单个地址
	int id;//地址对应的id
}srcIndexStruct;

void init_insetPicture();
void init_insetLogo();
#endif