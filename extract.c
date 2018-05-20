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
#include <arpa/inet.h>

void writeids(char *path, int tar, off_t offset);


int maketree(char *path, char type, int mode, int verbose, int tar, off_t offset)
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
			if (mode)
				fd = open(path, O_RDWR | O_TRUNC | O_CREAT, 0777);
			else	
				fd = open(path, O_RDWR | O_TRUNC | O_CREAT, 0666);
		}
		writeids(path, tar, offset);
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

uint32_t extractspecialint(char *where, int len)
{
	int32_t val = -1;
	if ((len >= sizeof(val)) && (where[0] & 0x80))
	{
		val = *(int32_t *)(where+len-sizeof(val));
		val = ntohl(val);
	}
	return val;
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
	free(size);
}

void writeids(char *path, int tar, off_t offset)
{
	char *uid;
	char *gid;
	int32_t writeuid;
	int32_t writegid;
	int len;
	uid = (char *)malloc(sizeof(char) * 9);
	gid = (char *)malloc(sizeof(char) * 9);
	uid[8] = '\0';
	gid[8] = '\0';
	lseek(tar, offset+108, SEEK_SET);
	read(tar, uid, 8);
	lseek(tar, offset+116, SEEK_SET);
	read(tar, gid, 8);
	len = strlen(uid);
	writeuid = extractspecialint(uid, len);
	if (writeuid < 0)
		writeuid = atoi(uid);
	writegid = atoi(gid);

	chown(path, writeuid, writegid);
	
	free(uid);
	free(gid);
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
	char version[2];
	size = (char *)malloc(sizeof(char) * 12);	
	lseek(tar, offset+124, SEEK_SET);
	read(tar, size, 12);
	intsize = strtol(size, NULL, 8);
	if (intsize == 0)
		blocks = 1;
	else if (intsize < 512)
		blocks = 2;
	else if (intsize % 512 != 0)
		blocks = (intsize / 512) + 2 ; /* intsize % 512  */
	else
		blocks = intsize / 512 + 1;
	offset = offset + (blocks * 512);
	/*fprintf(stderr, "size was %d\n", intsize);
	fprintf(stderr, "offset is now %d\n", offset);
	*/

	/* Stuff to help avoid corrupted parts of files */
	lseek(tar, offset+263, SEEK_SET);
	read(tar, &version, 2);
	/*if (version[0] != '0' || version[1] != '0')
		return 0;
	*/

	return offset;
}

int findmode(int tar, off_t offset)
{
	char *mode;
	long int newmode;
	lseek(tar, offset+100, SEEK_SET);
	mode = (char *)malloc(sizeof(char) * 9);
	read(tar, mode, 8);
	mode[8] = '\0';
	newmode = strtol(mode, NULL, 8);
	
	if (newmode & S_IXUSR || newmode & S_IXGRP || newmode & S_IXOTH)
		return 1;

	return 0;
}	

int compnames(char *prefixptr, int argc, char *argv[])
{
	int i = 3;
	int givenlen;
	int same = 0;
	char *sameptr;
	char *copyarg;
	int j = 1;
	while (i < argc)
	{
		givenlen = strlen(argv[i]);
		same = strncmp(prefixptr, argv[i], givenlen);
		if (same == 0)
			return 1;

		j = 1;
		copyarg = (char *)malloc(sizeof(char) * givenlen);
		while (argv[i][givenlen-j] != '/')
		{
			j++;
			givenlen -= 1;
		}
		strncpy(copyarg, argv[i], givenlen);

		sameptr = strstr(copyarg, prefixptr);
		if (sameptr != NULL)
			return 1;
		i++;
	}
	return 0;
}

int extract(int argc, char *argv[], int verbose)
{
	off_t offset = 0;
	char *bufferptr;
	char *prefixptr;
	char type = '0';
	int lenprefix;
	int lenname;
	int fd;
	int tar;
	int namesgiven = 0;
	int same = 0;
	char zerocheck = '\0';
	int mode = 0;
	

	tar = open(argv[2], O_RDONLY, 0700);

	if (argc > 3)
		namesgiven = 1;

	if (tar < 0)
	{
		fprintf(stderr, "Error opening tar file\n");
		return -1;
	}

	while (lseek(tar, offset, SEEK_SET) < (lseek(tar, 0, SEEK_END) - 1024))
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

		if (namesgiven)
			same = compnames(prefixptr, argc, argv);

		if (!namesgiven || same)
		{
			if (zerocheck != '\0')
			{
				mode = findmode(tar, offset);
				fd = maketree(prefixptr, type, mode, verbose, tar, offset);
			}
		
			/* Call writetree from here */
			checktype(fd, tar, offset); 
		}
		/* increment offset by checking if lseek is at a new file/dir */
		offset = findoffset(tar, offset);
		if (offset == 0)
		{
			fprintf(stderr, "Corrupted tar file, ending extract\n");
			return -1;
		}
	}
	return 0;
}
