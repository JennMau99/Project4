/* Jennifer Maunder and Richa Gadgil
 * CPE 357-03
 * Project 4
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

int maketree(char *path, char type, mode_t mode, int verbose)
{
	struct stat treeStat;
	int fd;
	/* Enters this if the directory or file doesn't exist */
	if (stat(path, &treeStat) != 0)
	{
		if (type == '5')
		{
			mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
			fd = -3;
		}
		else if (type == '2')
			fd = -2;
		else
		{		
			fd = open(path, O_RDWR | O_TRUNC | O_CREAT, 0666);
		}
	}
	else
	{
		if (type == '5')
			fd = -3;
		else if (type == '2')
			fd = -2;
		else
			fd = open(path, O_RDWR | O_TRUNC | O_CREAT);
	}
	if (verbose == 1)
		fprintf(stdout, "%s\n", path);
	return fd;
}

void writesym(int tar, off_t offset)
{

}

void writefile(int fd, int tar, off_t offset)
{
	char *size;
	int intsize;
	char *buffer;
	size = (char *)malloc(sizeof(char) * 12);
	lseek(tar, offset+124, SEEK_SET);
	read(tar, size, 12);
	intsize = strtol(size, NULL, 8);
	buffer = (char *)malloc(sizeof(char) * intsize);
	lseek(tar, offset+512, SEEK_SET);
	read(tar, buffer, intsize);
	write(fd, buffer, intsize);
	close(fd);
}

void checktype(int fd, int tar, off_t offset)
{
	if (fd == -2)
		writesym(tar, offset);
	else if (fd > 0)
		writefile(fd, tar, offset);
}

off_t findoffset(int tar, off_t offset)
{
	char *size;
	int intsize;
	int blocks;
	size = (char *)malloc(sizeof(char) * 12);
	lseek(tar, offset+124, SEEK_SET);
	read(tar, size, 12);
	intsize = strtol(size, NULL, 8);
	if (intsize == 0)
		blocks = 1;
	else if (intsize < 512)
		blocks = 2;
	else if (intsize % 512 != 0)
		blocks = (intsize % 512) + 1;
	else
		blocks = intsize % 512;
	offset = offset + (blocks * 512);
	return offset;
}

/*
mode_t findmode(int tar, off_t offset)
{

}	
*/
void extract(int tar, int verbose)
{
	off_t offset = 0;
	char *bufferptr;
	char *prefixptr;
	char type = '0';
	int lenprefix;
	int lenname;
	int fd;
	char zerocheck = '\0';
	mode_t mode = S_IWUSR;

	while (lseek(tar, offset, SEEK_SET) < lseek(tar, 0, SEEK_END))
	{
		lseek(tar, offset, SEEK_SET);
		bufferptr = (char *)malloc(sizeof(char) * 100);
		prefixptr = (char *)malloc(sizeof(char) * 256);
		read(tar, bufferptr, 100);
		bufferptr[100] = '\0';
		lseek(tar, offset, SEEK_SET);
		lseek(tar, offset+345, SEEK_SET);
		read(tar, prefixptr, 155);
		prefixptr[155] = '\0';
		/* Enters this if the prefix exists */
		if (prefixptr[0] != '\0')
		{
			/* Concatenate prefix with name */
			lenprefix = strlen(prefixptr);
			prefixptr[lenprefix] = '/';
			prefixptr[lenprefix+1] = '\0';
			lenprefix = strlen(prefixptr);
			lenname = strlen(bufferptr);
			strncpy((prefixptr + lenprefix), bufferptr, lenname+1);
		}
		else
		{
			free(prefixptr);
			prefixptr = bufferptr;
		}
		lseek(tar, offset+156, SEEK_SET);
		read(tar, &type, 1);

		/* Add support for findmode when everything else works */
		/*mode = findmode(tar, offset);*/
		/* This should probably return a file descriptor so I can write to it right after maketree */
		lseek(tar, offset, SEEK_SET);
		read(tar, &zerocheck, 1);
		if (zerocheck != '\0')
			fd = maketree(prefixptr, type, mode, verbose);

		/* Call writetree from here */
		checktype(fd, tar, offset); 

		/* increment offset by checking if lseek is at a new file/dir */
		offset = findoffset(tar, offset);
	}
}
/*
int main(int argc, char *argv[])
{
	int tar;
	tar = open(argv[1], O_RDONLY);
	extract(tar, 1);
	return 0;
}
*/
