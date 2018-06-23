#ifndef HELPER_H
#define HELPER_H

#ifdef  __cplusplus
extern "C" {
#endif

void dump_memory(void *buf, int len);
unsigned long get_file_size(char *path);
int read_exactly(int fd, void *buffer, int len);
int write_exactly(int fd, void *buffer, int len);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif // HELPER_H
