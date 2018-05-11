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
#include "header.h"

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
	

	fd = open(tar, O_RDONLY);
	

	if(read(fd, &head, sizeof(header)) > 0)
	{
		/*permissions - mode
		size
		mtime
		name*/
		printf("%s\n",head.mode);
		/*uname*/
		printf("%s\n",head.size);
		printf("%s\n",head.mtime);
		printf("NAME %s\n",head.name);
	} 	
	return 0;


}


