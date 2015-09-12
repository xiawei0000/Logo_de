#ifndef MYSQL_MAIN_H
#define MYSQL_MAIN_H
#include "mysql_tools.h"


void mySqlInsertResult(int URLImage_idURLImage,
	int AuthorityImage_idAuthorityImage);

void mySqlGetPictureAddress
	(int idProgect,
	MYSQL *sock, 
	srcIndexStruct **srcIndex,
	int *numberOfRows
	);



void mySqlGetLogoAddress
	(int idBrand,
	MYSQL *sock, 
	srcIndexStruct **srcIndex,int* numberOfRows
	);


int mySqlGetidBrand
(int idProgect ,
MYSQL *sock);

void mySqlLogoSrc(int idProgect,int*numberOfRows,srcIndexStruct **srcIndex);


void mySqlPictureSrc(int idProgect,int*numberOfRows,srcIndexStruct **srcIndex);

#endif
