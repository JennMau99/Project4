/*RICHA GADGIL JENNIFER MAUNDER SECTION 03-357 TAYLOR*/
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

int make_tree(char *tar, int argc,  char **argv);
int standard;
int verbose;
void print_header(header *head);


int list(int argc, char **argv)
{

	/*read implementation notes in create*/

	int i;
	int mode = 0;

	for(i = 0; i < strlen(argv[1]); i++)
	{
		if(argv[1][i] == 'v')
			verbose = 1;
		if(argv[1][i] == 's')
			standard = 1;
	}
	
	if (make_tree(argv[2], argc, argv) < 0)
	{
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
		if(strcmp(checksum, head->chksum) == 0)
                	return 1;
	}
	else if(strlen(head->chksum) == 6)
	{
		sprintf(checksum, "%06o", sum);
		if(strncmp(checksum, head->chksum, 6) == 0)
                	return 1;
	}
	/*printf("\n%s %lu\n", checksum, strlen(checksum));
*/
	return 0;

}

int check_standard(header *head)
{

	if(standard == 0)
		return 1;
	else if(standard == 1)
	{
		if(strncmp(head->version, "00", strlen("00") == 0)) 
		{
			return 1;
		}
	}

	return 0;



}
int make_tree(char *tar, int argc, char **argv)
{
	int fd;
	header head;

	unsigned int mode;
	char buff[20];
	char *groupuser;
	time_t sec;
	int included = 1;
	int v; 
	int headerokay = 0;
	/*char *namebuff;
	*/

	char namebuff[1000];

	fd = open(tar, O_RDONLY);
	lseek(fd, 0, SEEK_SET);
	while((read(fd, &head, 512)) > 0)
	{

		

		if(strlen(head.prefix) != 0)	
		{
			sprintf(namebuff, "%s/%s", head.prefix, head.name);
 		}
		else
		{
			sprintf(namebuff, "%s%s", head.prefix, head.name);
		}


		
	 	if(argc > 3)
		{
			included = check_conditions(argc, argv, namebuff);
		}

	


		if(check_standard(&head) == 1 && included == 1 && strncmp(head.magic, "ustar", strlen("ustar")) == 0)
		{
			headerokay = 1;
			print_header(&head);
		}
			
	}
	close(fd); 	
	if(headerokay == 0)
	{
		fprintf(stderr, "corrupt tar file!\n");
		return 1;
	}
	return 0;


}

void print_header(header *head)
{
        unsigned int mode;
        char buff[20];
        char *groupuser;
        time_t sec;
	int v;
	                v = valid(head);
                        if(v == 0)
                        {
                                fprintf(stderr, "corrupt tar file\n");
                                return;

                        }
                        if(verbose != 0)
                        {
                                mode = strtol(head->mode, '\0', 8);
                                /*print permissions     */
                                if(head->typeflag == '5')
                                        printf("d");
                                else if(head->typeflag == '2')
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
                        groupuser = malloc(strlen(head->gname) + strlen(head->uname) + 2);
                        strcpy(groupuser, head->gname);
                        strcat(groupuser, "/");
                        strcat(groupuser, head->uname);
                        printf("%.17s", groupuser);
                        free(groupuser);

                        printf(" ");

                        /*print size*/
                        mode = strtol(head->size, '\0', 8);
                        printf("%8d", mode);

                        printf(" ");

                        /*work on getting time to work!!*/
                        mode = strtol(head->mtime, '\0', 8);
                        sec = (time_t)mode;
                        strftime(buff, 20, "%Y-%m-%d %H:%M", localtime(&sec));
                        printf(buff);

                        printf(" ");

                        }
                        /*print name*/
                        if(strlen(head->prefix) == 0)
                        {
                                printf("%.100s\n",head->name);
                        }
                        else
                        {
                                printf("%s/",head->prefix);
                                printf("%.100s\n",head->name);
                        }
}


