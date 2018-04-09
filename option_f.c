#include "header.h"

void option_f(char *path)
{
	DIR *dp;
	struct stat st;
	struct dirent *d;

	for(int k=0; k<l; k++) {
		// s옵션이 설정되어있을 때와 안되어있을 때의 출력메시지를 구별한다.
		if( ( !os && (option_count > 1 || l > 1)) || (os && option_count > 1)) {
			printf("([%s/%d/fd])\n", path, ffd[k]);
		}
		if(os)
			printf("## Attribut : FILEDES, Target Process ID : %d ##\n", ffd[k]);

		// f옵션에 필요한 경로를 만든다.
		char pathname[MAX];
		sprintf(pathname, "%s/%d/fd", path, ffd[k]);

		// 디렉터리의 접근 권한을 검사한다. 존재하지 않는 경우와 접근할 수 없는 경우의 메>시지를 출력한다.
		if ( access(pathname, F_OK) < 0 ) {
			fprintf(stderr, "%s doesn't exist.\n", pathname);
			continue;
		}

		if ( (dp = opendir(pathname)) == NULL) {
			fprintf(stderr, "%s can't be read\n", pathname);
			continue;
		}

		while( (d = readdir(dp)) != NULL) {
			/* 디렉터리의 접근권한 검사가 끝난 후 디렉터리를 읽는데, 파일이 링크파일이면 readlink()를
			   이용 해 경로명을 buffer에 저장하고 출력한다.*/
			if( !strcmp(d->d_name, ".") || !strcmp(d->d_name, "..") )
				continue;
			char buffer[MAX], tempPath[MAX];
			sprintf(tempPath, "%s/%s", pathname, d->d_name);
			lstat(tempPath, &st);
			if( S_ISLNK(st.st_mode) ){
				ssize_t length = readlink(tempPath, buffer, MAX);
				buffer[length] = 0;
			}
			printf("File Descriptor number: %s, Opened File: %s\n", d->d_name, buffer);
		}
	}
	return;
}
