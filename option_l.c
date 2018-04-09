#include "header.h"

extern int option_count;

void option_l(char *path)
{
	char pathname[MAX];
	char buf[MAX];
	sprintf(pathname, "%s/uptime", path); // 경로를 만든다.
	if( option_count > 1 ) {
		// 옵션이 2개 이상이면 구분 메시지를 출력한다.
		printf("([%s])\n", pathname);
	}
	FILE *fp;

	if (access(pathname, F_OK) < 0) {
		// 파일의 접근권한을 검사한다.
		if ( errno == ENOENT )
			fprintf(stderr, "%s doesn't exist.\n", pathname);
		else if (errno == EACCES)
			fprintf(stderr, "%s can't be read.\n", pathname);
		return;
	}

	if( (fp = fopen(pathname, "r")) == NULL ) {
		fprintf(stderr, "%s can't be read\n", pathname);
		return;
	}
	while( fgets(buf, sizeof(buf), fp) != NULL) {
		// 파일의 내용을 한줄 씩 읽어 토큰으로 나누고, 구분 메시지와 함께 값들을 출력한다.
		char *token = strtok(buf, " ,\n");
		printf("Process worked time : %s(sec)\n", token);
		if ( (token = strtok(NULL, " ,\n")) !=NULL ) 
			printf("Process idle time : %s(sec)\n", token);
	}
}


