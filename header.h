#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <memory.h>
#include <unistd.h>
#include <dirent.h>

#define MAX 256


extern int ok, os, tos, or, oo;

extern int i;
extern int arg;
extern int ffd[17], l;
extern int option_count, num_k, num_s, my_uid;
extern char k_str[20][50];
extern char s_str[20][50];
extern int uid_in_t;
extern char *fname;

