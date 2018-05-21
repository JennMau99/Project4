#ifndef HEADER_H
#define HEADER_H

#include <unistd.h>
#include <sys/types.h>

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
	char hi[12];
};

off_t findoffset(int tar, off_t offset);



int check(int *checkarray, char *argv);

int create(int argc, char **argv);

int list(int argc, char **argv);

int extract(int argc, char *argv[], int verbose);

typedef struct Header header;

#endif
