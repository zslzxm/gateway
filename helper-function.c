#include <sys/stat.h>
#include "common.h"

unsigned long get_file_size(char *path)
{
    unsigned long filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0)
        return filesize;
    else
        filesize = statbuff.st_size;
    return filesize;
}

int read_exactly(int fd, void *buffer, int len)
{
    int n = 0, rc;
    char *p = (char *)buffer;

    while (n != len) {
        rc = read(fd, p, len - n);
        if (rc <= 0)
            return rc;
        n += rc;
        p += rc;
    }
    return n;
}

int write_exactly(int fd, void *buffer, int len)
{
    int n = 0, rc;
    char *p = (char *)buffer;

    while (n != len) {
        rc = write(fd, p, len - n);
        if (rc <= 0)
            return rc;
        n += rc;
        p += rc;
    }
    return n;
}
