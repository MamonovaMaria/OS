#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>


void OpenDir(char *, char *, int);
void FileCopy(char *, char *);
void CreatDestination(char *);
pid_t CreatFork(char *);
void SigHandler(int, int);

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		printf("incorrect input");
		exit(-1);
	}

	if(mkdir(argv[2], 0777) != -1)
	{
		printf("this directory does not exist, or the path is incorrect\n");
		rmdir(argv[2]);
		exit(-1);
	}
	else
	{
		pid_t p = CreatFork(argv[1]);
		if(!p)
		{
			int i = 0;
			OpenDir(argv[2], argv[3], i);
			sleep(1);
		}
		return 0;
	}
}

pid_t CreatFork(char *m)
{
	int n = atoi(m);
	pid_t pid = fork();

	for(int i = 0; i < n-1; i++)
	{
		if(pid != 0)
			fork();
	}
	return pid;
}

void CreatDestination(char *dest)
{
	char d_name[256];
	strcpy(d_name, dest);

	char *tmp = NULL;

	size_t d_size = strlen(d_name);

	if(d_name[d_size - 1] == '/')
		d_name[d_size - 1] = 0;
	for(tmp = d_name + 1; *tmp; tmp++)
		if(*tmp == '/') 
		{
			*tmp = 0;
			mkdir(d_name, 0777);
			*tmp = '/';
		}
	mkdir(d_name, 0777);
}

void OpenDir(char *dir1, char *dir2, int i)
{
	signal(SIGINT,  SigHandler(SIGINT, i));

	DIR *dr1 = opendir(dir1);

	char dir1_name[256];
	char dir2_name[256];
	char dr1_n[256];
	char dr2_n[256];

	strcpy(dir1_name, dir1);
	strcat(dir1_name,"/");

	strcpy(dir2_name, dir2);
	strcat(dir2_name,"/");

	if(!opendir(dir2))
		CreatDestination(dir2);
	DIR *dr2 = opendir(dir2);

	struct dirent *pre;

	while((pre = readdir(dr1))!= NULL)
	{
		if ((strcmp(pre->d_name, "..") != 0) && (strcmp(pre->d_name, ".") != 0))
		{
			strcpy(dr1_n,dir1_name);
			strcpy(dr2_n,dir2_name);

			strcat(dr1_n,pre->d_name);
			strcat(dr2_n,pre->d_name);

			if(pre->d_type != DT_DIR)
			{
				FileCopy(dr1_n, dr2_n);
				i++;
			}
			else
				OpenDir(dr1_n, dr2_n, i);

		}
	}
}

void FileCopy(char *dir1, char *dir2)
{
	int f1, f2;
	int n;
	char b[BUFSIZ];

	if ((f1 = open(dir1, O_RDONLY, 0)) == -1)
	{
		printf("cannot open file ");
		exit(-1);
	}

	if ((f2 = creat(dir2, 0666)) == -1)
	{
		printf("cannot make file");
		exit(-1);
	}
	while ((n = read(f1, b, BUFSIZ)) > 0)
	{
		if (write(f2, b, n) != n)
		{
			printf("error writing to file ");
			exit(-1);
		}
	}
}

void SigHandler(int sig, int n) 
{
        printf("PID %d\n",  getpid());
		//CopyInfo();
		printf("File copy:  %d\n",  n);
		signal(SIGINT, SIG_DFL);
}

//void CopyInfo()
//{
//
//	printf("File copy:  %d\n",  n);
//}
