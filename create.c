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
int insert_special_int(char *where, int size, int32_t val);
char uid[8];
int verbose;

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
			printf("%s\n", buffer);
        	}	
		listdir(argv[2], argv[i], indent);
		i++;
	}
	
	memset(&buff, 0, 512);
	
	fd = open(argv[2], O_WRONLY | O_APPEND);

	/*ending buffers*/
	write(fd, &buff, 512);
	write(fd, &buff, 512);

	close(fd);
	return 0;
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
		printf(file);
	taropen = open(tar, O_APPEND | O_WRONLY, 0700);
	
	head = make_header(file);
	
	get_checksum(head);

	write(taropen, head, 512);

	close(taropen);

	free(head);

	stat(file, &st);
	if(!S_ISDIR(st.st_mode))
		write_file(file, tar);



	return 0;

}
		

header* make_header(char * file)
{
	struct stat st;
	header *head;
	struct group *grp;
        struct passwd *pwd;
	head = (header*)calloc(1, sizeof(header));
	stat(file, &st);
	/*name -- clear memset, check prefix later*/
        strcpy(head->name, file);

        /*mode*/
        sprintf(head->mode, "%07o", (st.st_mode & 0x1F7FFF));

        /*UID*/
        sprintf(uid, "%07o", (unsigned short)st.st_uid);
	
	
	insert_special_int(head->uid, 8, (uint32_t)st.st_uid);
	/*GID*/
        sprintf(head->gid, "%07o", (unsigned int)st.st_gid);

        /*size*/
        sprintf(head->size, "%011o", (unsigned int)st.st_size);

	strcpy(head->chksum, "       ");

        /*mtime*/
        sprintf(head->mtime, "%011o", (int)st.st_mtime);

        /*typeflag*/
        if(S_ISREG(st.st_mode))
        {
		head->typeflag = '0';
        }
        else if(S_ISDIR(st.st_mode))
        {
                head->typeflag = '5';
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
	int checksum = 0;
	char *ptr;

	ptr = (char*)(head->name);
	checksum += char_total(ptr);

	ptr = (char*)(head->mode);
        checksum += char_total(ptr);

	ptr = (char*)(uid);
        checksum += char_total(ptr);

	ptr = (char*)(head->gid);
        checksum += char_total(ptr);

	ptr = (char*)(head->size);
        checksum += char_total(ptr);

	ptr = (char*)(head->mtime);
        checksum += char_total(ptr);

        checksum += (unsigned int)head->typeflag;

	ptr = (char*)(head->linkname);
	checksum+= char_total(ptr);

	ptr = (char*)(head->magic);
        checksum += char_total(ptr);

	ptr = (char*)(head->chksum);
	checksum += char_total(ptr);

	ptr = (char*)(head->uname);
        checksum += char_total(ptr);


	ptr = (char*)(head->gname);
        checksum += char_total(ptr);

	 ptr = (char*)(head->devmajor);
        checksum += char_total(ptr);

	 ptr = (char*)(head->devminor);
        checksum += char_total(ptr);

	checksum+=77;
	checksum+=96;
	sprintf(head->chksum, "%07o", checksum);
}	

int char_total(char *name)
{
	int i = 0;
	int total = 0;

	for(i = 0; name[i] != '\0'; i++)
	{
		total += (int)name[i];
	}

	return total;
}

int insert_special_int(char *where, int size, int32_t val) { /* For interoperability with GNU tar. GNU seems to
* set the high–order bit of the first byte, then
* * treat the rest of the field as a binary integer
* * in network byte order.
* * Insert the given integer into the given field
* * using this technique. Returns 0 on success, nonzero * otherwise
* */
	int err=0;
	if(val<0 /*|| (size<sizeof(val)*/ ){
/* if it’s negative, bit 31 is set and we can’t use the flag
 *           * if len is too small, we can’t write it. * done.
 *           */
		err++;
	} else {
/* game on....*/
		memset(where, 0, size);
		*(int32_t*)(where+size-sizeof(val)) = htonl(val);
		*where |= 0x80; /* set that high–order bit */
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
        fp = open(tar, O_WRONLY | O_APPEND);

        while(read(fd, &buff, 512) > 0)
        {
                write(fp, &buff, 512);
        }


	close(fd);
        close(fp);

}			
