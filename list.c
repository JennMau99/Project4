/*RICHA GADGIL SECTION 03-357 TAYLOR*/
#define _GNU_SOURCE 1
#define _POSIX_SOURCE
#define _USE_XOPEN
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "header.h"
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

int make_tree(char *tar, char verbose, int argc,  char **argv);

int list(int argc, char **argv)
{

	/*read implementation notes in create*/

	int i;
	int mode = 0;

	for(i = 0; i < strlen(argv[1]); i++)
	{
		if(argv[1][i] == 'v')
			mode = 1;
	}
	
	if (make_tree(argv[2], mode, argc, argv) < 0)
	{
		fprintf(stderr, "Bad tar file\n");
		return -1;
	}


	return 0;
}

int check_conditions(int argc, char **argv, char *name)
{

	int i = 0;

	for(i = 0; i < argc; i++)
	{
		if(strncmp(name, argv[i], strlen(argv[i])) == 0)
		{
			
			return 1;
		}
	}


	return 0;



}
/* This needs to be fixed */
int valid(int tar, header *head)
{
	char checksum[9];
	char givenchecksum[9];
	unsigned char *ptr;
	int i;
	int sum = 0;

	checksum[8] = '\0';
	givenchecksum[8] = '\0';

	ptr = (unsigned char *)head;

	for(i = 0; i < 512; i++)
	{
		if(i < 148 || i > 155)
		{
			sum += ptr[i];
		}
		else
		{
			sum += 32;
		}
	}

    sprintf(checksum, "%07o", sum);
	read(tar, &givenchecksum, 8);

	/*fprintf(stderr, "%s\n", checksum);
	fprintf(stderr, "%s\n", givenchecksum);
	*/
	if(strcmp(checksum, givenchecksum) == 0)
		return 1;
	return 1;

}

int checkifvalid(int tar)
{
	off_t offset = 0;
	header head;
	int v;	

	while (lseek(tar, offset, SEEK_SET) < (lseek(tar, 0, SEEK_END) - 1024))
	{
		read(tar, &head, 512);
		lseek(tar, offset+148, SEEK_SET);
		v = valid(tar, &head);
		if (v == 0)
			return -1;
		offset = findoffset(tar, offset);
	}
	
	return 0;
}


int make_tree(char *tar, char verbose, int argc, char **argv)
{
	int fd;
	header head;

	unsigned int mode;
	char buff[20];
	char *groupuser;
	time_t sec;
	int included = 1;
	char namebuff[256];		
	int v; 

	fd = open(tar, O_RDONLY);
			
	v = checkifvalid(fd);
	/*if (v < 0)
		return -1;
	*/
	lseek(fd, 0, SEEK_SET);
	while((read(fd, &head, 512)) > 0)
	{
		strcpy(namebuff, head.prefix);
		if(strlen(head.prefix) != 0)
		{
			strcat(namebuff, "/");
		}
		strcat(namebuff, head.name);

	 	if(argc > 3)
		{
			included = check_conditions(argc, argv, namebuff);
		}
	

		if(included == 1 && strncmp(head.magic, "ustar", strlen("ustar")) == 0)
		{
			if(verbose != 0)
			{
				mode = strtol(head.mode, '\0', 8);
				/*print permissions	*/	
				if(head.typeflag == '5')
					printf("d");
				else if(head.typeflag == '2')
					printf("l");
				else	
					printf("-");
    				printf( (mode & S_IRUSR) ? "r" : "-");
    				printf( (mode & S_IWUSR) ? "w" : "-");
    				printf( (mode & S_IXUSR) ? "x" : "-");
    				printf( (mode & S_IRGRP) ? "r" : "-");
    				printf( (mode & S_IWGRP) ? "w" : "-");
    				printf( (mode & S_IXGRP) ? "x" : "-");
    				printf( (mode & S_IROTH) ? "r" : "-");
    				printf( (mode & S_IWOTH) ? "w" : "-");
   				printf( (mode & S_IXOTH) ? "x" : "-");
	
			printf(" ");	
	
			/*print group and username*/	
			groupuser = malloc(strlen(head.gname) + strlen(head.uname) + 2);
			strcpy(groupuser, head.gname);
			strcat(groupuser, "/");
			strcat(groupuser, head.uname);			
			printf("%.17s", groupuser);
			free(groupuser);
		
			printf(" ");

			/*print size*/
			mode = strtol(head.size, '\0', 8);		
			printf("%8d", mode);
			
			printf(" ");

			/*work on getting time to work!!*/
			mode = strtol(head.mtime, '\0', 8);
			sec = (time_t)mode;
			strftime(buff, 20, "%Y-%m-%d %H:%M", localtime(&sec));	
			printf(buff);

			printf(" ");

			}
			/*print name*/
			if(strlen(head.prefix) == 0)		
			{
				printf("%.100s\n",head.name);
			}
			else 
			{
				printf("%s/",head.prefix);
				printf("%.100s\n",head.name);
			}
		}
			
	}
	close(fd); 	
	return 0;


}


