#include "header.h"

void option_t(char *path);
void option_c(char *path);
void option_m(char *path);
void option_f(char *path);
void s_exec(int k); 
void option_n(char *path);

void option_s(char *path)
{
	// s 옵션의 key로 올 수 있는 값들을 저장하여 어떤 key가 입력되었는지 구분한다.
	const char attr_name[5][10] = {"FILEDES", "CMDLINE", "IO", "STAT", "ENVIRON"};
	my_uid = getuid(); // 현재 프로세스의 uid를 얻는다.

	DIR *dp;
	struct dirent *d;
	int count=0;
	int firstPID_inProc;
	os = 1;

	if ( access(path, F_OK) < 0 ) {
		fprintf(stderr, "%s doesn't exist.\n", path);
		return;
	}

	if ( (dp = opendir(path)) == NULL) {
		fprintf(stderr, "%s can't be read\n", path);
		return;
	}

	while( (d = readdir(dp)) != NULL ) {
		/* proc 디렉터이의 내용을 하나씩 읽으면서, 숫자인 디렉터리만 연다.
		   그 숫자는 pid이므로 ffd[0]에 저장하고, 옵션 t를 이용해서 그 pid에 저장된 uid를 얻는다.
		   얻어 온 uid가 위에서 얻은 현재 프로세스의 uid와 일치하면 그 pid를 ffd 배열에 0부터 저장한다.
		   이 때, ffd[0]은 계속 다른 pid를 위해 사용되므로 uid가 일치하는 처음 pid는 따로 firstPID_inProc에
		   저장해 두고 마지막에 ffd[0]에 한번 더 넣어준다.
		 */
		char *pid_proc_str = d->d_name;
		int pid_proc;
		if(isdigit(pid_proc_str[0])){
			pid_proc = atoi(pid_proc_str);
			ffd[0] = pid_proc;
			option_t(path);
			if(my_uid == uid_in_t){
				ffd[count++] = pid_proc;
			}
			if (count == 1)
				firstPID_inProc = ffd[0];
		}
	}

	l = count;
	ffd[0] = firstPID_inProc;

	/* 위에서 만든 pid가 들어있는 ffd 배열을 가지고 입력된 key에 따른 실행을 명시된 순서대로 시행한다. */
	for(int k=0; k < 5; k++) 
		for(int p=0; p < num_s; p++) 
			if(!strcmp(s_str[p], attr_name[k]))
				s_exec(k);

	os=0;
}

void s_exec(int k)
{
	// 옵션 s가 설정되어 있을 때 자식 프로세스를 실행 하지 않고 옵션 함수들을 실행하는 함수이다.
	char *path = "/proc";
	if(k==0) 
		option_f(path);
	else if(k==1)
		option_c(path);
	else if(k==2)
		option_n(path);
	else if(k==3) {
		/* 옵션 t는 각 프로세스들의 uid를 검사할 때도 사용하기 때문에 
		   언제 사용되는 것인지 구분할 필요가 있다. 따라서 uid를 얻기 위해 사용될 떄에는 os,
		   key로써 사용되어 질 때에는 tos라는 값을 통해 구분한다.
		 */
		os=0;
		tos=1;
		option_t(path);
		os=1;
	}
	else if(k==4)
		option_m(path);
}
