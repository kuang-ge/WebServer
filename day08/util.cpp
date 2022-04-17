#include "util.h"
#include <stdio.h>//包含perror()的头文件
#include <stdlib.h>//包含exit()的头文件
void errif(bool condition,const char* errmsg){
	if(condition){
		perror(errmsg);
		exit(EXIT_FAILURE);
	}
}









