/*RICHA GADGIL SECTION 03-357 TAYLOR*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

struct Header {

	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char typeflag;
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
};

void make_header(char *file, char filetype);



typedef struct Header header;

int main(int argc, char **argv)
{
	make_header(argv[1], 1);
	return 0;
}


void make_header(char * file, char filetype)
{
	struct stat st;
	header *head;


	stat(file, &st);

	/*name -- clear memset, check prefix later*/
	memset(head->name, 0, 100);
	strcpy(head->name, file);

	memset(head->mode, 0, 8);
	sprintf(head->mode, "%8o", st.st_mode);
	/*have to determine mode's validity?*/

	memset(head->uid, 0, 8); /*check if memsets necessary*/
	sprintf(head->uid, "%lu", (unsigned long int)st.st_uid);

	memset(head->gid, 0, 8); /*check if memsets necessary*/
        sprintf(head->gid, "%lu", (unsigned long int)st.st_gid);	
	
	sprintf(head->size, "%12o", (unsigned int)st.st_size);
	
	sprintf(head->mtime, "%12o", (unsigned int)st.st_mtime);
	/*do checksum at end*/

	/*typeflag depends on what's passed in -- specifiy)*/

	/*linkname if symbolic link -- memset?*/

	strcpy(head->magic, "magic");
	
	strcpy(head->version, "00");

	/*uname and gname? ascii versions of uid and gid?*/

	/*devmajor and devminor unknown*/
	
	/*strcpy(head->uid, st.st_uid);
	strcpy(head->gid, st.st_gid);
	strcpy(head->size, st.st_size);
	*/





}
