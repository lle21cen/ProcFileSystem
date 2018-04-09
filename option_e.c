#include "header.h"

void option_e(char *path)
{
	char pathname[MAX];
	char buf[MAX];
	const int newline_num = 5; // 5개 단위로 개행하기 위해 사용한다.
	sprintf(pathname, "%s/filesystems", path); // 경로를 만든다.
	if( option_count > 1 ) {
		// 옵션의 개수가 2개 이상이면 구분 메시지를 출력한다.
		printf("([%s])\n", pathname);
	}
	printf("<<Supported Filesystems>>\n");
	FILE *fp;

	if (access(pathname, F_OK) < 0) {
		// 파일의 접근 권한을 검사한다.
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
	char *token = NULL;
	int count=0, token_check=0;
	while( fgets(buf, sizeof(buf), fp) != NULL) {
		/* 파일의 내용을 한줄씩 읽어와 strtok()로 구분하여 나누고, 
		   내용이 nodev가 아닌 경우에만 token을 계속 읽어 적절한 위치에 쉼표와
		   파일 시스템을 출력한다. 그리고 count값을 늘려 한 줄에 5개가 입력되면
		   개행을 하도록 한다.
		 */
		token = strtok(buf, " ,\t,\n");
		token_check=0;
		while( token != NULL ) {
			if( !strcmp(token, "nodev") )
				token_check = 1;

			if( !token_check ) {
				if(count > 0 && count < newline_num ) // 쉼표를 적절한 위치에 출력 
					printf(", ");
				printf("%s", token);
				count++;
			}
			token = strtok(NULL, " ,\t,\n");
		}

		if( count >= newline_num ) { 
			printf("\n");
			count=0;
		}
	}
	if(count)
		printf("\n");
}

