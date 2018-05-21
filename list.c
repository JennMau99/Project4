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
	
	make_tree(argv[2], mode, argc, argv);


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

int valid(header *head)
{
        unsigned char *ptr;
        int i;
        int sum = 0;
	char checksum[8];

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

	if(strlen(head->chksum) == 7)
	{
        	sprintf(checksum, "%07o", sum);
	
	}
	else if(strlen(head->chksum) == 6)
	{
		sprintf(checksum, "%06o", sum);
	
	}
	/*printf("\n%s %lu\n", checksum, strlen(checksum));
	*/if(strcmp(checksum, head->chksum) == 0)
		return 1;
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
	int headerokay = 0;

	fd = open(tar, O_RDONLY);
			
	
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
	
		
		/*if(v == 0)
		{
			fprintf(stderr, "usage!");
			return;
		}*/



		if(included == 1 && strncmp(head.magic, "ustar", strlen("ustar")) == 0)
		{
			headerokay = 1;
			v = valid(&head);
			if(v == 0)
			{
				fprintf(stderr, "corrupt tar file");
				return;

			}
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
			/*printf("%s %lu", head.chksum, strlen(head.chksum));
			*/
		}
			
	}
	close(fd); 	
	if(headerokay == 0)
	{
		fprintf(stderr, "corrupt tar file!");
		return 1;
	}
	return 0;


}


