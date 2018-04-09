#include "header.h"

void option_t(char *path);
void parsing(int *ffd, char *argv[], int argc);
void State(char *buf, char c);
void option_c(char *path);
void exec(int option);
void manual_contents(char *buf, char *str);
void option_m(char *path);
void get_option_string(char *argv[]);
void option_w(char *path);
void option_l(char *path);
void option_s(char *path);
void option_f(char *path);
void option_v(char *path);
void option_e(char *path);
void s_exec(int k);
void option_n(char *path);
int cmp(const void *a, const void *b);

int ok=0, os=0, tos=0, or=0, oo=0; 

int i=0;
int arg=0;
int ffd[17], l=0;
int option_count=0, num_k=0, num_s=0, my_uid;
char k_str[20][50];
char s_str[20][50];
int uid_in_t;
char *fname;

int main(int argc, char *argv[])
{
	/* 
	   메인함수의 주 기능은 커맨드라인 인자로 입력된 옵션과 key와 pid들을 식별하고 따로 구분하여
	   각각 배열에 저장하는 것이다. 후에 그 배열들을 이용하여 옵션에 따른 적절한 실행을 한다.
	 */
	const char option_list[10] = {'f', 't', 'c', 'n', 'm', 'w', 'e', 'l', 'v', 's'};
	arg = argc;
	int m_position = 100;

	puts(">: ssu_lsproc start. :<"); // 프로세스의 시작 메시지를 출력한다.
	if( argc < 2 ) {
		// 입력된 옵션이 없으면 바로 종료한다.
		puts(">: ssu_lsproc terminered. :<");
		exit(0);
	}

	for(int k=1; k<argc; k++) {
		if(!strcmp(argv[k], "-r")) // r옵션이 설정되어 있는지 검사한다.
			or = 1;

		// k와 o옵션을 제외하고 입력된 옵션의 개수를 센다. 
		// 이 숫자는 각 옵션에서 구분 메시지 출력 여부를 결정한다.
		if ( argv[k][0] == '-' && argv[k][1] != 'k' && argv[k][1] != 'o' )
			option_count++;

		if (!strcmp(argv[k], "-k") && m_position != option_count) {
			/* k옵션이 m옵션 바로 다음에 나오는지를 검사하기 위해 m옵션이 나오는 위치와 k옵션이
			   나오는 위치를 비교한다.
			   만약 k옵션이 m옵션 바로 다음에 나오지 않는다면, 오류 메시지와 종료 메시지를 출력하고
			   종료한다.
			 */
			fprintf(stderr, "must option \"m\" -> option \"k\".\n");
			printf(">: ssu_lsproc terminated. :<\n");
			exit(0);
		}

		if ( !strcmp(argv[k], "-m") ) 
			m_position = option_count;
	}

	for(int p=1; p < argc; p++) {
		/* 인자로 key를 받는 옵션들의 key를 따로 저장하기 위한 코드이다.
		   커맨드라인 인자들을 하나씩 검사하면서 k나 s옵션이 있는지 검사하고 있다면 각각의 key들을
		   get_option_string()을 이용하여 k_str배열과 s_str배열에 저장한다.
		 */
		if( !strcmp(argv[p], "-k") || !strcmp(argv[p], "-s"))
			get_option_string(argv);

		else if (!strcmp(argv[p], "-o")) {
			/* o옵션이 있다면, 표준 출력을 옵션의 인자로 지정된 파일로 재지정한다.
			   파일 이름은 옵션 바로 다음에 나와야 하며, 오류 시 재지정에 실패하면
			   오류 메시지를 출력하고 종료한다.
			 */
			fname = argv[p+1];
			oo = 1;
			if(freopen(fname, "w", stdout) == NULL) {
				perror(fname);
				exit(1);
			}
			exec(11); // 재지정에 성공하면 성공 메시지를 출력하기 위해 exec()함수를 호출한다.
		}
	}


	for(int k=0; k < 10; k++) {
		/* k, r, o옵션을 제외한 10개의 옵션 중에 사용자가 입력한 옵션이 있는지 검사하여,
		   명세된 순서대로 그 옵션의 인자로 입력된 pid를 parsing()을 통해 ffd배열에 저장하고 
		   그 옵션을 실행하는 exec() 함수를 호출한다.
		 */
		for(i=1; i < argc; i++) {
			if( argv[i][0] == '-' && argv[i][1] == option_list[k] ) {
				parsing(ffd, argv, argc);
				exec(k+1);
				break;
			}
		}
	}

	/* 모든 실행이 끝나면 종료 메시지를 출력한다. 이때 o옵션에 의해 표준 출력이 바뀔 수도 있으므로
	   표준 에러를 통해 출력한다.
	 */
	fprintf(stderr, ">: ssu_lsproc terminated. :<\n");

	exit(0);
}

void option_w(char *path)
{
	// w 옵션의 기능을 구현한 함수이다.
	char pathname[MAX];
	char buf[MAX];
	// sprintf()를 이용하여 경로 이름을 만든다.
	sprintf(pathname, "%s/interrupts", path);
	if( option_count > 1 ) {
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
		// 파일을 읽기 모드로 연다. 실패시 오류메시지를 띄우고 종료한다.

		fprintf(stderr, "%s can't be read\n", pathname);
		return;
	}
	char *token;
	int num_cpu=0;
	/* interrupts 파일의 첫 줄에 적힌 cpu의 개수를 nun_cpu에 저장한다.*/
	fgets(buf, sizeof(buf), fp);
	token = strtok(buf, " ,\n");
	while(token != NULL) {
		num_cpu++;
		token = strtok(NULL, " ,\n");
	}

	printf("---Number of CPUs : %d---\n", num_cpu);
	printf("      [Average] : [Description]\n");

	int p=0, sum=0;
	while( fgets(buf, sizeof(buf), fp) != NULL) {
		/* 파일에 있는 내용을 한줄 씩 읽어 buf에 저장한다. 
		   buf가 공백이 아닌 부분에서 첫글자가 숫자가 아니면,
		   그 buf를 strtok()로 공백, 쉼표, 콜론 단위로 나누고 cpu의 개수만큼 그 값들을 sum에 저장한다.
		   각 속성에 대한 합계가 저장된 sum을 cpu의 개수로 나누어 소수점 셋째자리까지 출력한다.
		 */
		while( buf[p] == ' ' )
			p++;

		if( !isdigit(buf[p]) ) {
			int q=0;
			char *token[30];
			token[q] = strtok(buf, " ,:");
			while( token[q] != NULL ) {
				token[q][strlen(token[q])] = 0;
				token[++q] = strtok(NULL, " ,\n");
			}

			for(int e = 1; e <= num_cpu; e++) {
				sum += atoi(token[e]);
			}
			float avg = (float)sum / (float)num_cpu;
			printf("%15.3f : <%s> ", avg, token[0]);
			for(int z=num_cpu+1; z < q; z++)
				printf("%s ", token[z]);
			printf("\n");
			sum=0;
		}
	}
}

void option_m(char *path)
{
	char pathname[MAX];
	char buffer[20];

	for(int k=0; k<l; k++) {
		// 사용자가 입력한 pid를 이용하여 environ 경로를 만든다.
		sprintf(pathname, "%s/%d/environ", path, ffd[k]);
		if( ( !os && (option_count > 1 || l > 1)) || (os && option_count > 1)) {
			// 옵션 s의 설정여부에 따라 출력 메시지가 달라진다.
			printf("([%s])\n", pathname);
		}
		if(os)
			printf("## Attribut : ENVIRON, Target Process ID : %d ##\n", ffd[k]);
		FILE *fp;

		if (access(pathname, F_OK) < 0) {
			// 디렉터리의 접근 권한을 검사한다.
			if ( errno == ENOENT )
				fprintf(stderr, "%s doesn't exist.\n", pathname);
			else if (errno == EACCES)
				fprintf(stderr, "%s can't be read.\n", pathname);
			continue;
		}

		if( (fp = fopen(pathname, "r")) == NULL ) {
			fprintf(stderr, "%s can't be read\n", pathname);
			continue;
		}
		int c;
		if(ok == 0) {
			/* 종속 옵션 k가 설정되어있지 않은 경우 파일을 한 글자씩 파일의 끝까지 읽고 
			   만약 글자가 널이라면 개행하고 널이라니라면 출력한다.
			 */
			while( (c = fgetc(fp)) != EOF) {

				if ( c == '\0' ) {
					printf("\n");
				}
				else
					printf("%c", c);
			}
		}
		else {
			int p=0, count = 0;
			// 종속 옵션k가 설정되어있을 경우
			while( (c = fgetc(fp)) != EOF) {
				/* 파일의 내용을 파일 끝까지 한 글자씩 읽는다. 만약 널을 만나면 
				   옵션 r의 설정 여부에따라 처리가 달라진다. 
				 */
				if (c == '\0') {
					buffer[p] = 0;
					if (!or) {
						/* 옵션 r이 설정되어있지 않은 경우 사용자가 k 옵션의 인자로 입력한
						   key값들 중에 buffer에 저장된 값과 일치하는 경우가 있는지 검사한다.
						 */
						for (int k=0; k < num_k; k++)
							if (!strncmp(buffer, k_str[k], strlen(k_str[k]))) {
								printf("%s\n", buffer);
							}
					}
					else {
						/* 옵션 r이 설정되어있는 경우 key를 순서대로 하나씩 검사하고
						   찾으면 파일의 처음으로 되돌아 가서 다음 key를 검사
						 */
						if (!strncmp(buffer, k_str[count], strlen(k_str[count]))) {
							printf("%s\n", buffer);
							count++;
							fseek(fp, 0, 0);
							if (count == num_k)
								break;
						}

					}

					memset(buffer, 0, sizeof(buffer));
					p=0;
				}
				else {
					// 읽은 무자가 널이 아니라면 buffer에 저장.
					buffer[p++] = c;
				}
			}
		}
	}
}

void option_c(char *path)
{
	/* 옵션 c의 기능을 구현하는 함수이다*/
	char pathname[MAX];
	char buf[MAX];
	int c;
	for(int k=0; k<l; k++) {
		int count=0;
		int len=0;
		// 주어진 pid를 이용하여 cmdline에 해당하는 경로를 만든다.
		sprintf(pathname, "%s/%d/cmdline", path, ffd[k]);
		// 옵션 s의 설정 여부에 따라 출력 메시지가 달라진다.
		if( ( !os && (option_count > 1 || l > 1)) || (os && option_count > 1)) {
			printf("([%s])\n", pathname);
		}
		if(os)
			printf("## Attribut : CMDLINE, Target Process ID : %d ##\n", ffd[k]);
		FILE *fp;

		if (access(pathname, F_OK) < 0) {
			// 파일의 접근권한 검사를 한다.
			if ( errno == ENOENT )
				fprintf(stderr, "%s doesn't exist.\n", pathname);
			else if (errno == EACCES)
				fprintf(stderr, "%s can't be read.\n", pathname);
			continue;
		}

		if( (fp = fopen(pathname, "r")) == NULL ) {
			fprintf(stderr, "%s can't be read\n", pathname);
			continue;
		}
		while((c =  fgetc(fp)) != EOF) {
			/* 파일의 내용을 fgetc()를 이용하여 한문자씩 파일의 끝까지 읽는다. 
			   문자가 널문자이면 buf에 저장된 내용을 출력하는데 이때, buf의 길이는 0이 아니고 
			   첫문자가 널이 아니어어야 한다.
			   반대로 문자가 널이 아니라면 buf에 저장한다.
			 */
			buf[len] = 0;
			if (c == '\0' && len != 0 && buf[0] != 0) {
				printf("argv[%d] = %s\n", count++, buf);
				memset(buf, '\0', strlen(buf));
				len=0;
			}
			else {
				buf[len++] = c;
			}
		}
	}
}

void option_n(char *path)
{
	char pathname[MAX];
	char buf[MAX];
	/* 옵션 n의 기능을 구현하는 함수이다. 주어진 pid를 이용하여 io경로를 만들고 그 경로에 해당하는
	   파일을 읽기모드로 열고 fgets()로 한 줄 씩 읽으면서 strtok()로 토큰한다. 토큰 한 속성값을 이용하여
	   manual_contents()를 통해 해당되는 문자열을 출력하고 다음 토큰인 그 값을 이어 출력한다.
	 */
	for(int k=0; k<l; k++) {
		sprintf(pathname, "%s/%d/io", path, ffd[k]);
		if( ( !os && (option_count > 1 || l > 1)) || (os && option_count > 1)) {
			printf("([%s])\n", pathname);
		}
		if(os)
			printf("## Attribut : IO, Target Process ID : %d ##\n", ffd[k]);
		FILE *fp;

		if (access(pathname, F_OK) < 0) {
			if ( errno == ENOENT )
				fprintf(stderr, "%s doesn't exist.\n", pathname);
			else if (errno == EACCES)
				fprintf(stderr, "%s can't be read.\n", pathname);
			continue;
		}

		if( (fp = fopen(pathname, "r")) == NULL ) {
			fprintf(stderr, "%s can't be read\n", pathname);
			continue;
		}
		while( fgets(buf, sizeof(buf), fp) != NULL) {
			// 한 문자씩 읽어 문자열을 분리하여 출력한다.
			char *token = strtok(buf, " ,\t\n,:");
			char buffer[50];
			manual_contents(buffer, token);
			printf("%s", buffer);
			while(token != NULL) {	
				token = strtok(NULL, " ,\t,\n,:");
				printf("%s\n", token);
				break;
			}
		}
	}
}

void option_t(char *path)
{
	// 옵션 t의 기능을 구현하는 함수이다. 
	char pathname[MAX];
	char buf[MAX];
	for(int k=0; k<l; k++) {
		// 옵션의 인자로 주어진 pid들을 이용하여 경로명을 만든다.
		sprintf(pathname, "%s/%d/status", path, ffd[k]);
		/* 옵션 t는 옵션 s가 설정되어 있을 때, 각 사용자 프로세스들의 uid를 얻는데에도 이용되기
		   때문에 상황에 따른 출력메시지와 이용 방법이 다르다.*/
		if(option_count > 1 || l > 1) {
			// 옵션 s가 설정되어있지 않을 경우.
			if(!os && !tos)
				printf("([%s])\n", pathname);
		}
		// 옵션 s가 설정되어있고, uid 정보를 얻는 용도가 아닌 출력하는 용도로 사용될 경우.
		if(!os && tos) {
			if (option_count > 1)
				printf("([%s])\n", pathname);
			printf("## Attribute : STAT, Target Process ID : %d\n", ffd[k]);
		}

		FILE *fp;

		// 파일의 접근 권한을 검사한다.
		if (access(pathname, F_OK) < 0) {
			if ( errno == ENOENT )
				fprintf(stderr, "%s doesn't exist.\n", pathname);
			else if (errno == EACCES)
				fprintf(stderr, "%s can't be read.\n", pathname);
			continue;
		}

		/* 파일을 읽기모드로 열고 파일안에 내용을 한줄 씩 읽어 속성과 값으로 나눈다음
		   주어진 형식에 맞게 출력한다
		 */
		if( (fp = fopen(pathname, "r")) == NULL ) {
			fprintf(stderr, "%s can't be read\n", pathname);
			continue;
		}
		while( fgets(buf, sizeof(buf), fp) != NULL) {
			/* 파일 내용의 한줄을 저장한 buf를 strtok()로 띄어쓰기나 탭 단위로 나누고 옵션 s가 설정되지
			   않은 경우 속성으로 FDSize가 나오기 전까지 출력한다. 옵션 s가 설정되어있다면, 아무것도
			   출력하지 않고 속성 Uid가 나오면 그 값만을 가지고 종료한다.
			 */
			char *token = strtok(buf, " ,\t\n");
			if( !strcmp(buf, "FDSize:") ) break;
			while(token != NULL) {
				if(!os)
					printf("%s ", token);

				// 옵션s가 설정되어 있을 시 각 pid마다의 uid를 가져온다.
				if(os && !strcmp(token, "Uid:")) {
					uid_in_t = atoi(strtok(NULL, " ,\t,\n,:"));
					return;
				}

				if( !strcmp(buf, "State:") ) {
					/* 속성으로 State가 나오면 그 값을 이용하여 State()함수를 호출하여 문장으로 바꾼다.
					 */
					token = strtok(NULL, " ,\t,\n,:");
					char str[100];
					State(str, token[0]);
					if(!os)
						printf("%s\n", str);
					break;
				}
				// s옵션이 설정되어있지 않을 경우 속성 다음 토큰인 값을 출력한다.
				token = strtok(NULL, " ,\t,\n,:");
				if(!os)
					printf("%s\n", token);
				break;
			}
		}
	}
}

int cmp(const void *a, const void *b)
{
	// pid를 qsort하는데 사용하는 비교함수이다.
	return *(int *)a - *(int *)b;
}

void parsing(int *ffd, char *argv[], int argc)
{
	/* 옵션에 따른 pid를 파싱하여 ffd 배열에 정수형태로 저장하는 함수이다.*/
	l=0;
	memset(ffd, 0, sizeof(ffd));

	if(argc <= ++i) {
		return;
	}
	while((48 <= argv[i][0]) && (argv[i][0] <= 57)) {
		ffd[l++] = atoi(argv[i++]);
		if(i >= argc)
			break;
	}

	if (l > 16) {
		// 옵션 속한 pid나 key의 개수가 16개를 초과할 때 오류 메시지를 띄움
		for(int k=16; k < l; k++)
			printf("Maximum Number of Argument Exceeded. :: %d\n", ffd[k]);
		l = 16;
	}

	// r옵션이 설정되어 있으면, qsort를 이용하여 오름차순 정렬한다.
	if(or)
		qsort(ffd, l, sizeof(*ffd), cmp);

	i--;
}

void manual_contents(char *buf, char *str)
{
	/*io 항목의 기존 내용(str)을 매뉴얼 관련 내용(buf)으로 바꾸어주는 함수 */
	if (!strcmp(str, "rchar")){
		strcpy(buf, "Characters read : ");
	}
	else if (!strcmp(str, "wchar")){
		strcpy(buf, "Charaters written : ");
	}
	else if (!strcmp(str, "syscr")){
		strcpy(buf, "Read syscalls : ");
	}
	else if (!strcmp(str, "syscw")){
		strcpy(buf, "Write syscalls : ");
	}
	else if (!strcmp(str, "read_bytes")){
		strcpy(buf, "Bytes read : ");
	}
	else if (!strcmp(str, "write_bytes")){
		strcpy(buf, "Bytes written : ");
	}
	else if (!strcmp(str, "cancelled_write_bytes")){
		strcpy(buf, "Cancelled write bytes : ");
	}
}

void State(char *buf, char c) 
{
	/* t옵션에서 State의 값을 알파벳에서 문장으로 바꾸어주는 함수이다. buf에 알파벳에 매칭되는
	   문장을 저장하여 buf의 값을 출력한다.
	 */
	switch(c) {
		case 'R' :
			strcpy(buf, "Running");
			break;
		case 'S' :
			strcpy(buf, "Sleeping in an interruptible wait");
			break;
		case 'D' :
			strcpy(buf, "Waiting in uninterruptible disk sleep");
			break;
		case 'Z' :
			strcpy(buf, "Zombie");
			break;
		case 'T' :
			strcpy(buf, "Stopped (on a signal) or trace stopped");
			break;
		case 't' :
			strcpy(buf, "Tracing stop");
			break;
		case '\\' :
			strcpy(buf, "Paging");
			break;
		case 'X' :
			strcpy(buf, "Dead");
			break;
		case 'x' :
			strcpy(buf, "Dead");
		case 'K' :
			strcpy(buf, "Wakekill");
			break;
		case 'W' :
			strcpy(buf, "Waking");
			break;
		case 'P' :
			strcpy(buf, "Parked");
			break;
		default :
			break;
	}
}

void exec(int option) {
	/* 옵션에 따른 함수를 실행하는 함수 */
	char *path = "/proc";
	int status;

	int pid = fork();
	if( pid == 0 ){
		// 생성된 자식프로세스가 옵션에 따라 각 기능을 실행하는 함수를 호출한다.
		if( l == 0 )
			ffd[l++] = getpid();
		if ( option == 1 )
			option_f(path);
		else if( option == 2)
			option_t(path);
		else if( option == 3)
			option_c(path);
		else if( option == 4)
			option_n(path);
		else if( option == 5)
			option_m(path);
		else if( option == 6)
			option_w(path);
		else if( option == 7)
			option_e(path);
		else if( option == 8)
			option_l(path);
		else if( option == 9)
			option_v(path);
		else if( option == 10)
			option_s(path);
		// o옵션은 자식프로세스를 생성하여 재지정된 출력에 메시지만 출력하고 종료한다.
		else if( option == 11)
			printf("!--Successfully Redirected : %s--!\n", fname);
		exit(1);
	}else
		wait(&status); // 부모프로세스는 자식프로세스가 끝날때까지 기다린다.
}

int string_cmp(const void *a, const void *b)
{
	// 문자열을 오름차순 정렬하는 qsort에 사용하는 함수
	return strcmp((char *)a, (char *)b);
}

void get_option_string(char *argv[])
{
	int k, p;
	// k 옵션의 인자로 사용되는 문자열을 parsing한다.
	if(ok == 0) {
		/* m의 종속옵션 k옵션을 key값들을 k_str배열에 저장하고 key값들의 개수를 num_k에 저장한다.
		   옵션 k가 여러번 입력되어도 한번만 실행된다.
		 */
		for(k=1; k<arg; k++) {
			if( !strcmp(argv[k], "-k") ) {
				ok = 1;
				break;
			}
		}

		for(p = k+1; p < arg; p++) {
			if( argv[p][0] == '-' )
				break;
			strcpy(k_str[num_k], argv[p]);
			k_str[num_k][strlen(argv[p])] = 0;
			num_k++;
		}
		if (num_k > 16) {
			/* k옵션의 인자가 16개를 넘으면, 초과 메시지를 출력하고 16개 까지만 출력되도록 한다. */
			for(int k = 16; k < num_k; k++)
				printf("Maximum Number of Argument Exceeded. :: %s\n", k_str[k]);
			num_k = 16;
		}
		// r옵션이 설정되면 key값들을 오름차순으로 정렬한다.
		if(or) 
			qsort(k_str, num_k, sizeof(*k_str), string_cmp);

		// 옵션 k가 설정되었는데 key값이 하나도 주어지지 않았다면, 옵션 k가 설정되지 않은 걸로 설정한다.
		if( num_k == 0 ) 
			ok = 0;
	}

	// s 옵션의 인자로 사용되는 문자열을 parsing한다.
	if(num_s == 0) {
		/* 옵션 s가 두번 이상 입력되어도 한번만 실행되도록 하며, 옵션 s다음에 오는 key값들을
		   s_str배열에 저장하고 key값들의 개수를 num_s에 저장한다.
		 */
		for(k=1; k<arg; k++) {
			if( !strcmp(argv[k], "-s") )
				break;
		}

		for(p = k+1; p < arg; p++) {
			// 커맨드라인 인자에서 -s 다음에 오는 인자들을 다음 옵션이 올때까지 s_str에 저장한다.
			if( argv[p][0] == '-')
				break;
			strcpy(s_str[num_s], argv[p]);
			s_str[num_s][strlen(argv[p])] = 0;
			num_s++;
		}
	}
}

