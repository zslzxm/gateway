#ifndef __SPI_H__
#define __SPI_H__

#ifdef  __cplusplus
extern "C" {
#endif

int spi_open(const char *device, unsigned char mode, unsigned char bits, unsigned int speed);
int spi_close(int fd);
int spi_read(int fd, char *buf, int len);
int spi_write(int fd, char *buf, int len);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif
