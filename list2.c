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

struct Node {
	char permissions[10];
	char owner[17];
	char size[8];
	char mtime[16];
	char name[100];
	/*struct Node nodes[40];
*/};
typedef struct Node node;
int make_tree(char *tar);

int main(int argc, char **argv)
{

	 make_tree(argv[1]);


	return 0;
}


int make_tree(char *tar)
{
	int fd;
	header head;

	unsigned int mode;
	char buff[20];
	char *groupuser;
	time_t sec;
	fd = open(tar, O_RDONLY);
	
	
	while((read(fd, &head, 512)) > 0)
	{
		if(strcmp(head.magic, "ustar  ") == 0)
		{
			mode = strtol(head.mode, '\0', 8);
			/*print permissions	*/	
			printf( (S_ISDIR(mode)) ? "d" : "-");
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

			/*print name*/
                        printf("%s",head.name);
			printf("\n");

		}
	}
	close(fd); 	
	return 0;


}


