/*RICHA GADGIL SECTION 03-357 TAYLOR*/
#define _GNU_SOURCE
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
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <dirent.h>
#include <arpa/inet.h>
header* make_header(char *file);
int filewriter(char *tar, char *file);
void write_file(char * file, char* tar);
void get_checksum(header *head);
int char_total(char *name);
int checksum;
void listdir(char *tar, char *name, int indent);
char uid[8];
int verbose;
void header_set_uid_bigsafe(char* buf, int32_t uid);
int header_insert_special_int(char* where, int size, int32_t val);

int create(int argc, char **argv)
{
	int i = 0;
	int fd = 0;
	char buff[512];
	struct stat st;
	char * buffer;
 	int indent = 0;	
	/*In this implementation, argc and argv v from main are passed to create*/

	for(i = 0; i < strlen(argv[1]); i++)
        {
                if(argv[1][i] == 'v')
                        verbose = 1;
        }

	/*Jennifer -- the first open here opens/creates the tar file, change the arg number as necesary*/
	/*make tar file*/
	fd = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0700);
	if(fd  < 0)
	{
		fprintf(stderr, "File could not be opened");
		return 0;
	}
	close(fd);

	i = 3;
	/*then the rest of the args are cycled through (remaining files)*/
	while(i < argc)
	{
		stat(argv[i], &st);
	   	if(S_ISDIR(st.st_mode))
        	{
			buffer = (char *)malloc((strlen(argv[i]) + 1) * sizeof(char)); 
                	strcpy(buffer, argv[i]);
			strcat(buffer, "/");
			filewriter(argv[2], buffer);
        		free(buffer);
		}	
		listdir(argv[2], argv[i], indent);
		i++;
	}
	
	memset(&buff, 0, 512);
	
	fd = open(argv[2], O_WRONLY | O_APPEND);
	if(fd < 0)
	{       
                fprintf(stderr, "File could not be opened");
                return 0;
        }

	/*ending buffers*/
	write(fd, &buff, 512);
	write(fd, &buff, 512);

	close(fd);
	return 1;
}


void listdir(char *tar, char *name, int indent)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
    {
	filewriter(tar, name);
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            char dash[1024];
	    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            sprintf(path, "%s/%s", name, entry->d_name);
	    /*printf("%s/\n",path);
	    */sprintf(dash, "%s/", path);
	    filewriter(tar, dash);
	    listdir(tar, path, indent + 2);
        } else {
	    char path[1024];
	    sprintf(path, "%s/%s", name, entry->d_name);
	    /*printf("%s\n", path); 
	    */filewriter(tar, path);
        }
    }
    closedir(dir);
}


int filewriter(char *tar, char *file)
{
	
	int taropen;
	header* head;
	struct stat st;
	

	if(verbose == 1)
		printf("%s\n", file);
	taropen = open(tar, O_APPEND | O_WRONLY, 0700);
	if(taropen < 0)
	{       
                fprintf(stderr, "File could not be opened");
                return 0;
        }


	if(stat(file, &st) < 0)
	{
		fprintf(stderr, "file could not be read.");
		return 0;
	}
	
	head = make_header(file);
	
	get_checksum(head);

	write(taropen, head, 512);

	close(taropen);

	free(head);

	stat(file, &st);
	if(!S_ISDIR(st.st_mode))
		write_file(file, tar);



	return 1;

}
		

header* make_header(char * file)
{
	struct stat st;
	header *head;
	struct group *grp;
        struct passwd *pwd;
	int i;
	int stop;
	int count = 0;
	head = (header*)calloc(1, sizeof(header));
	stat(file, &st);
	/*name -- clear memset, check prefix later*/
        

	for(i = strlen(file); i >= 0; i--)
	{
		count++;
		if(count > 100)
			break;
		if(file[i] == '/')
		{
			stop = i;
		}
	}

		

	if(strlen(file) > 100)
	{
		strncpy(head->prefix, file, stop);
		strncpy(head->name, file + (stop+1), strlen(file));
	}
	else
	{
		strcpy(head->name, file);
	}

	
        /*mode*/
        sprintf(head->mode, "%07o", (st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)));

        /*UID
        sprintf(uid, "%07o", (unsigned short)st.st_uid);
	
	
	insert_special_int(head->uid, 8, (uint32_t)st.st_uid);
	*/

	header_set_uid_bigsafe(head->uid, st.st_uid);
	

	/*GID*/
        sprintf(head->gid, "%07o", (unsigned int)st.st_gid);

        /*size
        sprintf(head->size, "%011o", (int)st.st_size);
	*/
	strcpy(head->chksum, "       ");

        /*mtime*/
        sprintf(head->mtime, "%011o", (int)st.st_mtime);

        /*typeflag*/
        if(S_ISREG(st.st_mode))
        {
		head->typeflag = '0';
        	sprintf(head->size, "%011o", (int)st.st_size);
	}
        else if(S_ISDIR(st.st_mode))
        {
                head->typeflag = '5';
        	sprintf(head->size, "%011o", 0);

	}

        /*linkname if symbolic link -- memset?*/


        /*magic*/
        strcpy(head->magic, "ustar");
	head->magic[6] = '0';
	head->magic[7] = '0';


        /*uname and gname? */
	pwd = getpwuid(st.st_uid);
	strcpy(head->uname, pwd->pw_name);

	grp = getgrgid(st.st_gid);
        strcpy(head->gname, grp->gr_name);
        

	/*devmajor and devminor unknown*/
	
	return head;
}

void get_checksum(header *head)
{
	unsigned char *ptr;
	int i;
	int sum = 0;


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
	sprintf(head->chksum, "%07o", sum);
}



void header_set_uid_bigsafe(char* buf, int32_t uid)
{
    int toobig = 07777777;

    /* Tests if bitpacking is needed */
    if(uid > toobig)
    {
        header_insert_special_int(buf, 8, uid);
    }
    else
    {
	sprintf(buf, "%07o", uid);
        /* Do normal formatting as octal */
    }
}

int header_insert_special_int(char* where, int size, int32_t val)
{
    int err = 0;
    if (val < 0 || size < sizeof(val))
    {
        err++;
    }
    else
    {
        memset(where, 0, size);
        *(int32_t *)(where+size-sizeof(val)) = htonl(val);
        *where |= 0x80;
    }
    return err;
}

void write_file(char * file, char* tar)
{

        int fd;
        int fp;

        char buff[512];
        memset(buff, 0, 512);

        fd = open(file, O_RDONLY);
	if(fd < 0)
	{       
                fprintf(stderr, "File could not be opened");
                return;
        }
        fp = open(tar, O_WRONLY | O_APPEND);
	if(fp < 0)
	{       
                fprintf(stderr, "File could not be opened");
                return;
        }

        while(read(fd, &buff, 512) > 0)
        {
                write(fp, &buff, 512);
        }


	close(fd);
        close(fp);

}			
