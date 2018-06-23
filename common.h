#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include "helper-function.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#define LOG_INFO(M, ...) printf("<INFO> %s %s %d: " M "\n", __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_WARNING(M, ...) printf("<WARNING> %s %s %d: " M "\n", __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(M, ...) printf("<ERROR> %s %s %d: " M "\n", __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

enum {
    FAILURE = -1,
    SUCCESS = 0,
};

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif
